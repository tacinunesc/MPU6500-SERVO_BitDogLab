#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo.h"

uint16_t inclinacao(float angulo) {
    // Converte ângulo para duração de pulso em microsegundos
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;
    return (uint16_t)((pulso / 20.0f) * 20000.0f);
}

void inicializar_pwm_servo() {
    // Configura PWM no pino do servo motor
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(slice_num, &config, true);
}

float mover_servo_por_eixo_x(float acc_x) {
    // Mapeia aceleração X para ângulo de 0 a 180 graus
    float angulo = 90 + (acc_x * 90);
    if (angulo < 0) angulo = 0;
    if (angulo > 180) angulo = 180;

    pwm_set_gpio_level(SERVO_PIN, inclinacao(angulo));
    return angulo;
}
