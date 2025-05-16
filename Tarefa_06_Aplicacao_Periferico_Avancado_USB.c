#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"

// Definições dos pinos
#define LED_VERMELHO 13
#define LED_VERDE    11
#define LED_AZUL     12
#define BUZZER       10

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
            uint32_t count = tud_cdc_read(buf, sizeof(buf));
            buf[count] = '\0'; // Garante término da string

            // ECO: Envia de volta o comando
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();

            // Verifica comandos
            if (strstr((char*)buf, "vermelho")) {
                acende_led(LED_VERMELHO);
            } else if (strstr((char*)buf, "verde")) {
                acende_led(LED_VERDE);
            } else if (strstr((char*)buf, "azul")) {
                acende_led(LED_AZUL);
            } else if (strstr((char*)buf, "som")) {
                toca_buzzer();
            }
        }
        tud_task(); // Mantém o USB funcionando
    }

    return 0;
}
