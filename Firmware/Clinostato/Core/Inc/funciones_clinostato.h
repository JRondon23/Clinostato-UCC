/*
 * funciones_clinostato.h
 *
 *  Created on: 8 oct 2026
 *      Author: Valentin26
 */

#ifndef INC_FUNCIONES_CLINOSTATO_H_
#define INC_FUNCIONES_CLINOSTATO_H_

#define BUFFER_SIZE 60
#define PULSOS_POR_VUELTA 60.0f //revisar porque da 60
#define MAX_PWM 100
#define TIEMPO_PID 0.01f
#define PID_KP 8.0f//3.5f
#define PID_KI 0.2f//1.0f

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

float Filtro_Promedio(uint32_t delta, uint8_t motor);
float Calcular_RPM(float valor);
void Mover_Motor(float duty_porcentaje, uint8_t motor,uint32_t *ccr_actual);
void PID(uint8_t motor, float set_point, float rpm, float *salida_debug,uint32_t *ccr_actual);
;

#endif /* INC_FUNCIONES_CLINOSTATO_H_ */
