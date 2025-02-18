#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "lib/ssd1306.h"
#include "lib/font.h"

#define LED_1 11
#define LED_2 12
#define LED_3 13
#define BTN_JOY 22
#define BTN_A 5
#define JOY_X 26
#define JOY_Y 27
#define I2C_SDA 14
#define I2C_SCL 15

//Trecho para modo BOOTSEL com botão B
#include "pico/bootrom.h"
#define botaoB 6
void gpio_irq_handler(uint gpio, uint32_t events)
{
  reset_usb_boot(0, 0);
}

volatile bool toggle_green = false;
volatile bool toggle_leds = true;
volatile int border_style = 0;
ssd1306_t display;

void btn_joy_irq_handler(uint gpio, uint32_t events) {
    toggle_green = !toggle_green;
    border_style = (border_style + 1) % 3;
    gpio_put(LED_1, toggle_green);  // Corrigido: LED_1 em vez de LED_GREEN
}

void btn_a_irq_handler(uint gpio, uint32_t events) {
    toggle_leds = !toggle_leds;
}

void setup_pwm(uint gpio) {
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_wrap(slice_num, 4095);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), 0);
    pwm_set_enabled(slice_num, true);
}

void set_pwm(uint gpio, uint level) {
    uint slice_num = pwm_gpio_to_slice_num(gpio);
    pwm_set_chan_level(slice_num, pwm_gpio_to_channel(gpio), level);
}

void update_display(int x, int y) {
    ssd1306_clear(&display);
    int pos_x = (x * (128 - 8)) / 4095;
    int pos_y = (y * (64 - 8)) / 4095;
    ssd1306_draw_rect(&display, pos_x, pos_y, 8, 8, 1);
    ssd1306_show(&display);
}

int main() {
    stdio_init_all();
    gpio_init(LED_1);
    gpio_init(LED_2);
    gpio_init(LED_3);
    gpio_set_dir(LED_1, GPIO_OUT);
    gpio_set_dir(LED_2, GPIO_OUT);
    gpio_set_dir(LED_3, GPIO_OUT);
    
    gpio_init(BTN_JOY);
    gpio_set_dir(BTN_JOY, GPIO_IN);
    gpio_pull_up(BTN_JOY);
    gpio_set_irq_enabled_with_callback(BTN_JOY, GPIO_IRQ_EDGE_FALL, true, &btn_joy_irq_handler);
    
    gpio_init(BTN_A);
    gpio_set_dir(BTN_A, GPIO_IN);
    gpio_pull_up(BTN_A);
    gpio_set_irq_enabled_with_callback(BTN_A, GPIO_IRQ_EDGE_FALL, true, &btn_a_irq_handler);
    
    setup_pwm(LED_1);
    setup_pwm(LED_2);
    
    adc_init();
    adc_gpio_init(JOY_X);
    adc_gpio_init(JOY_Y);
    
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    ssd1306_init(&display, 128, 64, false, 0x3C, i2c0);  // Corrigido    
    while (1) {
        adc_select_input(0);
        int x_val = adc_read();
        adc_select_input(1);
        int y_val = adc_read();
        
        if (toggle_leds) {
            set_pwm(LED_1, abs(x_val - 2048) * 2);
            set_pwm(LED_2, abs(y_val - 2048) * 2);
        } else {
            set_pwm(LED_1, 0);
            set_pwm(LED_2, 0);
        }
        
        update_display(x_val, y_val);
        sleep_ms(50);
    }
}