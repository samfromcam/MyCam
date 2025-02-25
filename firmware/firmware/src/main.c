/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# "Insert system clock initialization code here" comment
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>

int main (void)
{
	/* Insert system clock initialization code here (sysclk_init()). */

	board_init();
	
	//Logic
	
	//init everything from wifi and camera (system, timeout_timer, wifi, cam)
	//Reset wifi module (why) // 
	// wait for connection --> if connected want to sent test --> if connection !success go back to the start. 
	//// if connection == success begin the main loop
		////check for wifi provision flag (function from wifi.c =) 
		/////if flag -> put the chip into provision mode
		///// else: check to the network 
			//check for any connected client
			// if client 
				// take picture
		//else:
			//retry
		
	

	/* Insert application code here, after the board has been initialized. */
}
