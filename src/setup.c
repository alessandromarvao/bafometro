#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "hardware/gpio.h"
#include "mq3_sensor/mq3_sensor.h"
#include "oled_config/oled_config.h"
#include "setup.h"


/**
 * Inicializa os componentes a serem utilizados pelo Core0 (foreground)
 */
void init_setup()
{
    // Inicializa os LEDs
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);
    gpio_put(RED_LED_PIN, false);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
    gpio_put(GREEN_LED_PIN, false);

    gpio_init(BLUE_LED_PIN);
    gpio_set_dir(BLUE_LED_PIN, GPIO_OUT);
    gpio_put(BLUE_LED_PIN, false);

    // Inicializa o botão A
    gpio_init(BTN_PIN);
    gpio_set_dir(BTN_PIN, GPIO_IN);
    gpio_pull_up(BTN_PIN);

    // Inicializa o i2c para o display OLED
    i2c_oled_init();
}

/**
 * Realiza a leitura do sensor MQ-3
 */
void mq3_reading()
{
    mq3_init();

    while (true)
    {
        uint16_t response = multicore_fifo_pop_blocking();

        if (response != 0)
        {
            uint16_t higher_data_received = 0;

            absolute_time_t start = get_absolute_time();

            // Faz a leitura do sensor por 10 segundos
            while (absolute_time_diff_us(start, get_absolute_time()) < 10 * 1000000)
            {
                uint16_t data_received = mq3_read();
                MQ3Sensor result = mq3_result(data_received);
                printf("Dado bruto da leitura do MQ-3: %d - %.2f V - %.2f PPM\n", data_received, result.voltagem, result.ppm);

                // Lê o sensor MQ-3
                if (data_received > higher_data_received)
                {
                    higher_data_received = data_received;
                }
                sleep_ms(150);
            }

            // devolve ao core0 o valor lido do sensor
            multicore_fifo_push_blocking(higher_data_received);
        }
    }
}

/**
 * Aguarda o carregamento do monitor serial
 */
void tarefa1()
{
    display_message(0);
    while (!stdio_usb_connected())
    {
        gpio_put(RED_LED_PIN, true);
        gpio_put(GREEN_LED_PIN, true);
        sleep_ms(200);
        gpio_put(RED_LED_PIN, false);
        gpio_put(GREEN_LED_PIN, false);
        sleep_ms(200);
    }
}

/**
 * Aguarda 10 segundos para aquecimento do sensor MQ-3
 */
void tarefa2()
{
    printf("Aguarde 10 segundos enquanto o sensor MQ-3 aquece...\n");
    display_message(3);
    sleep_ms(10000);
}

/**
 * Aguarda o botão A ser pressioando para iniciar a leitura do sensor
 */
void tarefa3()
{
    printf("Pressione o botão A para realizar o monitoramento de alcoolemia.\n");
    display_message(1);
}

void tarefa4(bool *is_reading_sensor)
{
    if (*is_reading_sensor)
    {
        // desliga os LEDs (quando o teste é refeito)
        gpio_put(RED_LED_PIN, false);
        gpio_put(GREEN_LED_PIN, false);
        gpio_put(BLUE_LED_PIN, false);
        
        display_message(2);
        multicore_fifo_push_blocking(1);
        *is_reading_sensor = false;
    }
}

/**
 * Exibe no display o resultado da leitura do sensor MQ-3
 */
void tarefa5(uint16_t value)
{
    MQ3Sensor sensor_data = mq3_result(value);
    char *msg;
    // uint16_t valor_bruto;
    // float ppm;
    // float voltagem;

    // Valores PPM:
    // 0 – 0,5 PPM	Sem álcool (ruído, ambiente)
    // 0,5 – 1 PPM	Traços mínimos, exposição ambiental
    // > 1 - 5 PPM	Consumo moderado
    // > 5     PPM	Consumo significativo detectado

    printf("Valor recebido: %d - %.2f PPM - %.2f V\n", sensor_data.valor_bruto, sensor_data.ppm, sensor_data.voltagem);

    if (sensor_data.ppm < 0.5) { // Sem álcool (ruído, ambiente)
        gpio_put(RED_LED_PIN, true);
        gpio_put(GREEN_LED_PIN, true);
        gpio_put(BLUE_LED_PIN, true);
        printf("Ambiente livre de álcool.\n");
        msg = "Ambiente limpo";
    } else if (sensor_data.ppm < 1) { // Traços mínimos, exposição ambiental
        gpio_put(GREEN_LED_PIN, true);
        printf("Traços mínimos de álcool, exposição ambiental.\n");
        msg = "Baixo consumo";
    } else if (sensor_data.ppm < 5) { // Consumo moderado
        gpio_put(RED_LED_PIN, true);
        gpio_put(GREEN_LED_PIN, true);
        printf("Consumo de álcool moderado.\n");
        msg = "Consu moderado";
    } else { // Consumo significativo detectado
        gpio_put(RED_LED_PIN, true);
        printf("Consumo de álcool significativo detectado.\n");
        msg = "Consumo alto";
    }
    
    display_sensor_data(sensor_data.ppm, msg);
}