/*
 * XPT2046.h
 *
 *  Created on: Jan 27, 2022
 *      Author: asz
 */

#ifndef INC_XPT2046_H_
#define INC_XPT2046_H_

#include "main.h"

class XPT2046_Touchscreen {
	private:
		uint16_t __scale_x=320,
				 __scale_y=240;

		uint8_t __read_x=0xD0,
				__read_y=0x90,
				__no_samples=16;

		uint16_t __min_x = 1900,
				 __max_x = 29800,
				 __min_y = 2300,
				 __max_y = 31300;

		SPI_HandleTypeDef* __bus;
		GPIO_TypeDef* CS_PORT; GPIO_TypeDef* IRQ_PORT;
		uint16_t CS_PIN, IRQ_PIN;
	public:
		XPT2046_Touchscreen(SPI_HandleTypeDef &bus, GPIO_TypeDef& GPIO_CS_PORT, uint16_t GPIO_CS_PIN, GPIO_TypeDef& GPIO_IRQ_PORT, uint16_t GPIO_IRQ_PIN);
		~XPT2046_Touchscreen();

		void setRotation(uint8_t rotation);
		void setSamplesNumber(uint8_t number_of_samples);
		bool pressed(void);
		bool getTouch(uint16_t* x, uint16_t* y);
};

#endif
