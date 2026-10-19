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
	uint32_t set_point;
	uint32_t rpm;

	//Datos de hardware
	TIM_HandleTypeDef *tim_motor; // Puntero al timer (ej. &htim2)
	uint32_t canal_pwm;          // Canal del timer (ej. TIM_CHANNEL_1)

} motor_typedef;

int32_t Filtro_Promedio(uint32_t delta, uint8_t motor);
int32_t Calcular_RPM(int32_t valor);
void Mover_Motor(int32_t rpm, uint8_t motor);
void PID(uint8_t motor, int32_t set_point, int32_t rpm);

#endif /* INC_FUNCIONES_CLINOSTATO_H_ */
