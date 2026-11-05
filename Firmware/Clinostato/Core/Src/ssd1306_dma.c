/*
 * ssd1306_dma.c
 *
 *  Created on: 2 nov 2026
 *      Author: Valentin26
 */


#include "ssd1306_dma.h"

#if !defined(SSD1306_USE_I2C)
#error "Esta variante DMA requiere I2C."
#endif
#if SSD1306_WIDTH != 128 || SSD1306_HEIGHT != 64 || SSD1306_BUFFER_SIZE != 1024
#error "Esta variante DMA requiere una pantalla SSD1306 de 128 x 64."
#endif
#if defined(SSD1306_X_OFFSET) && SSD1306_X_OFFSET != 0
#error "Esta variante DMA requiere columnas 0 a 127, sin desplazamiento."
#endif
#ifndef SSD1306_DMA_TIMEOUT_MS
#define SSD1306_DMA_TIMEOUT_MS 250U
#endif

typedef enum {
    LIBRE, COMANDOS, DATOS_PENDIENTES, IMAGEN, COMANDO_SIMPLE, ERROR_DMA
} EstadoDMA;
static volatile EstadoDMA estado = LIBRE;
static volatile HAL_StatusTypeDef ultimo_estado = HAL_OK;
static uint8_t imagen[SSD1306_BUFFER_SIZE];
static uint8_t comandos[6];
static uint32_t inicio;

static HAL_StatusTypeDef iniciar(uint16_t control, uint8_t *datos, uint16_t cantidad,
                                EstadoDMA siguiente) {
    estado = siguiente;
    inicio = HAL_GetTick();
    HAL_StatusTypeDef resultado = HAL_I2C_Mem_Write_DMA(
        &SSD1306_I2C_PORT, SSD1306_I2C_ADDR, control,
        I2C_MEMADD_SIZE_8BIT, datos, cantidad);
    if (resultado != HAL_OK) {
        ultimo_estado = resultado;
        estado = ERROR_DMA;
    }
    return resultado;
}

static HAL_StatusTypeDef disponible(void) {
    if (estado == ERROR_DMA) return HAL_ERROR;
    if (estado != LIBRE || HAL_I2C_GetState(&SSD1306_I2C_PORT) != HAL_I2C_STATE_READY)
        return HAL_BUSY;
    return HAL_OK;
}

HAL_StatusTypeDef ssd1306_UpdateScreen_DMA(void) {
    HAL_StatusTypeDef resultado = disponible();
    if (resultado != HAL_OK) return resultado;
    ssd1306_CopyFrame(imagen);
    comandos[0] = 0x21; comandos[1] = 0; comandos[2] = 127;
    comandos[3] = 0x22; comandos[4] = 0; comandos[5] = 7;
    ultimo_estado = HAL_OK;
    return iniciar(0x00, comandos, 6, COMANDOS);
}

HAL_StatusTypeDef ssd1306_Command_DMA(uint8_t comando, uint8_t parametro,
                                     uint8_t cantidad) {
    HAL_StatusTypeDef resultado = disponible();
    if (resultado != HAL_OK) return resultado;
    if (cantidad != 1 && cantidad != 2) return HAL_ERROR;
    comandos[0] = comando;
    comandos[1] = parametro;
    ultimo_estado = HAL_OK;
    return iniciar(0x00, comandos, cantidad, COMANDO_SIMPLE);
}

void ssd1306_Process(void) {
    if (estado == DATOS_PENDIENTES) {
        (void)iniciar(0x40, imagen, sizeof(imagen), IMAGEN);
    } else if (estado == COMANDOS || estado == IMAGEN || estado == COMANDO_SIMPLE) {
        if ((uint32_t)(HAL_GetTick() - inicio) >= SSD1306_DMA_TIMEOUT_MS) {
            /* No reutilizar memoria: DMA puede continuar activo. La aplicación
               debe cancelar/recuperar I2C antes de llamar a ResetError. */
            ultimo_estado = HAL_TIMEOUT;
            estado = ERROR_DMA;
        }
    }
}

void ssd1306_DMA_TxCpltCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c != &SSD1306_I2C_PORT) return;
    if (estado == COMANDOS) estado = DATOS_PENDIENTES;
    else if (estado == IMAGEN || estado == COMANDO_SIMPLE) estado = LIBRE;
}

void ssd1306_DMA_ErrorCallback(I2C_HandleTypeDef *hi2c) {
    if (hi2c != &SSD1306_I2C_PORT) return;
    ultimo_estado = HAL_ERROR;
    estado = ERROR_DMA;
}

uint8_t ssd1306_Ocupada(void) { return estado != LIBRE; }
HAL_StatusTypeDef ssd1306_GetLastStatus(void) { return ultimo_estado; }
HAL_StatusTypeDef ssd1306_DMA_ResetError(void) {
    if (estado != ERROR_DMA) return HAL_BUSY;
    if (HAL_I2C_GetState(&SSD1306_I2C_PORT) != HAL_I2C_STATE_READY) return HAL_BUSY;
    if (SSD1306_I2C_PORT.hdmatx != NULL &&
        HAL_DMA_GetState(SSD1306_I2C_PORT.hdmatx) != HAL_DMA_STATE_READY) return HAL_BUSY;
    ultimo_estado = HAL_OK;
    estado = LIBRE;
    return HAL_OK;
}
