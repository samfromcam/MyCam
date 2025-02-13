/*
 * wifi.c
 *
 * Created: 2/12/2025 12:23:09 PM
 *  Author: elian
 */ 

#include <asf.h>
#include "wifi.h"

// WIFI variable initializations:
volatile uint32_t received_byte_wifi = 0;
volatile bool new_rx_wifi = false;
volatile unsigned int input_pos_wifi = 0;
volatile bool wifi_comm_success = false;

volatile uint32_t transfer_index = 0;
volatile uint32_t transfer_len = 0;

void wifi_usart_handler(void) {
	// Handler for incoming data from the WiFi. Should call process incoming byte wifi when a new byte arrives.
	uint32_t ul_status;

	/* Read USART status. */
	ul_status = usart_get_status(WIFI_USART);

	/* Receive buffer is full. */
	if (ul_status & US_CSR_RXBUFF) {
		usart_read(WIFI_USART, &received_byte_wifi);
		new_rx_wifi = true;
		process_incoming_byte_wifi((uint8_t)received_byte_wifi);
	}
}

void process_incoming_byte_wifi(uint8_t in_byte) {
	// Stores every incoming byte (in byte) from the ESP32 in a buffer.

}

void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask) {
	// Handler for “command complete” rising-edge interrupt from ESP32. When this is triggered, it is time to process the response of the ESP32.
	unused(ul_id);
	unused(ul_mask);
	
	wifi_comm_success = true;
	process_data_wifi();
	for (int jj=0;jj<MAX_INPUT_WIFI;jj++) input_line_wifi[jj] = 0;
	input_pos_wifi = 0;
}

void process_data_wifi(void) {
	// Processes the response of the ESP32, which should be stored in the buffer filled by process incoming byte wifi. This processing
	// should be looking for certain responses that the ESP32 should give, such as “SUCCESS” when “test” is sent to it.
	if (strstr(input_line_wifi, "SUCCESS")) {
		ioport_toggle_pin_level(LED_PIN);
	}
}

void wifi_provision_handler(uint32_t ul_id, uint32_t ul_mask) {
	// Handler for button to initiate provisioning mode of the ESP32. Should set a flag indicating a request to initiate provisioning mode.

}

void wifi_spi_handler(void) {
	// Handler for peripheral mode interrupts on SPI bus. When the ESP32 SPI controller requests data, this interrupt should send one byte of the image at a time.
	uint32_t new_cmd = 0;
	static uint16_t data;
	uint8_t uc_pcs;

	if (spi_read_status(SPI) & SPI_SR_RDRF) {
		spi_read(SPI, &data, &uc_pcs);
		
		if (transfer_len--) {
			spi_write(SPI, transfer_index++, 0, 0);
		}
	}
}

void configure_usart_wifi(void) {
	// Configuration of USART port used to communicate with the ESP32.
	gpio_configure_pin(PIN_USART0_RXD_IDX, PIN_USART0_RXD_FLAGS);
	gpio_configure_pin(PIN_USART0_TXD_IDX, PIN_USART0_TXD_FLAGS);
	
	static uint32_t ul_sysclk;
	const sam_usart_opt_t wifi_usart_settings = {
		WIFI_USART_BAUDRATE,
		WIFI_USART_CHAR_LENGTH,
		WIFI_USART_PARITY,
		WIFI_USART_STOP_BITS,
		WIFI_USART_MODE,
		/* This field is only used in IrDA mode. */
		0
	};
	
	/* Get system clock. */
	ul_sysclk = sysclk_get_cpu_hz();
	
	pmc_enable_periph_clk(WIFI_USART_ID);
	
	usart_init_rs232(WIFI_USART,&wifi_usart_settings,ul_sysclk);

	/* Disable all the interrupts. */
	usart_disable_interrupt(WIFI_USART, ALL_INTERRUPT_MASK);
	
	/* Enable TX & RX function. */
	usart_enable_tx(WIFI_USART);
	usart_enable_rx(WIFI_USART);

	usart_enable_interrupt(WIFI_USART, US_IER_RXRDY);

	/* Configure and enable interrupt of USART. */
	NVIC_EnableIRQ(WIFI_USART_IRQn);
}

