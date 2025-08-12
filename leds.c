#include "pico/stdlib.h"
#include "leds.h"

// Definições dos pinos dos LEDs
#define LED_VERMELHO 13
#define LED_VERDE    11
#define LED_AZUL     12

void configurar_leds(void) {
    gpio_init(LED_VERMELHO); gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(LED_VERDE);    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AZUL);     gpio_set_dir(LED_AZUL, GPIO_OUT);
}

void desligar_leds(void) {
    gpio_put(LED_VERMELHO, 0);
    gpio_put(LED_VERDE, 0);
    gpio_put(LED_AZUL, 0);
}
