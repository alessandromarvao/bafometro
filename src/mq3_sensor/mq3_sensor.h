#ifndef MQ3_SENSOR_H
#define MQ3_SENSOR_H

#define MQ3_PIN 28
#define MQ3_CHANNEL 2
#define RL 10000 // Valor máximo da resistência de carga do sensor
#define RO 600 // Valor aproximado da resistência do sensor ao ar livre (Ro = Rs/60 ~= 1100) 
#define VCC 3.3



// Coeficientes do MQ-3
// Esses valores são aproximadamente corretos para a faixa de 0.1 a 1.0 mg/L (ou 50 a 500 ppm) de álcool segundo a curva do datasheet.
#define A_COEF -1.29
#define B_COEF 1.93

typedef struct {
    uint16_t valor_bruto;
    float ppm;
    float voltagem;
} MQ3Sensor;

/**
 * Inicializa o ADC no pino 28 do Raspberry Pi Pico
 */
void mq3_init();

/**
 * Retorna o valor da leitura do sensor MQ3.
 * @param raw O valor lido no ADC.
 * @return MQ3Sensor Valor lido do sensor MQ3.
 */
MQ3Sensor mq3_result(uint16_t raw);

uint16_t mq3_read();

// Valores PPM:
// 0 – 0,5 PPM	Sem álcool (ruído, ambiente)
// 0,5 – 25 PPM	Traços mínimos, exposição ambiental
// > 25 - 50 PPM	Consumo moderado
// > 50 – 100 PPM	Consumo significativo detectado



#endif