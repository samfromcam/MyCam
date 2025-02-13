#include <asf.h>
#include "conf_board.h"
#include "conf_clock.h"
#include "wifi.h"

int main (void)
{
	sysclk_init();
	wdt_disable(WDT);
	board_init();
	
	configure_usart_wifi();
// 	configure_spi();
// 	spi_peripheral_initialize();
	configure_wifi_comm_pin();

	while(1) {
		usart_write_line(WIFI_USART, "test\r\n");
		delay_ms(500);
// 		spi_prep_transfer();
// 		usart_write_line(WIFI_USART, "image_transfer 100\r\n");
// 		delay_ms(1000);
	}
}
