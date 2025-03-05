#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"
#include "camera.h"
#include "timer_interface.h"
#include "camera_helper/ov2640.h"
#include <asf.h>

int main(void) {
	// Initialize clock and board definitions
	sysclk_init();
	board_init();
	
	// Configure and start the Timer
	configure_tc();
	
	// Configure WiFi USART, SPI, and pins
	configure_usart_wifi();
	configure_spi();
	configure_wifi_comm_pin();
	configure_wifi_provision_pin();
	
	// Initialize and configure camera
	init_camera();
	configure_camera();
	
	// Reset WiFi and wait for connection
	ioport_set_pin_level(WIFI_RESET, false);
	delay_ms(100);
	ioport_set_pin_level(WIFI_RESET, true);
	
	while (!ioport_get_pin_level(WIFI_GPIO_23)) { // Choose: connection pin 23
		if (provision_flag) {
			write_wifi_command("provision\r\n", 5);
			provision_flag = 0;
		}
		delay_ms(1000);
	}
	
	// Send test command to WiFi module
	while (1) {
		write_wifi_command("test\r\n", 10);
		//
		if (strstr((char*)input_line_wifi, "SUCCESS")) {
			break;
		}
		ioport_set_pin_level(WIFI_RESET, false);
		delay_ms(100);
		ioport_set_pin_level(WIFI_RESET, true);
		delay_ms(10000);
	}
	
	// Main loop
	while (1) {
		// Check for provisioning request
		if (provision_flag) {
			write_wifi_command("provision\r\n", 5);
			provision_flag = 0;
		}
		
		// Take picture if network available and clients connected
		if (ioport_get_pin_level(WIFI_GPIO_23) && ioport_get_pin_level(WIFI_GPIO_32)) {
			if (start_capture()) {
				write_image_to_web();
			}
		}
		
		delay_ms(100);
	}
}