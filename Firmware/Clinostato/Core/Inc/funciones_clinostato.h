/*
 * funciones_clinostato.h
 *
 *  Created on: 8 oct 2026
 *      Author: Valentin26
 */

#ifndef INC_FUNCIONES_CLINOSTATO_H_
#define INC_FUNCIONES_CLINOSTATO_H_

#define BUFFER_SIZE 60
#define PULSOS_POR_VUELTA 30.0f //revisar porque da 60
#define MAX_PWM 100
#define TIEMPO_PID 0.01f
#define PID_KP 4.0f//8.0f //con cuerpo 8.0 //3.5
#define PID_KI 1.0f//0.8f // con cuerpo: 0.2 //1.0

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
