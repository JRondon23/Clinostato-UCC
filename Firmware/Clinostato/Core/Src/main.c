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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE BEGIN Includes */
#include "funciones_clinostato.h"
#include "math.h"
#include "ssd1306.h"
#include "ssd1306_dma.h"
#include "ssd1306_fonts.h"
#include <stdio.h>
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

volatile HAL_StatusTypeDef estado_oled = HAL_ERROR;
volatile HAL_StatusTypeDef estado_envio_oled = HAL_ERROR;
volatile uint8_t flag_actualizar_oled = 0;
uint8_t contador_oled = 0;
char texto_rpm1[24];

//variables temp para debuguear
float salida_pid = 0.0f;
uint32_t ccr_actual = 0;
float set_point = 15.0f;

volatile float error = 0.0f;
volatile float error_3s = 0.0f, error_5s = 0.0f, error_10s = 0.0f;
uint32_t tiempo_inicio_informe = 0;
uint8_t error_3s_registrado = 0, error_5s_registrado = 0, error_10s_registrado = 0;
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
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  //variables para mostrar en informe
  tiempo_inicio_informe = HAL_GetTick();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C2_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
  volatile uint32_t ccr_inicial = ccr_actual;

  //variables para mostrar en informe
  /* Comprobar que la pantalla responde. */
  estado_oled = HAL_I2C_IsDeviceReady(
      &hi2c2,
      SSD1306_I2C_ADDR,
      3,
      100
  );

  if (estado_oled == HAL_OK) {
      /* Inicialización bloqueante, una sola vez. */
      ssd1306_Init();

      /* Preparar la imagen en RAM. */
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("RPM1: 0.00", Font_7x10, White);

      /* Iniciar su transmisión por DMA. */
      estado_envio_oled = ssd1306_UpdateScreen_DMA();
  }

	//comprobacion de pines
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, ccr_actual);
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_4);
	HAL_TIM_Base_Start_IT(&htim4);

	//comprobacion de pines
	HAL_GPIO_WritePin(M1_P1_GPIO_Port, M1_P1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(M1_P2_GPIO_Port, M1_P2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6, GPIO_PIN_RESET);
	tiempo_inicial = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1) {

		ssd1306_Process();

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

		/* TIM4 solicita el refresco; el dibujo y el envío se hacen fuera de la ISR. */
		if (flag_actualizar_oled && estado_oled == HAL_OK && !ssd1306_Ocupada()) {
			flag_actualizar_oled = 0;
			/* Formato con dos decimales sin habilitar printf de punto flotante. */
			uint32_t rpm1_centesimas = (uint32_t)(rpm1 * 100.0f + 0.5f);
			snprintf(texto_rpm1, sizeof(texto_rpm1), "RPM1: %lu.%02lu",
			         (unsigned long)(rpm1_centesimas / 100U),
			         (unsigned long)(rpm1_centesimas % 100U));
			ssd1306_Fill(Black);
			ssd1306_SetCursor(0, 0);
			ssd1306_WriteString(texto_rpm1, Font_7x10, White);
			estado_envio_oled = ssd1306_UpdateScreen_DMA();
			if (estado_envio_oled == HAL_BUSY) {
				flag_actualizar_oled = 1;
			}
		}

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//variables para mostrar en informe
		error = fabsf(set_point - rpm1);
		uint32_t tiempo_informe = HAL_GetTick() - tiempo_inicio_informe;
		if (!error_3s_registrado && tiempo_informe >= 3000U) {
			error_3s = error;
			error_3s_registrado = 1;
		}
		if (!error_5s_registrado && tiempo_informe >= 5000U) {
			error_5s = error;
			error_5s_registrado = 1;
		}
		if (!error_10s_registrado && tiempo_informe >= 10000U) {
			error_10s = error;
			error_10s_registrado = 1;
		}
		if (error <= 0.5f && flag_pid_listo) {
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
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {


	if (htim->Instance == TIM8 &&
		htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4) {

			uint32_t captura_actual_1 = HAL_TIM_ReadCapturedValue(htim,
			TIM_CHANNEL_4);

			if (primera_captura_1) {
				captura_anterior_1 = captura_actual_1;
				primera_captura_1 = 0;
				return; //sale del if si solo tengo una medicion y no puedo calcular delta
			}
			// TIM8 cuenta 16 bits: diferencia módulo 65536 ante desbordamiento.
			delta_m1 = (uint16_t)(captura_actual_1 - captura_anterior_1);
			captura_anterior_1 = captura_actual_1;
			flag_sensado_m1 = 1;
			tiempo_parado = 0;

	} else if (htim->Instance == TIM2 &&
			   htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
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

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM4) {
		/* 16 MHz / (15 + 1) / (9999 + 1) = 100 interrupciones/s. */
		if (++contador_oled >= 100U) {
			contador_oled = 0;
			flag_actualizar_oled = 1;
		}

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

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
    ssd1306_DMA_TxCpltCallback(hi2c);
}

void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c)
{
    ssd1306_DMA_ErrorCallback(hi2c);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
