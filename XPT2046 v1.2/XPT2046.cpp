/*
 * XPT2046.cpp
 *
 *  Created on: Jan 27, 2022
 *      Author: asz
 */

#include <XPT2046.h>

XPT2046_Touchscreen::XPT2046_Touchscreen(SPI_HandleTypeDef &bus, GPIO_TypeDef& GPIO_CS_PORT, uint16_t GPIO_CS_PIN, GPIO_TypeDef& GPIO_IRQ_PORT, uint16_t GPIO_IRQ_PIN) {
	__bus = &bus;
	CS_PORT	 = &GPIO_CS_PORT;
	CS_PIN	 = GPIO_CS_PIN;

	IRQ_PORT = &GPIO_IRQ_PORT;
	IRQ_PIN  = GPIO_IRQ_PIN;
	setRotation(1);
}

XPT2046_Touchscreen::~XPT2046_Touchscreen() { }

void XPT2046_Touchscreen::setRotation(uint8_t rotation) {
	rotation = rotation % 4;
	switch(rotation) {
		case 0:
			__scale_x = 240;
			__scale_y = 320;
			__read_x  = 0xD0;
			__read_y  = 0x90;
		break;
		case 1:
			__scale_x = 320;
			__scale_y = 240;
			__read_x  = 0x90;
			__read_y  = 0xD0;
		break;
		case 2:
			__scale_x = 240;
			__scale_y = 320;
			__read_x  = 0xD0;
			__read_y  = 0x90;
		break;
		case 3:
			__scale_x = 320;
			__scale_y = 240;
			__read_x  = 0x90;
			__read_y  = 0xD0;
		break;
	}
}

void XPT2046_Touchscreen::setSamplesNumber(uint8_t number_of_samples){
	if(number_of_samples >= 128) __no_samples = 128;
	__no_samples = number_of_samples;
}

bool XPT2046_Touchscreen::pressed(void)
{
    return (IRQ_PORT->IDR & IRQ_PIN) == 0;
}

bool XPT2046_Touchscreen::getTouch(uint16_t* x, uint16_t* y)
{
    CS_PORT->BSRR = (uint32_t)CS_PIN << 16U;

    uint32_t sum_x = 0;
    uint32_t sum_y = 0;
    uint8_t samples = 0;
    uint16_t zorros = 0;

    for(uint8_t i = 0; i < __no_samples; i++)
    {
        if(!pressed())
            break;

        samples++;

        uint8_t y_raw[2];
        uint8_t x_raw[2];

        HAL_SPI_Transmit(__bus, reinterpret_cast<uint8_t*>(&__read_y), 1, HAL_MAX_DELAY);
        HAL_SPI_TransmitReceive(__bus, reinterpret_cast<uint8_t*>(&zorros), y_raw, 2, HAL_MAX_DELAY);

        HAL_SPI_Transmit(__bus, reinterpret_cast<uint8_t*>(&__read_x), 1, HAL_MAX_DELAY);
        HAL_SPI_TransmitReceive(__bus, reinterpret_cast<uint8_t*>(&zorros), x_raw, 2, HAL_MAX_DELAY);


        sum_x += (((uint16_t)x_raw[0]) << 8) | ((uint16_t)x_raw[1]);
        sum_y += (((uint16_t)y_raw[0]) << 8) | ((uint16_t)y_raw[1]);
    }

    CS_PORT->BSRR = (uint32_t)CS_PIN;

    if(samples < __no_samples) return false;

    uint32_t raw_x = (sum_x / __no_samples);
    if(raw_x < __min_x) raw_x = __min_x;
    if(raw_x > __max_x) raw_x = __max_x;

    uint32_t raw_y = (sum_y / __no_samples);
    if(raw_y < __min_y) raw_y = __min_y;
    if(raw_y > __max_y) raw_y = __max_y;

    *x = (raw_x - __min_x) * __scale_x / (__max_x - __min_x);
    *y = (raw_y - __min_y) * __scale_y / (__max_y - __min_y);

    return true;
}
