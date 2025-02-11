/*
 * IncFile1.h
 *
 * Created: 2/10/2025 5:35:05 PM
 *  Author: elian
 */ 

/*
Wifi GPIO 21 -> MCU PA21
Wifi GPIO 22 -> MCU PA22
Wifi GPIO 23 -> MCU PA8
Wifi GPIO 32 -> MCU PA9
Wifi Reset -> MCU PA19 (currently disconnected for testing)
MCU Reset Button -> MCU PA10 (bottom button on breakout board)
Wifi Setup Button -> MCU PA18 (top button on breakout board)
*/

#ifndef INCFILE1_H_
#define INCFILE1_H_

#define WIFI_GPIO_21	PIO_PA21_IDX
#define WIFI_GPIO_22	PIO_PA22_IDX
#define WIFI_GPIO_23	PIO_PA8_IDX
#define WIFI_GPIO_32	PIO_PA9_IDX


#endif /* INCFILE1_H_ */