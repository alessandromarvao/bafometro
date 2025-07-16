#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/time.h"
#include "oled_config/oled_config.h"
#include "setup.h"

uint16_t max_value = 0;

absolute_time_t ultimo_evento = 0;
bool button_triggered = false;

/**
 * Função de interrupção, acionado pelo botão A
 */
void gpio_callback(uint gpio, uint32_t events)
{
    if (events & GPIO_IRQ_EDGE_FALL)
    {
        absolute_time_t agora = get_absolute_time();
        if (absolute_time_diff_us(ultimo_evento, agora) > 300000)
        { // aguarda 300ms para o debouncing
            ultimo_evento = agora;
            button_triggered = true;
        }
    }
}

int main()
{
    stdio_init_all();

    multicore_launch_core1(mq3_reading);

    // Inicializa os pinos GPIO do LED RGB e do botão A
    init_setup();

    // tarefa 1: solicitar a leitura do monitor serial
    tarefa1();

    // tarefa 2: aguardar 10 segundos para aquecimento do sensor MQ3
    tarefa2();

    // tarefa 3: interrupção para aguardar a leitura do sensor
    tarefa3();
    gpio_set_irq_enabled_with_callback(BTN_PIN, GPIO_IRQ_EDGE_FALL, true, gpio_callback);

    while (true) {    
        // tarefa 4: calcular a concentração de álcool e exibir no display oled
        tarefa4(&button_triggered);
        
        if (multicore_fifo_rvalid()) {
            max_value = multicore_fifo_pop_blocking();
            printf("Valor máximo recebido do MQ-3: %d\n", max_value);
            tarefa5(max_value);
        }

        // Mantém o processador em estado de espera
        tight_loop_contents();
    }
}
