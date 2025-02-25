/*
 * wifi.h
 *
 * Created: 2/12/2025 12:22:56 PM
 *  Author: elian
 */ 

/*
WIFI control pin definitions, WIFI UART parameters and pin definitions, WIFI SPI parameters
and pin definitions, WiFi function and variable declarations.

WIFI GPIO 21 -> MCU PA21
WIFI GPIO 22 -> MCU PA22
WIFI GPIO 23 -> MCU PA8
WIFI GPIO 32 -> MCU PA9
WIFI Reset -> MCU PA19
Provisioning Button -> MCU PA18

GPIOs 15 (RX) and 13 (TX) of the WIFI to PA6 and PA5 on MCU (TXD0 and RXD0)
GPIOs 19 (SPCK), 17 (MISO), 18 (MOSI), and 16 (CS) of the WIFI to PA14, PA12, PA13, PA11 on the MCU (SCK0, MISO, MOSI, NPCS0)
*/

#ifndef WIFI_H_
#define WIFI_H_

#include <asf.h>
#include <string.h>
#include <stdio.h>

// WIFI control pins:
#define COMMAND_COMPLETE	PIO_PA21	// command complete pin, WIFI_GPIO_21
#define WIFI_GPIO_22		PIO_PA22
#define WIFI_GPIO_23		PIO_PA8 // CONNECTION PIN
#define WIFI_GPIO_32		PIO_PA9 // CLIENT PIN 
// WIFI reset / setup pins:
#define WIFI_RESET			PIO_PA19

#define WIFI_PROVIS_PIN_NUM			PIO_PA18
#define WIFI_PROVIS_PIO				PIOA
#define WIFI_PROVIS_ID				ID_PIOA
#define WIFI_PROVIS_ATTR				PIO_IT_RISE_EDGE

// WIFI UART parameters:
#define WIFI_USART					USART0
#define WIFI_USART_ID				ID_USART0
#define WIFI_USART_BAUDRATE			115200
#define WIFI_USART_HANDLER			USART0_Handler
#define WIFI_USART_IRQn				USART0_IRQn
#define WIFI_USART_CHAR_LENGTH		US_MR_CHRL_8_BIT
#define WIFI_USART_PARITY			US_MR_PAR_NO
#define WIFI_USART_STOP_BITS		US_MR_NBSTOP_1_BIT
#define WIFI_USART_MODE				US_MR_CHMODE_NORMAL
// WIFI UART pins:
#define PINS_WIFI_USART				(PIO_PA5A_RXD0 | PIO_PA6A_TXD0)
#define PINS_WIFI_USART_FLAGS		(PIO_PERIPH_A | PIO_DEFAULT)
#define PINS_WIFI_USART_MASK		(PIO_PA5A_RXD0 | PIO_PA6A_TXD0)
#define PINS_WIFI_USART_PIO			PIOA
#define PINS_WIFI_USART_ID			ID_PIOA
#define PINS_WIFI_USART_TYPE		PIO_PERIPH_A
#define PINS_WIFI_USART_ATTR		PIO_DEFAULT
// RX pin
#define PIN_USART0_RXD	  {PIO_PA5A_RXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_RXD_IDX        (PIO_PA5_IDX)
#define PIN_USART0_RXD_FLAGS      (PIO_PERIPH_A | PIO_PULLUP)
// TX pin
#define PIN_USART0_TXD    {PIO_PA6A_TXD0, PIOA, ID_PIOA, PIO_PERIPH_A, PIO_PULLUP}
#define PIN_USART0_TXD_IDX        (PIO_PA6_IDX)
#define PIN_USART0_TXD_FLAGS      (PIO_PERIPH_A | PIO_PULLUP)

// WIFI SPI pins:
// MISO pin:
#define SPI_MISO_GPIO         (PIO_PA12_IDX)
#define SPI_MISO_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
// MOSI pin:
#define SPI_MOSI_GPIO         (PIO_PA13_IDX)
#define SPI_MOSI_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
// SPCK pin:
#define SPI_SPCK_GPIO         (PIO_PA14_IDX)
#define SPI_SPCK_FLAGS       (PIO_PERIPH_A | PIO_DEFAULT)
// chip select 0 pin:
#define SPI_NPCS0_GPIO         (PIO_PA11_IDX)
#define SPI_NPCS0_FLAGS        (PIO_PERIPH_A | PIO_DEFAULT)
// WIFI SPI parameters:
// chip select:
#define SPI_CHIP_SEL 0
#define SPI_CHIP_PCS spi_get_pcs(SPI_CHIP_SEL)
// clock polarity:
#define SPI_CLK_POLARITY 0
// clock phase:
#define SPI_CLK_PHASE 0//1
// delay before SPCK:
#define SPI_DLYBS 0x40
// delay between consecutive transfers:
#define SPI_DLYBCT 0x10

// all interrupt mask:
#define ALL_INTERRUPT_MASK  0xffffffff

#define WIFI_COMM_PIN_NUM			PIO_PB10
#define WIFI_COMM_PIO				PIOB
#define WIFI_COMM_ID				ID_PIOB
#define WIFI_COMM_ATTR				PIO_IT_RISE_EDGE

// WIFI variable definitions:
#define MAX_INPUT_WIFI 1000
volatile char input_line_wifi[MAX_INPUT_WIFI];
volatile uint32_t received_byte_wifi;
volatile bool new_rx_wifi;
volatile unsigned int input_pos_wifi;
volatile bool wifi_comm_success;

volatile uint32_t transfer_index;
volatile uint32_t transfer_len;

volatile uint8_t provision_flag = 0;

// Timer counter
#define TC_FREQ             1
volatile uint8_t counts;

void configure_tc(void);

// WIFI function definitions:
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