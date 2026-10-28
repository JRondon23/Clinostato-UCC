/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "funciones_clinostato.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//Captura de velocidad
volatile uint32_t primera_captura_1 = 1, captura_anterior_1 = 0, delta_m1 = 0;
volatile uint32_t primera_captura_2 = 1, captura_anterior_2 = 0, delta_m2 = 0;
volatile uint8_t hay_captura = 0;
volatile uint8_t flag_sensado_m1 = 0;
volatile uint8_t flag_sensado_m2 = 0;
volatile float rpm1 = 0.0f, rpm2 = 0.0f;
volatile uint32_t motor_detenido = 0;
uint32_t delta_local = 0;

//Motor Parado
uint32_t tiempo_parado = 0, llego_pulso = 0;

//variables temp para debuguear
float salida_pid = 0.0f;
uint32_t ccr_actual = 0;
float set_point = 18.0f;

//temp para debuggerar 2
uint32_t tiempo_pid = 0, tiempo_inicial = 0;
uint8_t flag_pid_listo = 1;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
 	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_TIM2_Init();
	MX_TIM3_Init();
	MX_TIM4_Init();
	/* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, ccr_actual);
	HAL_TIM_Base_Start_IT(&htim4);

	HAL_GPIO_WritePin(M1_P1_GPIO_Port, M1_P1_Pin, 1);
	HAL_GPIO_WritePin(M1_P2_GPIO_Port, M1_P2_Pin, 0);
	tiempo_inicial = HAL_GetTick();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		// Medición habitual del motor 1
		if (flag_sensado_m1 == 1) {
			delta_local = delta_m1;
			rpm1 = Calcular_RPM(Filtro_Promedio(delta_local, 1));
			flag_sensado_m1 = 0;
		}

		// Medición habitual del motor 2
		if (flag_sensado_m2) {
			delta_local = delta_m2;
			rpm2 = Calcular_RPM(Filtro_Promedio(delta_local, 2));
			flag_sensado_m2 = 0;
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
		if ((set_point - rpm1) < 0.001f && flag_pid_listo) {
			tiempo_pid = HAL_GetTick() - tiempo_inicial;
			flag_pid_listo = 0;
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {

	if (htim->Instance == TIM2) {
		if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1) {

			uint32_t captura_actual_1 = HAL_TIM_ReadCapturedValue(htim,
			TIM_CHANNEL_1);

			if (primera_captura_1) {
				captura_anterior_1 = captura_actual_1;
				primera_captura_1 = 0;
				return; //sale del if si solo tengo una medicion y no puedo calcular delta
			}
			delta_m1 = captura_actual_1 - captura_anterior_1;
			captura_anterior_1 = captura_actual_1;
			flag_sensado_m1 = 1;
			tiempo_parado = 0;

		} else {
			uint32_t captura_actual_2 = HAL_TIM_ReadCapturedValue(htim,
			TIM_CHANNEL_2);
			if (primera_captura_2) {
				captura_anterior_2 = captura_actual_2;
				primera_captura_2 = 0;
				return; //sale del if si solo tengo una medicion y no puedo calcular delta
			}
			delta_m2 = captura_actual_2 - captura_anterior_2;
			captura_anterior_2 = captura_actual_2;
			flag_sensado_m2 = 1;
		}
	}

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4) {

		//Si no llegan pulsos en 1 segundo pone rpm en 0 (tengo que hacer esto para el otro motor)
		if (!flag_sensado_m1) {
			tiempo_parado++;
		}
		if (tiempo_parado == 100) {
			rpm1 = 0;
		}

		PID(1, set_point, rpm1, &salida_pid, &ccr_actual);
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
	}
	/* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
