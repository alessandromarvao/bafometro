#ifndef SETUP_H
#define SETUP_H

// Definição dos pinos GPIO do LED RGB
#define RED_LED_PIN 13
#define GREEN_LED_PIN 11
#define BLUE_LED_PIN 12

// Definição do pino GPIO do botão A
#define BTN_PIN 5

/**
 * Inicializa os componentes a serem utilizados pelo Core0 (foreground)
 */
void tarefa0();

/**
 * Realiza a leitura do sensor MQ-3
 */
void mq3_reading();

/**
 * Aguarda o carregamento do monitor serial
 */
void tarefa1();

/**
 * Aguarda 10 segundos para aquecimento do sensor MQ-3
 */
void tarefa2();

void tarefa3();

void tarefa4(bool *is_reading_sensor);

/**
 * Exibe no display o resultado da leitura do sensor MQ-3
 */
void tarefa5(uint16_t value);

/**
 * Função de interrupção, acionado pelo botão A
 */
void gpio_callback(uint gpio, uint32_t events);

#endif