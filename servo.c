#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "servo.h"
#include "leds.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

uint16_t inclinacao(float angulo) {
    float pulso = 0.5f + (angulo / 180.0f) * 2.0f;
    return (uint16_t)((pulso / 20.0f) * 20000.0f);
}

void inicializar_pwm_servo() {
    gpio_set_function(SERVO_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(SERVO_PIN);
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, 125.0f);
    pwm_config_set_wrap(&config, 20000);
    pwm_init(slice_num, &config, true);
}

float mover_servo_por_eixos(float acc_x, float acc_y, float acc_z) {
    static float angulo_anterior = -1;

    float angulo_raw = atan2f(acc_x, sqrtf(acc_y * acc_y + acc_z * acc_z)) * (180.0f / M_PI);
    float angulo = 90.0f + angulo_raw;

    if (angulo < 0) angulo = 0;
    if (angulo > 180) angulo = 180;

    int faixa = angulo / 5.625f;
    switch (faixa) {
        case 0: angulo = 0; break;
        case 1: angulo = 10; break;
        case 2: angulo = 15; break;
        case 3: angulo = 20; break;
        case 4: angulo = 25; break;
        case 5: angulo = 30; break;
        case 6: angulo = 35; break;
        case 7: angulo = 40; break;
        case 8: angulo = 45; break;
        case 9: angulo = 50; break;
        case 10: angulo = 55; break;
        case 11: angulo = 65; break;
        case 12: angulo = 75; break;
        case 13: angulo = 80; break;
        case 14: angulo = 85; break;
        case 15: angulo = 90; break;
        case 16: angulo = 95; break;
        case 17: angulo = 100; break;
        case 18: angulo = 105; break;
        case 19: angulo = 110; break;
        case 20: angulo = 115; break;
        case 21: angulo = 120; break;
        case 22: angulo = 125; break;
        case 23: angulo = 130; break;
        case 24: angulo = 135; break;
        case 25: angulo = 140; break;
        case 26: angulo = 145; break;
        case 27: angulo = 150; break;
        case 28: angulo = 155; break;
        case 29: angulo = 160; break;
        case 30: angulo = 165; break;
        case 31: angulo = 170; break;
        case 32: angulo = 175; break;
        default: angulo = 180; break;
    }

    float direcao = (acc_y > 0.5f || acc_y < -0.5f) ? -1.0f : 1.0f;
    float passo = ((angulo > angulo_anterior) ? 1 : -1) * direcao;

    if (angulo_anterior < 0) angulo_anterior = angulo;

    if (angulo != angulo_anterior) {
        for (float a = angulo_anterior + passo; passo > 0 ? a <= angulo : a >= angulo; a += passo) {
            pwm_set_gpio_level(SERVO_PIN, inclinacao(a));
            sleep_ms(10);
        }
        angulo_anterior = angulo;
    }

    desligar_leds();
    if (fabs(acc_x) > fabs(acc_y) && fabs(acc_x) > fabs(acc_z)) {
        if (acc_x < -0.2f) gpio_put(13, 1);
        else if (acc_x > 0.2f) gpio_put(11, 1);
        else gpio_put(12, 1);
    } else if (fabs(acc_y) > fabs(acc_x) && fabs(acc_y) > fabs(acc_z)) {
        if (acc_y < -0.2f) gpio_put(13, 1);
        else if (acc_y > 0.2f) gpio_put(11, 1);
        else gpio_put(12, 1);
    } else {
        if (acc_z > 0.8f) gpio_put(12, 1);
        else gpio_put(13, 1);
    }

    float pulso_ms = 1.5f;
    uint16_t nivel_pwm = (uint16_t)((pulso_ms / 20.0f) * 20000.0f);
    pwm_set_gpio_level(SERVO_PIN, nivel_pwm);

    return angulo;
}
