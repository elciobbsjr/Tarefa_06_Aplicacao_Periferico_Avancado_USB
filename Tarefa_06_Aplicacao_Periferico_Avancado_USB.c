/*
 * ============================================================================
 * TAREFA SEU1C6 - APLICAÇÃO USB (CDC COM IDENTIFICAÇÃO VISUAL)
 *
 * ALUNO: ÉLCIO BERILO BARBOSA DOS SANTOS JÚNIOR
 * MATRÍCULA: 20251RSE.MTC0024
 *
 * DESCRIÇÃO:
 * Este projeto implementa uma aplicação USB CDC utilizando a placa BitDogLab,
 * baseada no microcontrolador RP2040 (Raspberry Pi Pico W). A aplicação
 * responde a comandos enviados pelo computador através do Monitor Serial do VSCode,
 * faz eco (envia de volta o comando recebido) e realiza uma identificação visual e sonora
 * por meio de LEDs e buzzer, além de exibir o comando em um display OLED.
 *
 * FUNCIONALIDADES IMPLEMENTADAS:
 * - Comunicação via USB usando a biblioteca TinyUSB (tusb.h)
 * - Recepção de comandos através do Monitor Serial
 * - Eco (resposta) do comando via USB
 * - Acionamento de LEDs correspondentes aos comandos "vermelho", "verde", "azul"
 * - Comando adicional "som" que aciona o buzzer
 * - Exibição do comando recebido em display OLED (I2C)
 *
 * GPIOs UTILIZADOS:
 * - LED VERDE: GPIO 11
 * - LED AZUL:  GPIO 12
 * - LED VERMELHO: GPIO 13
 * - BUZZER: GPIO 10
 * - I2C SDA: GPIO 14 | I2C SCL: GPIO 15
 *
 * COMO UTILIZAR:
 * 1. Conecte a placa BitDogLab ao computador via cabo USB.
 * 2. No VSCode, abra o Monitor Serial.
 * 3. Envie comandos: "vermelho", "verde", "azul" ou "som".
 * 4. Observe:
 *    - Eco da palavra no Monitor Serial.
 *    - LED correspondente acende por 1 segundo.
 *    - "som" aciona o buzzer.
 *    - O display OLED exibe o comando.
 *
 * ============================================================================
 */

#include <stdio.h>              // Biblioteca padrão de entrada/saída
#include <string.h>             // Biblioteca para manipulação de strings
#include "pico/stdlib.h"        // Biblioteca padrão da Raspberry Pi Pico
#include "hardware/i2c.h"       // Controle da interface I2C
#include "tusb.h"               // TinyUSB: biblioteca para USB CDC
#include "ssd1306.h"            // Biblioteca do display OLED
#include "ssd1306_i2c.h"        // Controle do display via I2C

// Definições dos pinos conforme especificado para a BitDogLab
#define LED_VERMELHO 13
#define LED_VERDE    11
#define LED_AZUL     12
#define BUZZER       10
#define I2C_SDA_PIN  14
#define I2C_SCL_PIN  15

// Objeto global que representa o display OLED
ssd1306_t display;

// Função para inicializar os periféricos (LEDs e buzzer)
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

// Função para inicializar o display OLED via I2C
void init_display() {
    i2c_init(i2c1, 400 * 1000); // Inicializa I2C com 400kHz
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    ssd1306_init_bm(&display, 128, 64, false, ssd1306_i2c_address, i2c1);
    ssd1306_config(&display);
    ssd1306_init(); // Inicialização necessária do display
}

// Função para limpar o conteúdo do display
void limpa_display() {
    memset(display.ram_buffer + 1, 0, display.bufsize - 1);
    ssd1306_send_data(&display);
}

// Exibe o texto informado no display OLED
void imprime_display(char *texto) {
    limpa_display();
    ssd1306_draw_string(display.ram_buffer + 1, 0, 0, texto);
    ssd1306_send_data(&display);
}

// Exibe no display as instruções iniciais dos comandos disponíveis
void exibe_instrucoes_display() {
    limpa_display();
    ssd1306_draw_string(display.ram_buffer + 1, 0, 0, "Comandos:");
    ssd1306_draw_string(display.ram_buffer + 1, 0, 10, "vermelho - LED");
    ssd1306_draw_string(display.ram_buffer + 1, 0, 20, "verde    - LED");
    ssd1306_draw_string(display.ram_buffer + 1, 0, 30, "azul     - LED");
    ssd1306_draw_string(display.ram_buffer + 1, 0, 40, "som      - Buzzer");
    ssd1306_send_data(&display);
}

// Acende o LED correspondente por 1 segundo
void acende_led(int gpio) {
    gpio_put(gpio, 1);
    sleep_ms(1000);
    gpio_put(gpio, 0);
}

// Aciona o buzzer por 1 segundo
void toca_buzzer() {
    gpio_put(BUZZER, 1);
    sleep_ms(1000);
    gpio_put(BUZZER, 0);
}

int main() {
    stdio_init_all();         // Inicializa comunicação padrão
    init_perifericos();       // Inicializa LEDs e buzzer
    init_display();           // Inicializa o display OLED

    exibe_instrucoes_display(); // Mostra os comandos disponíveis

    // Aguarda até a conexão via USB ser detectada
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
            buf[count] = '\0'; // Finaliza string recebida

            // Envia de volta o comando recebido (ECO)
            tud_cdc_write(buf, count);
            tud_cdc_write_flush();

            // Mostra o comando no display
            imprime_display((char *)buf);

            // Verifica qual comando foi recebido e executa a ação
            if (strstr((char *)buf, "vermelho")) {
                acende_led(LED_VERMELHO);
            } else if (strstr((char *)buf, "verde")) {
                acende_led(LED_VERDE);
            } else if (strstr((char *)buf, "azul")) {
                acende_led(LED_AZUL);
            } else if (strstr((char *)buf, "som")) {
                toca_buzzer();
            }

            // Após executar, retorna as instruções ao display
            sleep_ms(1000);
            exibe_instrucoes_display();
        }

        // Mantém a pilha USB da TinyUSB operando corretamente
        tud_task();
    }

    return 0;
}
