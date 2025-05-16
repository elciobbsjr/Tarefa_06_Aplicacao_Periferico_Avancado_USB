#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "tusb.h"
#include "ssd1306.h"
#include "ssd1306_i2c.h"

// Definições dos pinos
#define LED_VERMELHO 13
#define LED_VERDE    11
#define LED_AZUL     12
#define BUZZER       10
#define I2C_SDA_PIN  14
#define I2C_SCL_PIN  15

// Objeto global para o display
ssd1306_t display;

void init_perifericos() {
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);

    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);

    gpio_init(BUZZER);
    gpio_set_dir(BUZZER, GPIO_OUT);
}

void init_display() {
    i2c_init(i2c1, 400 * 1000); // 400 kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init_bm(&display, 128, 64, false, ssd1306_i2c_address, i2c1);
    ssd1306_config(&display);
    ssd1306_init(); // Inicialização obrigatória para evitar distorções
}

void limpa_display() {
    memset(display.ram_buffer + 1, 0, display.bufsize - 1);
    ssd1306_send_data(&display);
}

void imprime_display(char *texto) {
    limpa_display();
    ssd1306_draw_string(display.ram_buffer + 1, 0, 0, texto);
    ssd1306_send_data(&display);
}

void acende_led(int gpio) {
    gpio_put(gpio, 1);
    sleep_ms(1000);
    gpio_put(gpio, 0);
}

void toca_buzzer() {
    gpio_put(BUZZER, 1);
    sleep_ms(1000); // Toca por 1 segundo
    gpio_put(BUZZER, 0);
}

int main() {
    stdio_init_all();
    init_perifericos();
    init_display();

    // Aguarda conexão USB
    while (!tud_cdc_connected()) {
        sleep_ms(100);
    }

    printf("USB conectado!\n");
    printf("Digite:\n");
    printf(" - 'vermelho', 'verde' ou 'azul' para acender o LED correspondente.\n");
    printf(" - 'som' para acionar o buzzer.\n");

    while (true) {
        if (tud_cdc_available()) {
            uint8_t buf[64];
            uint32_t count = tud_cdc_read(buf, sizeof(buf) - 1);
            buf[count] = '\0'; // Garante término da string

            // ECO no USB Serial
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();

            // Exibe no display OLED
            imprime_display((char *)buf);

            // Verifica comandos
            if (strstr((char *)buf, "vermelho")) {
                acende_led(LED_VERMELHO);
            } else if (strstr((char *)buf, "verde")) {
                acende_led(LED_VERDE);
            } else if (strstr((char *)buf, "azul")) {
                acende_led(LED_AZUL);
            } else if (strstr((char *)buf, "som")) {
                toca_buzzer();
            }
        }
        tud_task(); // Mantém o USB funcionando
    }

    return 0;
}
