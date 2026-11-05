/*
 * ssd1306_dma.h
 *
 *  Created on: 2 nov 2026
 *      Author: Valentin26
 */

#ifndef SSD1306_DMA_H
#define SSD1306_DMA_H
#include "ssd1306.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Llamar desde el programa principal, nunca desde una interrupción. */
HAL_StatusTypeDef ssd1306_UpdateScreen_DMA(void);
HAL_StatusTypeDef ssd1306_Command_DMA(uint8_t comando, uint8_t parametro, uint8_t cantidad);
void ssd1306_Process(void);
uint8_t ssd1306_Ocupada(void);
HAL_StatusTypeDef ssd1306_GetLastStatus(void);
/* Reenviar desde las callbacks HAL existentes, sin reemplazarlas. */
void ssd1306_DMA_TxCpltCallback(I2C_HandleTypeDef *hi2c);
void ssd1306_DMA_ErrorCallback(I2C_HandleTypeDef *hi2c);
/* Tras recuperar el bus y detener DMA, permite salir de un error. */
HAL_StatusTypeDef ssd1306_DMA_ResetError(void);
/* Uso interno: copia la imagen dibujada, sin transmitir. */
void ssd1306_CopyFrame(uint8_t *destino);
#ifdef __cplusplus
}
#endif


#endif /* INC_SSD1306_DMA_H_ */
