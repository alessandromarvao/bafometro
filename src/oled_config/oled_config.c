#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "inc/ssd1306.h"
#include "oled_config.h"

const uint I2C_SDA = 14;
const uint I2C_SCL = 15;

bool is_i2c_running;

extern uint8_t ssd[];

const uint8_t big_char_degree[64] = {
    0x0E, 0x00, 0x11, 0x00, 0x11, 0x00, 0x0E, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


void ssd1306_clear_display(uint8_t *ssd) {
    memset(ssd, 0, ssd1306_buffer_length);
    struct render_area area = {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };
    calculate_render_area_buffer_length(&area);
    render_on_display(ssd, &area);
    sleep_ms(10);
}

/**
 * Inicializa a conexão i2C com o display.
 */
void i2c_oled_init()
{
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();
}

/**
 * Imprime os valores passados como parâmetro no display OLED
 */
void display_message(uint8_t menu)
{
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    char temp_formatted[30];
    int vert_alignment;

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];

    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    ssd1306_clear_display(ssd);
// 0: Pressione A para iniciar teste
// 1: Realizando teste...
// 2: Sem presença significativa de alcool
// 3: Presença moderada de alcool detectada
// 4: Presença forte de alcool detectada

    switch (menu) {
        case 0:
            ssd1306_draw_string(ssd, 20, 15, "Inicializar");
            ssd1306_draw_string(ssd, 35, 32, "Monitor");
            ssd1306_draw_string(ssd, 37, 48, "Serial");
            break;
        case 1:
            ssd1306_draw_string(ssd, 10, 18, "Iniciar Teste");
            ssd1306_draw_string(ssd, 18, 40, "Pressione A");
            break;
        case 2:
            ssd1306_draw_string(ssd, 20, 16, "Realizando");
            ssd1306_draw_string(ssd, 37, 32, "Teste");
            break;
        case 3:
            ssd1306_draw_string(ssd, 20, 16, "Carregando...");
            break;
    }

    render_on_display(ssd, &frame_area);
}

void display_sensor_data(float ppm)
{
    struct render_area frame_area = {
        start_column : 0,
        end_column : ssd1306_width - 1,
        start_page : 0,
        end_page : ssd1306_n_pages - 1
    };

    char temp_formatted[30];
    int vert_alignment;

    calculate_render_area_buffer_length(&frame_area);

    uint8_t ssd[ssd1306_buffer_length];

    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, &frame_area);

    ssd1306_clear_display(ssd);

    // uint16_t value, float ppm, float voltage

    // Valores PPM:
    // 0 – 0,5 PPM	Sem álcool (ruído, ambiente)
    // 0,5 – 25 PPM	Traços mínimos, exposição ambiental
    // > 25 - 50 PPM	Consumo moderado
    // > 50 – 100 PPM	Consumo significativo detectado

    if (ppm < 0.5) { // Sem álcool (ruído, ambiente)
        printf("Ambiente livre de álcool.\n");
        ssd1306_draw_string(ssd, 10, 20, "Ambiente limpo");
    } else if (ppm < 1) { // Traços mínimos, exposição ambiental
        printf("Traços mínimos de álcool, exposição ambiental.\n");
        ssd1306_draw_string(ssd, 10, 20, "Baixo consumo");
    } else if (ppm < 5) { // Consumo moderado
        printf("Consumo de álcool moderado.\n");
        ssd1306_draw_string(ssd, 10, 20, "Consumo moderado");
    } else { // Consumo significativo detectado
        printf("Consumo de álcool significativo detectado.\n");
        ssd1306_draw_string(ssd, 10, 20, "Consumo alto");
    }
    
    char linha_status[32];
    snprintf(linha_status, sizeof(linha_status), "%.2f PPM", ppm);
    ssd1306_draw_string(ssd, 30, 45, linha_status);

    render_on_display(ssd, &frame_area);
    
}