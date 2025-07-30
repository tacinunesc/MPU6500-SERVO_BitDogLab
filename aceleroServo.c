#include <stdio.h>                   // Biblioteca padrão de entrada/saída
#include "pico/stdlib.h"             // Funções essenciais da Raspberry Pi Pico
#include "hardware/i2c.h"            // Comunicação I2C com periféricos
#include "hardware/pwm.h"            // Controle de sinais PWM (servo)
#include "mpu6500.h"                 // Biblioteca para o sensor MPU6500
#include "inc/ssd1306.h"             // Biblioteca de controle do display OLED
#include "inc/ssd1306_fonts.h"       // Fontes utilizadas pelo display OLED


#define I2C_PORT i2c0                // Define uso da interface I2C0
#define I2C_SDA 0                    // Pino SDA: GPIO 0
#define I2C_SCL 1                    // Pino SCL: GPIO 1

#define SERVO_PIN 2                 // Pino de controle do servo motor via PWM

#define LED_AZUL     12             // LED azul 
#define LED_VERDE    11             // LED verde 
#define LED_VERMELHO 13             // LED vermelho 


void configurar_leds() {
    // Inicializa os LEDs e define como saída
    gpio_init(LED_AZUL);     gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_init(LED_VERDE);    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
}

void desligar_leds() {
    // Desliga todos os LEDs simultaneamente
    gpio_put(LED_AZUL, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_VERMELHO, 0);
}


uint16_t inclinacao(float angulo) {
    // Converte ângulo (0–180°) para ciclo de trabalho em microsegundos
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;
    return (uint16_t)((pulso / 20.0f) * 20000.0f); // Mapeia para intervalo de 20 ms
}


void inicializar_pwm_servo() {
    // Configura PWM para o pino do servo motor
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);   
    pwm_config_set_wrap(&config, 20000);      
    pwm_init(slice_num, &config, true);
}

float mover_servo_por_eixo_x(float acc_x) {
    // Converte a aceleração no eixo X em ângulo do servo (0–180°)
    float angulo = 90 + (acc_x * 90); 
    if (angulo < 0) angulo = 0;
    if (angulo > 180) angulo = 180;

    pwm_set_gpio_level(SERVO_PIN, inclinacao(angulo)); // Aplica pulso PWM
    return angulo; // Retorna o ângulo atual
}

int main() {
    stdio_init_all();                       // Inicializa comunicação via USB
    while (!stdio_usb_connected()) sleep_ms(100);  // Aguarda conexão
    printf("Iniciando sistema: MPU6500 + Servo + LEDs\n");


    i2c_init(I2C_PORT, 400 * 1000);         // Inicializa I2C a 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);                  
    gpio_pull_up(I2C_SCL);

    ssd1306_Init();                         // Inicializa OLED
    ssd1306_Fill(Black);                    // Limpa tela
    ssd1306_UpdateScreen();                 // Atualiza visualmente


    mpu6500_init(I2C_PORT);                 // Inicia sensor MPU6500
    configurar_leds();                      // Configura LEDs
    desligar_leds();                        // Desliga todos
    inicializar_pwm_servo();                // Configura PWM para servo


    while (1) {
        mpu6500_data_t dados;
        mpu6500_read_raw(I2C_PORT, &dados);

        float acc_x = dados.accel[0] / 16384.0f;
        float acc_y = dados.accel[1] / 16384.0f;
        float acc_z = dados.accel[2] / 16384.0f;

        float angulo = mover_servo_por_eixo_x(acc_x);

        // --- Controle de LEDs baseado na inclinação ---
        desligar_leds(); // Desliga tudo antes de ativar o correto

        if (acc_x < -0.5f) {                     // Inclinação à esquerda
            gpio_put(LED_VERMELHO, 1);
        } else if (acc_x > 0.5f) {               // Inclinação à direita
            gpio_put(LED_VERDE, 1);
        } else {                                 // Posição neutra
            gpio_put(LED_AZUL, 1);
        }
        
        // --- Exibição no terminal ---
        printf("Acel X: %.2fg | Servo: %.0f°\n", acc_x, angulo);

        // --- Atualiza display OLED com dados atuais ---
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

        ssd1306_UpdateScreen();
        sleep_ms(500);
    }

    return 0;
}
