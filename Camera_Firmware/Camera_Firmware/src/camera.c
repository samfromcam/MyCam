/*
 * camera.c
 *
 * Created: 2/20/2025 1:22:42 PM
 *  Author: snb5582
 */ 

#include "camera.h"
#include "board.h"
#include "pio.h"
#include "pdc.h"
#include "pmc.h"
#include "twi.h"
#include "sam4s.h"
#include <stdbool.h>
sysclk_init();

// VSYNC Interrupt Handler
void VSYNC_Handler(uint32_t ul_id, uint32_t ul_mask) {
	if (ul_mask & VSYNC) {
		vsync_flag = 1;
		PIOA->PIO_ISR;  // Clear interrupt status
	}
}


void init_vsync_interrupts(void) {
	// 1. Enable PIOA peripheral clock
	pmc_enable_periph_clk(ID_PIOA);
	
	// 2. Clear any pending interrupts
	PIOA->PIO_ISR;

	// 3. Configure VSYNC pin (PA15) as input with pull-up
	pio_set_input(PIOA, VSYNC, PIO_PULLUP | PIO_DEBOUNCE);

	// 4. Configure interrupt handler for rising edge
	pio_handler_set(PIOA,
	ID_PIOA,          // Peripheral identifier
	VSYNC,            // Pin mask (PA15)
	PIO_IT_RISE_EDGE, // Trigger condition
	VSYNC_Handler);   // Callback function

	// 5. Enable interrupt for VSYNC pin
	pio_enable_interrupt(PIOA, VSYNC);

	// 6. Configure NVIC (Nested Vectored Interrupt Controller)
	NVIC_EnableIRQ(PIOA_IRQn);       // Enable PIOA interrupt line
	NVIC_SetPriority(PIOA_IRQn, 4);  // Set medium priority level
}


void configure_twi(void) {
	pmc_enable_periph_clk(ID_TWI0);
	twi_options_t options;
	options.master_clk = sysclk_get_cpu_hz();
	options.speed = TWI_CLOCK_FREQ;
	options.chip = OV_I2C_SENSOR_ADDRESS;
	pio_configure(PIOA, PIO_PERIPH_A, PIO_PA3A_TWCK0 | PIO_PA4A_TWD0, PIO_DEFAULT);
	
	// 5. Set pull-up resistors on SDA/SCL lines
	PIOA->PIO_PUER = PIO_PA3 | PIO_PA4;
	
	// 6. Verify communication
	uint32_t whoami;
	twi_packet_t packet;
	packet.addr[0] = OV2640_PIDH;  // PIDH register
	packet.addr_length = 1;
	packet.chip = OV_I2C_SENSOR_ADDRESS;
	packet.buffer = &whoami;
	packet.length = 1;
}

void pio_capture_init(Pio *p_pio, uint32_t ul_id) {
	pmc_enable_periph_clk(ul_id);

	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN); // Disable capture
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF; 
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;
	/* Only HSYNC and VSYNC enabled */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);
}

uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size){
	if ((p_pio->PIO_RCR == 0) && (p_pio->PIO_RNCR == 0)) {
		p_pio->PIO_RPR = (uint32_t)uc_buf;
		p_pio->PIO_RCR = ul_size;
		p_pio->PIO_PTCR = PIO_PTCR_RXTEN;
		return 1;
	} else if (p_pio->PIO_RNCR == 0) {
		p_pio->PIO_RNPR = (uint32_t)uc_buf;
		p_pio->PIO_RNCR = ul_size;
		return 1;
	} else {
		return 0;
	}
}

void init_camera(void){
	//calling the configure_twi() function.
	configure_twi();
	init_vsync_interrupts();
	pio_capture_init();
	pmc_enable_pllbck();
}

void configure_camera(void){
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_320x240);
}

uint8_t start_capture(void){
	//enable_vsync_interupt
	pio_enable_interrupt(PIOA, VSYNC);
	
	if (!vsync_flag)
	{
	}
	
	pio_disable_interrupt(PIOA,VSYNC);
	// Enable capture 
	pio_capture_enable(PIOA);
	pio_capture_to_buffer(PIOA,*frame_buffers,100);
	
	/* Wait end of capture*/
	while ()
	{
	}
	/* Disable pio capture*/
	pio_capture_disable(PIOA);
	/* Reset vsync flag*/
	vsync_flag = false;
	
	/* Check Size  */
	len_success = 0;
	find_image_len();
	//return len_success;
}

uint8_t find_image_len(void){
	image_size = 0;
	image_started = 0;
	image_ended = 0;
	uint8_t byte;
	uint8_t next_byte;
	uint8_t complete = 0;
	
	for (uint32_t i = 0; i < 100000; ++i){
		
		//look for start of image
		byte = frame_buffers[i];
		next_byte = frame_buffers[i+1];
		
		if (byte == 0xff && next_byte == 0xd8) {
			image_started = 1;
			image_ended = 0;
			start_pos = i;
		}
		else if (byte == 0xff && next_byte == 0xd9 && image_started == 1) {
			image_ended = 1;
			end_pos = i+1;
			len_success = 1;
			break;
		}
		//if (image_started && image_ended) {
		//len_success = 1;
		//break; 			//add +1 to the length counter
		//}
		image_size += image_started;
	}
}

