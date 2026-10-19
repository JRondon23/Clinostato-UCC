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

int32_t Filtro_Promedio(uint32_t delta, uint8_t motor) {
	static uint32_t motor1_buffer[BUFFER_SIZE] = { 0 },
			motor2_buffer[BUFFER_SIZE] = { 0 };
	static uint32_t indice_1 = 0, indice_2 = 0, flag_primera1 = 0, flag_primera2 = 0;
	int32_t valor_m1 = 0, valor_m2 = 0;

	if (motor == 1) {

		motor1_buffer[indice_1] = delta;
		indice_1 = (indice_1 + 1) % BUFFER_SIZE;
		if (flag_primera1 < 60) {
			flag_primera1++;
			return 40404;
		}
		for (int i = 0; i < BUFFER_SIZE; ++i) {
			valor_m1 += motor1_buffer[i];
		}
		return valor_m1 / BUFFER_SIZE; // se multiplica por 100 para tener 2 cifras despues de la coma
	}
	if (motor == 2) {
		motor2_buffer[indice_2] = delta;
		indice_2 = (indice_2 + 1) % BUFFER_SIZE;
		if (flag_primera2 < 60) {
			flag_primera2++;
			return 40404;
		}
		for (int i = 0; i < BUFFER_SIZE; ++i) {
			valor_m2 += motor2_buffer[i];
		}
		return valor_m2 / BUFFER_SIZE;
	}
	return 40404;
}

int32_t Calcular_RPM(int32_t valor) {
	if(valor == 0){
		return 0;
	}
	return (1000 / valor);

}

void Mover_Motor(int32_t rpm, uint8_t motor) {
	uint32_t valor_rcc = rpm;
	if (motor == 1) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t )rpm);
	}
	if (motor == 2) {
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (uint32_t )rpm);
	}
}

void PID(uint8_t motor, int32_t set_point, int32_t rpm) {
	static int32_t error_proporcional_1 = 0, ep_2 = 0, error_integrador_1 = 0,
			ei_2 = 0, error_1 = 0, error_2 = 0;
	static uint32_t kp = 1, ki = 1, kd = 1;
	uint32_t arr_maximo = 99;

	if (motor == 1) {
		error_proporcional_1 = (set_point - rpm) * kp;
		error_integrador_1 += (set_point - rpm) * ki;
		error_1 = error_proporcional_1 + error_integrador_1;
		rpm = rpm + error_1;

		if (rpm < 0) {
			rpm = 0;
		} else if (rpm >> arr_maximo) {
			rpm = arr_maximo;
		}
		Mover_Motor(rpm, motor);
	}
}
/*
 void Motor_Init(motor_typedef motor, uint8_t num_m,
 TIM_HandleTypeDef *tim_m, uint32_t canal) {

 motor->num_motor = num_m;
 motor.tim_motor = tim_m;
 motor.canal_pwm = canal;
 motor.rpm = 0;
 motor.rpm = 1;

 }

 void Filtro_Promedio2(motor_typedef motor) {

 }
 */
