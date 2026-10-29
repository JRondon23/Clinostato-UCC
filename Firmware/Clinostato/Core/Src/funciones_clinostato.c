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

float Filtro_Promedio(uint32_t delta, uint8_t motor) {
	static uint32_t buffer_m1[BUFFER_SIZE] = { 0 };
	static uint32_t buffer_m2[BUFFER_SIZE] = { 0 };
	static uint32_t indice_m1 = 0, indice_m2 = 0;
	static uint32_t muestras_m1 = 0, muestras_m2 = 0;
	static float promedio_m1 = 0.0f, promedio_m2 = 0.0f;

	uint32_t *buffer;
	uint32_t *indice;
	uint32_t *muestras;
	float *promedio;
	float suma = 0.0f;

	// Seleccionar el motor, se hace asi para no repetir codigo
	if (motor == 1) {
		buffer = buffer_m1;
		indice = &indice_m1;
		muestras = &muestras_m1;
		promedio = &promedio_m1;
	} else if (motor == 2) {
		buffer = buffer_m2;
		indice = &indice_m2;
		muestras = &muestras_m2;
		promedio = &promedio_m2;
	} else {
		return 0.0f;
	}

	if (delta == 0) {
		return *promedio;
	}

	// Almacenar la nueva medición
	buffer[*indice] = delta;

	// Actualizar índice circular
	*indice = (*indice + 1) % BUFFER_SIZE;

	// Contar muestras hasta completar el buffer
	if (*muestras < BUFFER_SIZE) {
		(*muestras)++;
	}

	// Calcular promedio utilizando las muestras disponibles
	for (uint32_t i = 0; i < *muestras; i++) {
		suma += buffer[i];
	}
	*promedio = suma / (float) (*muestras);
	return *promedio;
}

float Calcular_RPM(float tiempo) {
	if (tiempo <= 0.0f) {
		return 0.0f;
	}
	return (60.0f * 500) / (PULSOS_POR_VUELTA * tiempo);
}

void Mover_Motor(float duty_porcentaje, uint8_t motor, uint32_t *ccr_actual) {

	uint32_t canal;
	if (motor == 1) {
		canal = TIM_CHANNEL_1;
	} else if (motor == 2) {
		canal = TIM_CHANNEL_2;
	} else {
		return;
	}

	if (duty_porcentaje < 0.0f) {
		duty_porcentaje = 0.0f;
	} else if (duty_porcentaje > 100.0f) {
		duty_porcentaje = 100.0f;
	}

	uint32_t ccr = (uint32_t) ((duty_porcentaje * (float) MAX_PWM / 100.0f)+ 0.5f);
	*ccr_actual = ccr;
	__HAL_TIM_SET_COMPARE(&htim3, canal, ccr);

}

void PID(uint8_t motor, float set_point, float rpm, float *salida_debug,
		uint32_t *ccr_actual) {

	// integral motor 1
	static float integral_m1 = 0.0f;

	//integral motor 2
	static float integral_m2 = 0.0f;

	//puntero integral que cambia
	float *integral;

	if (motor == 1) {
		integral = &integral_m1;
	} else if (motor == 2) {
		integral = &integral_m2;
	} else {
		return;
	}
	float error = set_point - rpm;

	float integral_aux = *integral + (error * TIEMPO_PID);
	float salida = PID_KP * error + PID_KI * integral_aux;

	if (!((salida > 100.0f && error > 0.0f) || (salida < 0.0f && error < 0.0f))) {

		*integral = integral_aux;
	}

	salida = PID_KP * error + PID_KI * (*integral);

	if (salida > 100.0f) {
		salida = 100.0f;
	}
	if (salida < 0.0f) {
		salida = 0.0f;
	}

	*salida_debug = salida;

	Mover_Motor(salida, motor, ccr_actual);
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