void configure_wifi_comm_pin(void) {
	// Configuration of “command complete” rising-edge interrupt.
	/* Configure PIO clock. */
	pmc_enable_periph_clk(WIFI_COMM_ID);

	/* Initialize PIO interrupt handler, see PIO definition in conf_board.h
	**/
	pio_handler_set(WIFI_COMM_PIO, WIFI_COMM_ID, WIFI_COMM_PIN_NUM, WIFI_COMM_ATTR, wifi_command_response_handler);

	/* Enable PIO controller IRQs. */
	NVIC_EnableIRQ((IRQn_Type)WIFI_COMM_ID);

	/* Enable PIO interrupt lines. */
	pio_enable_interrupt(WIFI_COMM_PIO, WIFI_COMM_PIN_NUM);
}

void configure_wifi_provision_pin(void) {
	// Configuration of button interrupt to initiate provisioning mode.

}

void configure_spi(void) {
	// Configuration of SPI port and interrupts used to send images to the ESP32.
	gpio_configure_pin(SPI_MISO_GPIO, SPI_MISO_FLAGS);
	gpio_configure_pin(SPI_MOSI_GPIO, SPI_MOSI_FLAGS);
	gpio_configure_pin(SPI_SPCK_GPIO, SPI_SPCK_FLAGS);
	gpio_configure_pin(SPI_NPCS0_GPIO, SPI_NPCS0_FLAGS);
	
	/* Configure SPI interrupts for slave only. */
	NVIC_DisableIRQ(SPI_IRQn);
	NVIC_ClearPendingIRQ(SPI_IRQn);
	NVIC_SetPriority(SPI_IRQn, 0);
	NVIC_EnableIRQ(SPI_IRQn);
}

void spi_peripheral_initialize(void) {
	// Initialize the SPI port as a peripheral (slave) device.
	spi_enable_clock(SPI);
	spi_disable(SPI);
	spi_reset(SPI);
	spi_set_slave_mode(SPI);
	spi_disable_mode_fault_detect(SPI);
	spi_set_peripheral_chip_select_value(SPI, SPI_CHIP_PCS);
	spi_set_clock_polarity(SPI, SPI_CHIP_SEL, SPI_CLK_POLARITY);
	spi_set_clock_phase(SPI, SPI_CHIP_SEL, SPI_CLK_PHASE);
	spi_set_bits_per_transfer(SPI, SPI_CHIP_SEL, SPI_CSR_BITS_8_BIT);
	spi_enable_interrupt(SPI, SPI_IER_RDRF);
	spi_enable(SPI);
}

void prepare_spi_transfer(void) {
	// Set necessary parameters to prepare for SPI transfer.
	transfer_len = 100;
	transfer_index = 0;
}

void write_wifi_command(char* comm, uint8_t cnt) {
	// Writes a command (comm) to the ESP32, and waits either for an acknowledgment (via the “command complete” pin)
	// or a timeout. The timeout can be created by setting the global variable counts to zero, which will automatically increment every second, and waiting while counts < cnt.

}

void write_image_to_web(void) {
	// Writes an image from the SAM4S8B to the ESP32. If the length of the image is zero (i.e. the image is not valid), return. Otherwise,
	// follow this protocol (illustrated in Appendix C):
	// 1. Configure the SPI interface to be ready for a transfer by setting its parameters appropriately.
	// 2. Issue the command “image transfer xxxx”, where xxxx is replaced by the length of the image you want to transfer.
	// 3. The ESP32 will then set the “command complete” pin low and begin transferring the image over SPI.
	// 4. After the image is done sending, the ESP32 will set the “command complete” pin high. The MCU should sense this and then move on.

}