#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "lib/ssd1306.h"
#include "lib/font.h"
#include "pico/bootrom.h"

// Definindo as portas e pinos
#define I2C_PORT i2c1
#define I2C_SDA 14
#define I2C_SCL 15
#define endereco 0x3C
#define JOYSTICK_X_PIN 26
#define JOYSTICK_Y_PIN 27
#define JOYSTICK_PB 22
#define Botao_A 5
#define LED_AZUL_PIN 12
#define LED_VERMELHO_PIN 13 // Adicionando o LED vermelho
#define botaoB 6

// Função de interrupção para modo BOOTSEL com botão B
void gpio_irq_handler(uint gpio, uint32_t events) {
    reset_usb_boot(0, 0);
}

// Função para configurar o PWM para um LED
void setup_pwm_for_led(uint gpio_pin, uint channel) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);
    pwm_set_wrap(slice_num, 255);
    pwm_set_chan_level(slice_num, channel, 0);
    pwm_set_enabled(slice_num, true);
}

int main() {
    stdio_init_all();

    // Inicializando o modo BOOTSEL com botão B
    gpio_init(botaoB);
    gpio_set_dir(botaoB, GPIO_IN);
    gpio_pull_up(botaoB);
    gpio_set_irq_enabled_with_callback(botaoB, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    gpio_init(JOYSTICK_PB);
    gpio_set_dir(JOYSTICK_PB, GPIO_IN);
    gpio_pull_up(JOYSTICK_PB);

    gpio_init(Botao_A);
    gpio_set_dir(Botao_A, GPIO_IN);
    gpio_pull_up(Botao_A);

    // Inicializando PWM para LEDs azul e vermelho
    setup_pwm_for_led(LED_AZUL_PIN, PWM_CHAN_A);
    setup_pwm_for_led(LED_VERMELHO_PIN, PWM_CHAN_B);

    // Inicializando I2C e display
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_t ssd;
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, endereco, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Inicializando o ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_X_PIN);
    adc_gpio_init(JOYSTICK_Y_PIN);

    uint16_t adc_value_x, adc_value_y;
    char str_x[5];
    char str_y[5];
    bool cor = true;

    while (true) {
        adc_select_input(0);
        adc_value_x = adc_read();
        adc_select_input(1);
        adc_value_y = adc_read();
        sprintf(str_x, "%d", adc_value_x);
        sprintf(str_y, "%d", adc_value_y);

        // Atualiza o brilho do LED azul com base no eixo Y do joystick
        uint8_t brilho_azul = 0;
        if (adc_value_y < 2048) {
            brilho_azul = (uint8_t)((2048 - adc_value_y) * 255 / 2048);
        } else if (adc_value_y > 2048) {
            brilho_azul = (uint8_t)((adc_value_y - 2048) * 255 / 2048);
        }
        pwm_set_chan_level(pwm_gpio_to_slice_num(LED_AZUL_PIN), PWM_CHAN_A, brilho_azul);

        // Atualiza o brilho do LED vermelho com base no eixo X do joystick
        uint8_t brilho_vermelho = 0;
        if (adc_value_x < 2048) {
            brilho_vermelho = (uint8_t)((2048 - adc_value_x) * 255 / 2048);
        } else if (adc_value_x > 2048) {
            brilho_vermelho = (uint8_t)((adc_value_x - 2048) * 255 / 2048);
        }
        pwm_set_chan_level(pwm_gpio_to_slice_num(LED_VERMELHO_PIN), PWM_CHAN_B, brilho_vermelho);

        // Atualiza o conteúdo do display com animações
        ssd1306_fill(&ssd, !cor);
        ssd1306_rect(&ssd, 3, 3, 122, 60, cor, !cor);
        ssd1306_line(&ssd, 3, 25, 123, 25, cor);
        ssd1306_line(&ssd, 3, 37, 123, 37, cor);
        ssd1306_draw_string(&ssd, "CEPEDI   TIC37", 8, 6);
        ssd1306_draw_string(&ssd, "EMBARCATECH", 20, 16);
        ssd1306_draw_string(&ssd, "ADC   JOYSTICK", 10, 28);
        ssd1306_draw_string(&ssd, "X    Y    PB", 20, 41);
        ssd1306_line(&ssd, 44, 37, 44, 60, cor);
        ssd1306_draw_string(&ssd, str_x, 8, 52);
        ssd1306_line(&ssd, 84, 37, 84, 60, cor);
        ssd1306_draw_string(&ssd, str_y, 49, 52);
        ssd1306_rect(&ssd, 52, 90, 8, 8, cor, !gpio_get(JOYSTICK_PB));
        ssd1306_rect(&ssd, 52, 102, 8, 8, cor, !gpio_get(Botao_A));
        ssd1306_rect(&ssd, 52, 114, 8, 8, cor, !cor);
        ssd1306_send_data(&ssd);

        sleep_ms(100);
    }

    return 0;
}
