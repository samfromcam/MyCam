/*
 * wifi.h
 *
 * Created: 2/10/2025 5:35:05 PM
 *  Author: elian
 */ 

/*
WiFi control pin definitions, WiFi UART parameters and pin definitions, WiFi SPI parameters
and pin definitions, WiFi function and variable declarations.

Wifi GPIO 21 -> MCU PA21
Wifi GPIO 22 -> MCU PA22
Wifi GPIO 23 -> MCU PA8
Wifi GPIO 32 -> MCU PA9
Wifi Reset -> MCU PA19 (currently disconnected for testing)
MCU Reset Button -> MCU PA10 (bottom button on breakout board)
Wifi Setup Button -> MCU PA18 (top button on breakout board)

GPIOs 15 (RX) and 13 (TX) of the WiFi to PA6 and PA5 on MCU (TXD0 and RXD0)
GPIOs 19 (SPCK), 17 (MISO), 18 (MOSI), and 16 (CS) of the WiFi to PA14, PA12, PA13, PA11 on the MCU (SCK0, MISO, MOSI, NPCS0)
*/

#ifndef WIFI_H_
#define WIFI_H_

#include <asf.h>

// wifi control pins:
#define WIFI_GPIO_21	PIO_PA21
#define WIFI_GPIO_22	PIO_PA22
#define WIFI_GPIO_23	PIO_PA8
#define WIFI_GPIO_32	PIO_PA9
// wifi reset / setup pins:
#define WIFI_RESET		PIO_PA19
#define WIFI_SETUP		PIO_PA18

// wifi uart parameters:

// wifi uart pins:
#define TX				PIO_PA6
#define RX				PIO_PA7

// wifi spi parameters:

// wifi spi pins:
#define SCK				PIO_PA14
#define MISO			PIO_PA12
#define MOSI			PIO_PA13
#define CS				PIO_PA11

// wifi function and variable definitions:
void wifi_usart_handler(void);
void process_incoming_byte_wifi(uint8_t in_byte);
void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask);
void process_data_wifi(void); 
void wifi_provision_handler(uint32_t ul_id, uint32_t ul_mask);
void wifi_spi_handler(void);
void configure_usart_wifi(void);
void configure_wifi_comm_pin(void);
void configure_wifi_provision_pin(void);
void configure_spi(void);
void spi_peripheral_initialize(void);
void prepare_spi_transfer(void);
void write_wifi_command(char* comm, uint8_t cnt)
void write_image_to_web(void)

#endif /* WIFI_H_ */