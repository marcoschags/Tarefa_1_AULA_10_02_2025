#include <stdio.h> 
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"  // Inclusão para PWM

#define LED_AZUL_PIN 12    // Usando GPIO 12 para o LED azul
#define JOYSTICK_Y_PIN 27  // GPIO para eixo Y do joystick

void setup_pwm_for_led(uint gpio_pin) {
    gpio_set_function(gpio_pin, GPIO_FUNC_PWM);  // Configura o pino como PWM
    uint slice_num = pwm_gpio_to_slice_num(gpio_pin);  // Obtém o número do slice do PWM
    pwm_set_wrap(slice_num, 255);  // Define o valor máximo para o PWM (255)
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);  // Inicializa o PWM com valor 0 (LED apagado)
    pwm_set_enabled(slice_num, true);  // Habilita o PWM no pino
}

int main() {
    stdio_init_all();

    // Inicializando o PWM para o LED azul no GPIO 12
    setup_pwm_for_led(LED_AZUL_PIN);

    // Inicializando o ADC para o joystick
    adc_init();
    adc_gpio_init(JOYSTICK_Y_PIN);

    uint16_t adc_value_y;
    uint8_t brilho = 0;

    while (true) {
        adc_select_input(1); // Seleciona o ADC para o eixo Y
        adc_value_y = adc_read();  // Lê o valor analógico do joystick

        // Calcula o brilho com base no valor do eixo Y (joystick)
        if (adc_value_y < 2048) {
            brilho = (uint8_t)((2048 - adc_value_y) * 255 / 2048);  // Joystick para cima (valores menores)
        } else if (adc_value_y > 2048) {
            brilho = (uint8_t)((adc_value_y - 2048) * 255 / 2048);  // Joystick para baixo (valores maiores)
        }

        // Atualiza o PWM para controlar o brilho do LED azul
        pwm_set_chan_level(pwm_gpio_to_slice_num(LED_AZUL_PIN), PWM_CHAN_A, brilho);  

        printf("adc_value_y: %d, brilho: %d\n", adc_value_y, brilho);  // Imprime para depuração

        sleep_ms(100);  // Atraso para reduzir a quantidade de leituras
    }

    return 0;
}
