/**
 * \file
 *
 * \brief User board initialization template
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	
	ioport_init();
	
	//ioport_set_pin_dir(WIFI_COMM_PIN, IOPORT_DIR_INPUT); // comm pin
	ioport_set_pin_dir(WIFI_GPIO_22, IOPORT_DIR_INPUT); // network pin
	ioport_set_pin_dir(WIFI_GPIO_32, IOPORT_DIR_INPUT); // client pin
	ioport_set_pin_dir(WIFI_RESET, IOPORT_DIR_OUTPUT);
}
