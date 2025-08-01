# 🏃‍♀️ Sensor de Movimento MPU6500 + Servo Motor atuando com Pico W (BitDogLab)
![Linguagem](https://img.shields.io/badge/Linguagem-C-blue.svg)
![Plataforma](https://img.shields.io/badge/Plataforma-Raspberry%20Pi%20Pico-purple.svg)
![Sensor](https://img.shields.io/badge/Sensor-MPU6500-yellow.svg)
![Sensor](https://img.shields.io/badge/Servo-Motor-green.svg)

## Sobre o Projeto
Este projeto visa criar um firmware para controlar e vizualizar a inclinação do
acelerômetro (MPU6500), utilizando em conjunto um servo motor, LEDs RGB e um
display Oled inclusos na placa Raspberry Pi Pico W (BItDogLab).

<div align="center">
  <img src="img/mpu verde.jpg "  alt="Controle" width="30%">
  <img src="img/mpu vermelho.jpg "  alt="Controle" width="30%">
 <img src="img/mpu azul.jpg "  alt="Controle" width="30%">
</div>

## 🛠️ Estrutura do projeto
- aceleroServo.c – Programa principal em C que faz leitura movimento, com base nesta informação utiliza o servo motor girar no especificado no código e essa informação é exibida no porta serial e no visor oled da BitDogLab
- mpu6500.c - Onde fica as definições do sensor de movimento
- Pasta inc - Onde esta localizada as informações da oled
- CMakeLists.txt – Configuração do build usando o Pico SDK

- ## 🔌 Requisitos
Hardware:

- Raspberry Pi Pico W
- Sensor de Movimento (MPU6500)
- Servo Motor 9g
- Adaptador do Servo Motor
- LEDs

## ⚙️ Como usar
1- Clone o repositorio

2- Deploy no Pico W
 - Segure o botão BOOTSEL do Pico W e conecte-o ao PC
 - Clique no botão run no Vscode ou arraste o arquivo .u2 para dentro do disco removível que aparecer
 - O Pico irá reiniciar executando o firmware
   
## 🔧 Funcionamento do Código
O programa realiza as seguintes ações:

1. Inicialização dos periféricos
- Configura o barramento I2C0 (pinos GPIO 0 e 1)
- Inicializa o sensor MPU6500 e o display OLED SSD1306
- Configura os pinos dos LEDs RGB
- Inicializa o sinal PWM para controle do servo motor

2. Leitura da luminosidade
A cada segundo, o sistema:

- Lê a inclinação do acelerômetro com o sensor MPU6500
- Exibe o valor no display OLED
- Acende um LED correspondente ao nível de aceleração:

   * 🔵 Azul: inclinação neutra, no meio

   * 🟢 Verde: inclinação para direita ou cima

   * 🔴 Vermelho: inclinação para esquerda ou baixo
 
3. Controle do servo motor
- O ângulo do servo é ajustado de acordo os valores de aceleração no eixo x
- Com isso, enquando o sensor de movimento detecatar aceleração for menor que -0.5 o led vermelho é ativo e indica que está inclinado para cima, se for mais que 0.5 o led verde será ativado e a inclinação está para baixo e entre -0.5 e 0.5 indica que está no meio.

## 📦 Dependências

mpu6500.h para o sensor de movimento

ssd1306.h e ssd1306_fonts.h para o display OLED
