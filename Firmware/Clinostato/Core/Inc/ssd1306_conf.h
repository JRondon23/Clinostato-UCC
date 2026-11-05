/*
 * ssd1306_conf.h
 *
 *  Created on: 2 nov 2026
 *      Author: Valentin26
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__
/* Configuración inicial para el clinostato: ajustar dirección si corresponde. */
#define STM32G4
#define SSD1306_USE_I2C
#define SSD1306_I2C_PORT hi2c2
#define SSD1306_I2C_ADDR (0x3C << 1)
#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_DMA_TIMEOUT_MS 250U

#endif /* INC_SSD1306_CONF_H_ */
