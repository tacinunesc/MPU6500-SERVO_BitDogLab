#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "mpu6500.h"
#include "inc/ssd1306.h"
#include "inc/ssd1306_fonts.h"
#include "servo.h"
#include "leds.h"

#define I2C_PORT i2c0
#define I2C_SDA 0
#define I2C_SCL 1

int main() {
    stdio_init_all();
    while (!stdio_usb_connected()) sleep_ms(100);
    printf("Sistema iniciado: MPU6500 + Servo + LEDs\n");

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
    inicializar_pwm_servo();

    char buffer[32];

    while (1) {
        mpu6500_data_t dados;
        mpu6500_read_raw(I2C_PORT, &dados);

        float acc_x = dados.accel[0] / 16384.0f;
        float acc_y = dados.accel[1] / 16384.0f;
        float acc_z = dados.accel[2] / 16384.0f;

        float angulo = mover_servo_por_eixos(acc_x, acc_y, acc_z);

        desligar_leds();

        if (acc_x < -0.5f || acc_y < -0.5f || acc_z < -0.5f) {
            gpio_put(13, 1);
        } else if (acc_x > 0.5f || acc_y > 0.5f || acc_z > 0.5f) {
            gpio_put(11, 1);
        } else {
            gpio_put(12, 1);
        }

        // Determinar direção com base nos eixos
        const char* direcao;
        if (acc_x > 0.5f) direcao = "Dir";
        else if (acc_x < -0.5f) direcao = "Esq";
        else if (acc_y > 0.5f) direcao = "Frente";
        else if (acc_y < -0.5f) direcao = "Tras";
        else if (acc_z > 0.5f) direcao = "Cima";
        else if (acc_z < -0.5f) direcao = "Baixo";
        else direcao = "Neutro";

        printf("Acc X: %.2f | Y: %.2f | Z: %.2f | Servo: %.0f° | Direcao: %s\n", acc_x, acc_y, acc_z, angulo, direcao);

        ssd1306_Fill(Black);
        ssd1306_SetCursor(0, 0);
        ssd1306_WriteString("Monitor MPU/SERVO", Font_7x10, White);

        ssd1306_SetCursor(0, 12);
        snprintf(buffer, sizeof(buffer), "Servo: %.0f graus", angulo);
        ssd1306_WriteString(buffer, Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "EIXO_X: %.2f", acc_x);
        ssd1306_SetCursor(0, 24);
        ssd1306_WriteString(buffer, Font_7x10, White);

        // Linha combinada: EIXO_Y + Direcao
        snprintf(buffer, sizeof(buffer), "Y: %.2f Dir: %s", acc_y, direcao);
        ssd1306_SetCursor(0, 36);
        ssd1306_WriteString(buffer, Font_7x10, White);

        snprintf(buffer, sizeof(buffer), "EIXO_Z: %.2f", acc_z);
        ssd1306_SetCursor(0, 48);
        ssd1306_WriteString(buffer, Font_7x10, White);

        ssd1306_UpdateScreen();
        sleep_ms(500);
    }

    return 0;
}
