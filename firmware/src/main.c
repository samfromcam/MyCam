#include <asf.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"
#include "camera.h"
#include "camera_helper/ov2640.h"
#include "timer_interface.h"

volatile uint32_t counts = 0;

void SysTick_Handler(void) {
	counts++;
}

int main (void)
{
	// Initialize clock and board definitions
	sysclk_init();
	wdt_disable(WDT);
	board_init();
	
	// Configure and start the Timer
	SysTick_Config(SystemCoreClock / 1000); // 1ms interrupt
	
	// Configure WiFi
	configure_usart_wifi();
	configure_wifi_comm_pin();
	configure_wifi_provision_pin();
	configure_spi();
	
	// Configure indicators and GPIOs on ESP32
	write_wifi_command("config_led wlan top\r\n", 5);
	write_wifi_command("config_led websockets middle\r\n", 5);
	write_wifi_command("config_led ap bottom\r\n", 5);
	write_wifi_command("config_gpio command_complete 14\r\n", 5);
	write_wifi_command("config_gpio network 12\r\n", 5);
	write_wifi_command("config_gpio clients 13\r\n", 5);
	
	// Initialize and configure camera
	init_camera();
	configure_camera();
	
	// Reset WiFi and wait for connection
	gpio_set_pin_low(WIFI_RESET_PIN);
	delay_ms(100);
	gpio_set_pin_high(WIFI_RESET_PIN);
	
	while (!gpio_pin_is_high(WIFI_NETWORK_PIN)) {
		if (gpio_pin_is_high(WIFI_PROVISION_PIN)) {
			write_wifi_command("provision\r\n", 5);
		}
		delay_ms(1000);
	}
	
	// Send test command to WiFi module
	while (1) {
		write_wifi_command("test\r\n", 10);
		if (strstr(latest_wifi_response, "SUCCESS")) {
			break;
		}
		gpio_set_pin_low(WIFI_RESET_PIN);
		delay_ms(100);
		gpio_set_pin_high(WIFI_RESET_PIN);
		delay_ms(10000);
	}
	
	// Main loop
	while (1) {
		// Check for provisioning request
		if (gpio_pin_is_high(WIFI_PROVISION_PIN)) {
			write_wifi_command("provision\r\n", 5);
		}
		
		// Take picture if network available and clients connected
		if (gpio_pin_is_high(WIFI_NETWORK_PIN) && gpio_pin_is_high(WIFI_CLIENTS_PIN)) {
			if (start_capture()) {
				write_image_to_web();
			}
		}
		
		delay_ms(100);
	}
}
