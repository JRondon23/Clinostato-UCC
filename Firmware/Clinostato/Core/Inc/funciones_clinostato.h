/*
 * funciones_clinostato.h
 *
 *  Created on: 8 oct 2026
 *      Author: Valentin26
 */

#ifndef INC_FUNCIONES_CLINOSTATO_H_
#define INC_FUNCIONES_CLINOSTATO_H_

#define BUFFER_SIZE 60

#include "stm32g4xx_hal.h"
//frecuencia pwm = 200hz

typedef struct {
	//variables del motor
	uint8_t num_motor;
	float set_point;
	float rpm;

	//Datos de hardware
	TIM_HandleTypeDef *htim_pwm; // Puntero al timer (ej. &htim2)
	uint32_t canal_pwm;          // Canal del timer (ej. TIM_CHANNEL_1)

} Motor;

float Filtro_Promedio(uint32_t *t_ventana, uint8_t motor);
float Calcular_RPM(float valor);
void Mover_Motor(uint32_t rpm, uint8_t motor);

#endif /* INC_FUNCIONES_CLINOSTATO_H_ */
