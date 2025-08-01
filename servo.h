#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

// Define o pino do servo
#define SERVO_PIN 2

// Inicializa PWM para controle do servo
void inicializar_pwm_servo(void);

// Converte ângulo para ciclo de trabalho PWM
uint16_t inclinacao(float angulo);

// Move servo com base na aceleração no eixo X
float mover_servo_por_eixo_x(float acc_x);

#endif // SERVO_H
