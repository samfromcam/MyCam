/*
 * wifi.c
 *
 * Created: 2/10/2025 5:44:44 PM
 *  Author: elian
 */ 

#include "wifi.h"

// wifi variable initializations:

void wifi_usart_handler(void) {
// Handler for incoming data from the WiFi. Should call process incoming byte wifi when a new byte arrives.

}

void process_incoming_byte_wifi(uint8_t in_byte) {
// Stores every incoming byte (in byte) from the ESP32 in a buffer.

}

void wifi_command_response_handler(uint32_t ul_id, uint32_t ul_mask) {
// Handler for “command complete” rising-edge interrupt from ESP32. When this is triggered, it is time to process the response of the ESP32.

}

void process_data_wifi(void) {
// Processes the response of the ESP32, which should be stored in the buffer filled by process incoming byte wifi. This processing 
// should be looking for certain responses that the ESP32 should give, such as “SUCCESS” when “test” is sent to it.

}

void wifi_provision_handler(uint32_t ul_id, uint32_t ul_mask) {
// Handler for button to initiate provisioning mode of the ESP32. Should set a flag indicating a request to initiate provisioning mode.

}

void wifi_spi_handler(void) {
// Handler for peripheral mode interrupts on SPI bus. When the ESP32 SPI controller requests data, this interrupt should send one byte of the image at a time.

}

void configure_usart_wifi(void) {
// Configuration of USART port used to communicate with the ESP32.
	
}

void configure_wifi_comm_pin(void) { 
// Configuration of “command complete” rising-edge interrupt.

}

void configure_wifi_provision_pin(void) { 
// Configuration of button interrupt to initiate provisioning mode.

}

void configure_spi(void) { 
// Configuration of SPI port and interrupts used to send images to the ESP32.

}

void spi_peripheral_initialize(void) { 
// Initialize the SPI port as a peripheral (slave) device. 

}

void prepare_spi_transfer(void) { 
// Set necessary parameters to prepare for SPI transfer. 

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