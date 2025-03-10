#include "config/conf_board.h"
#include "config/conf_clock.h"
#include "wifi.h"
#include "camera.h"
#include "timer_interface.h"
#include "camera_helper/ov2640.h"
#include <asf.h>

int main(void) {
	// Initialize clock and board definitions
	sysclk_init();
	board_init();
	wdt_disable(WDT);
	
	// Configure and start the Timer
	configure_tc();
	
	// Configure WiFi USART, SPI, and pins
	configure_usart_wifi();
	configure_spi();
	configure_wifi_comm_pin();
	configure_wifi_provision_pin();
	spi_peripheral_initialize();
	
	// Initialize and configure camera
	init_camera();
	configure_camera();
	
	// Test write_wifi_command
	// write_wifi_command("ver\r\n", 5);
	// Config indicators
	write_wifi_command("set wlan_gpio 25\r\n", 5);
 	write_wifi_command("set websocket_gpio 26\r\n", 5);
	write_wifi_command("get mac\r\n", 5);
	write_wifi_command("set ap_gpio 27\r\n", 5);
	// Config GPIOs
	write_wifi_command("set comm_gpio 21\r\n", 5);
	write_wifi_command("set clients_gpio 32\r\n", 5);
	write_wifi_command("set net_gpio 22\r\n", 5);
	// Set SPI baud rate
	write_wifi_command("set spi_baud 100000\r\n", 5);
	
	
	// Send test command to WiFi module
	while (!(success_flag == 1)) {
		write_wifi_command("test\r\n", 10);
		
		if (success_flag == 1) {
			break;
		}
		
		ioport_set_pin_level(WIFI_RESET, false);
		delay_ms(100);
		ioport_set_pin_level(WIFI_RESET, true);
		delay_ms(10000);
	}
	
	while (1){
		if (provision_flag){
			write_wifi_command("provision\r\n", 5);
			provision_flag = false;
			while (!ioport_get_pin_level(WIFI_GPIO_22)){ // check network pin
			}
			continue;
		}
		
		bool network_pin_level = ioport_get_pin_level(WIFI_GPIO_22);
					if (!ioport_get_pin_level(WIFI_GPIO_22)){ // check network pin
						continue;
					}
					
					if (!ioport_get_pin_level(WIFI_GPIO_32)){ // check clients pin
						continue;
					}
						
		if (start_capture()){
			write_image_to_web();
		}
	}
}