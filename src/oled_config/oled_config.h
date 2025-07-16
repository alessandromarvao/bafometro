#ifndef OLED_CONFIG_H
#define OLED_CONFIG_H

void i2c_oled_init();

void display_message(uint8_t menu);
// onde:
// 0: Pressione A para iniciar teste
// 1: Realizando teste...
// 2: Sem presença significativa de alcool
// 3: Presença fraca de alcool detectada
// 4: Presença moderada de alcool detectada
// 5: Presença forte de alcool detectada
// 6: Inicialize o monitor serial
// 7: Aguarde, por favor

void display_sensor_data(float ppm);

#endif