#include <stdio.h>                   // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h"             // Funções essenciais da Raspberry Pi Pico
#include "hardware/i2c.h"            // Comunicação I2C com periféricos

#include "mpu6500.h"                 // Sensor MPU6500
#include "inc/ssd1306.h"            // Controle OLED
#include "inc/ssd1306_fonts.h"      // Fontes OLED

#include "servo.h"                   // Controle do servo motor 

// --- Definições das portas utilizadas pelo MPU6500 no i2c0---
#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

//---Definições da portas GPIOs utilizadas pelos LEDs---
#define LED_AZUL     12
#define LED_VERDE    11
#define LED_VERMELHO 13

void configurar_leds() {
    gpio_init(LED_AZUL);     gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE);    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

void desligar_leds() {
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
}

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);
    printf("Sistema iniciado: MPU6500 + Servo + LEDs\n");

    // I2C inicialização
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_Init();
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen();

    mpu6500_init(I2C_PORT);
    configurar_leds();
    desligar_leds();
    inicializar_pwm_servo(); //  Controle servo

       while (1) {
        mpu6500_data_t dados;
        mpu6500_read_raw(I2C_PORT, &dados);  // Lê dados brutos do acelerômetro

        // Converte os dados em valores reais de aceleração (g)
        float acc_x = dados.accel[0] / 16384.0f;
        float acc_y = dados.accel[1] / 16384.0f;
        float acc_z = dados.accel[2] / 16384.0f;

        // Move servo de acordo com aceleração no eixo X
        float angulo = mover_servo_por_eixo_x(acc_x);

        desligar_leds(); // Desliga todos os LEDs antes de decidir o próximo

        // Acende LED conforme a inclinação detectada
        if (acc_x < -0.5f) {
            gpio_put(LED_VERMELHO, 1);  // Inclinação à esquerda
        } else if (acc_x > 0.5f) {
            gpio_put(LED_VERDE, 1);     // Inclinação à direita
        } else {
            gpio_put(LED_AZUL, 1);      // Posição neutra
        }

        // Imprime dados no terminal
        printf("Acel X: %.2fg | Servo: %.0f°\n", acc_x, angulo);

        // Exibe dados no display OLED
        char buffer[32];
        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("Monitor MPU/SERVO", Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "Acel: %.1f acc", acc_x);
        ssd1306_SetCursor(0, 12);
        ssd1306_WriteString(buffer, Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "Servo: %.0f graus", angulo);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString(buffer, Font_7x10, White);

        ssd1306_UpdateScreen(); // Atualiza OLED com os dados

        sleep_ms(500); // Aguarda meio segundo antes da próxima leitura
    }

    return 0;
}
