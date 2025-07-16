#include <stdio.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "mq3_sensor.h"

/**
 * Inicializa o ADC no pino 28 do Raspberry Pi Pico
 */
void mq3_init()
{
    adc_init();
    adc_gpio_init(MQ3_PIN);
    adc_select_input(MQ3_CHANNEL);
}

// Aproximação baseada na curva do datasheet para álcool (etanol)
static float calcular_ppm(float rs_ro_ratio)
{
    // Fórmula: log10(PPM) = a * log10(Rs/Ro) + b
    // Para MQ-3, no gráfico para álcool, aproximadamente:
    // a ≈ -1.497, b ≈ 1.57 (valores aproximados extraídos da curva)
    float log_ratio = log10(rs_ro_ratio);
    float log_ppm = -1.497 * log_ratio + 1.57;
    return pow(10, log_ppm);
}

/**
 * Faz o cálculo da tensão recebida do sensor de acordo com o valor lido no ADC
 */
static float get_tensao(uint16_t result)
{
    float vrl = (float)result * 3.3f / (1 << 12); // tensão recebida na leitura do sensor

    return vrl;
}

uint16_t mq3_read()
{
    return adc_read();
}

/**
 * Retorna o valor da leitura do sensor MQ3.
 * @param raw O valor lido no ADC.
 * @return MQ3Sensor Valor lido do sensor MQ3.
 */
MQ3Sensor mq3_result(uint16_t result)
{
    MQ3Sensor sensor;

    float vrl = get_tensao(result); // tensão recebida na leitura do sensor

    // Para o cálculo da resistência do leitor durante a leitura
    float rs = ((VCC - vrl) * RL) / vrl;

    // Onde:
    // VCC = tensão de alimentação (3.3 V)
    // vrl = tensão lida no pino ADC
    // RL = resistência de carga (10kΩ)
    // rs = resistência do sensor durante a leitura atual.

    float ratio = rs / RO;

    // Aplica a equação logarítmica que aproxima a curva do sensor
    float log_ppm = A_COEF * log10f(ratio) + B_COEF;
    // Faz a exponenciação para voltar do logaritmo ao valor original
    float ppm = powf(10, log_ppm);

    sensor.valor_bruto = result;
    sensor.voltagem = vrl;
    sensor.ppm = ppm;

    return sensor;
}