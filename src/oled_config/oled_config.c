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
            ssd1306_draw_string(ssd, 20, 32, "Teste");
            break;
        case 3: 
            ssd1306_draw_string(ssd, 20, 16, "Sem presenca");
            ssd1306_draw_string(ssd, 20, 32, "de alcool");
            break;
        case 4:
            ssd1306_draw_string(ssd, 20, 16, "Alcool");
            ssd1306_draw_string(ssd, 20, 32, "Detectado");
            ssd1306_draw_string(ssd, 20, 48, "fraco");
            break;
        case 5:
            ssd1306_draw_string(ssd, 20, 16, "Alcool");
            ssd1306_draw_string(ssd, 20, 32, "Detectado");
            ssd1306_draw_string(ssd, 20, 48, "Moderado");
            break;
        case 6:
            ssd1306_draw_string(ssd, 20, 14, "Alcool");
            ssd1306_draw_string(ssd, 20, 30, "Detectado");
            ssd1306_draw_string(ssd, 20, 46, "Intenso");
            break;
        case 7:
            ssd1306_draw_string(ssd, 20, 16, "Carregando...");
            break;
    }

    render_on_display(ssd, &frame_area);
}

void display_sensor_data(uint16_t value)
{
    
}