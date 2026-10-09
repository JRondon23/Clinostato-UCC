/*
 * funciones_clinostato.c
 *
 *  Created on: 8 oct 2026
 *      Author: Valentin26
 */

#include "funciones_clinostato.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_tim.h"
extern TIM_HandleTypeDef htim3;


float Filtro_Promedio(uint32_t *t_ventana, uint8_t motor) {
	static uint32_t motor1_buffer[BUFFER_SIZE] = { 0 },
			motor2_buffer[BUFFER_SIZE] = { 0 };
	static uint32_t indice_1 = 0, indice_2 = 0, valor_m1 = 0, valor_m2 = 0;

	if (motor == 1) {

		motor1_buffer[indice_1] = (t_ventana[1] - t_ventana[0]) / 2;
		indice_1 = (indice_1 + 1) % BUFFER_SIZE;
		for (int i = 0; i < BUFFER_SIZE; ++i) {
			valor_m1 += motor1_buffer[i];
		}
		return valor_m1 / BUFFER_SIZE;
	}
	if (motor == 2) {
		motor2_buffer[indice_2] = (t_ventana[1] - t_ventana[0]) / 2;
		indice_2 = (indice_2 + 1) % BUFFER_SIZE;

		for (int i = 0; i < BUFFER_SIZE; ++i) {
			valor_m2 += motor2_buffer[i];
		}
		return valor_m2 / BUFFER_SIZE;
	}
	return 40404;
}


float Calcular_RPM(float valor){
	return 2000.0f/valor;

}

void Mover_Motor(uint32_t rpm,uint8_t motor){

	if(motor == 1){
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1,rpm);
	}
	if(motor == 2){
		__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2,rpm);
	}
}


