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

#include "conf_board.h"
#include "conf_clock.h"
#include "camera_helper/ov2640.h"

#define TWI_CLK     (400000UL)

void vsync_handler(uint32_t ul_id, uint32_t ul_mask) {
	unused(ul_id);
	unused(ul_mask);
	g_vsync_flag = 1;
}


void init_vsync_interrupts(void) {
	pio_handler_set(PIOA, ID_PIOA, CAMERA_VSYNC_PIN, PIO_IT_RISE_EDGE, vsync_handler);
	NVIC_EnableIRQ(PIOA_IRQn);
}


void configure_twi(void) {
	twi_options_t opt;

	/* Init Vsync handler*/
	init_vsync_interrupts();

	/* Init PIO capture*/
	pio_capture_init(PIOA, ID_PIOA);

	/* Init PCK0 to work at 24 Mhz */
	/* 96/4=24 Mhz */
	PMC->PMC_PCK[0] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLA_CLK);
	PMC->PMC_SCER = PMC_SCER_PCK0;
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK0)) {
	}

	/* Enable TWI peripheral */
	pmc_enable_periph_clk(BOARD_TWI_ID);

	/* Init TWI peripheral */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = TWI_CLK;
	twi_master_init(BOARD_TWI, &opt);

	/* Configure TWI interrupts */
	NVIC_DisableIRQ(BOARD_TWI_IRQn);
	NVIC_ClearPendingIRQ(BOARD_TWI_IRQn);
	NVIC_SetPriority(BOARD_TWI_IRQn, 0);
	NVIC_EnableIRQ(BOARD_TWI_IRQn);

	/* ov7740 Initialization */
	//while (ov_init(BOARD_TWI) == 1) {
	//}

}

void pio_capture_init(Pio *p_pio, uint32_t ul_id) {
	/* Enable periphral clock */
	pmc_enable_periph_clk(ul_id);

	/* Disable pio capture */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_PCEN);

	/* Disable rxbuff interrupt */
	p_pio->PIO_PCIDR |= PIO_PCIDR_RXBUFF;

	/* 32bit width*/
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_DSIZE_Msk);
	p_pio->PIO_PCMR |= PIO_PCMR_DSIZE_WORD;

	/* Only HSYNC and VSYNC enabled */
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_ALWYS);
	p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_HALFS);

	#if !defined(DEFAULT_MODE_COLORED)
		/* Samples only data with even index */
		p_pio->PIO_PCMR |= PIO_PCMR_HALFS;
		p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_FRSTS);
	#endif
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
	//Configuration of camera pins, camera clock (XCLK), and
	//calling the configure_twi() function.
	configure_twi();
	
	/* Init Vsync handler*/
	init_vsync_interrupts();

	/* Init PIO capture*/
	pio_capture_init(PIOA, ID_PIOA);
	
	gpio_configure_pin(PIN_PCK1, PIN_PCK1_FLAGS);
	
	// Enable XCLCK
	pmc_enable_pllbck(7, 0x1, 1); /* PLLA work at 96 Mhz */ // PA17 is xclck signal
	
	/* Init PCK1 to work at 24 Mhz initialize PLLB*/
	/* 96/4=24 Mhz */
	PMC->PMC_PCK[1] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLB_CLK);
	PMC->PMC_SCER = PMC_SCER_PCK1;
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK1)) {
	}
	
	// Initialize camera and wait to let it adjust  // ASK ILYA
	//while (ov_init(BOARD_TWI) == 1) {
	//}
	
}

void configure_camera(void){
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_320x240);
	delay_ms(3000);
}

uint8_t start_capture(void) {

	pio_enable_interrupt(PIOA, CAMERA_VSYNC_PIN);
	while (!g_vsync_flag) {
		// Wait for VSYNC rising edge
	}
	
	pio_disable_interrupt(PIOA, CAMERA_VSYNC_PIN);
	
	// Enable the capture function to start capture data
	pio_capture_enable(PIOA);
	
	/* Capture data and send it to external SRAM memory thanks to PDC
	 * feature */
	pio_capture_to_buffer(PIOA, g_image_buffer, (100000) >> 2);

	/* Wait end of capture*/
	while (!((PIOA->PIO_PCISR & PIO_PCIMR_RXBUFF) ==
			PIO_PCIMR_RXBUFF)) {
	}

	/* Disable pio capture*/
	pio_capture_disable(PIOA);

	/* Reset vsync flag*/
	g_vsync_flag = 0;
		
}


uint8_t find_image_len(void) {
	uint32_t soi_pos = 0, eoi_pos = 0;
	
	// Find SOI marker
	for (uint32_t i = 0; i < (100000) - 1; i++) {
		if (g_image_buffer[i] == 0xFF && g_image_buffer[i+1] == 0xD8) {
			soi_pos = i;
			break;
		}
	}
	
	// Find EOI marker
	for (uint32_t i = soi_pos; i < (100000) - 1; i++) {
		if (g_image_buffer[i] == 0xFF && g_image_buffer[i+1] == 0xD9) {
			eoi_pos = i + 1;
			break;
		}
	}
	
	if (soi_pos < eoi_pos && eoi_pos > 0) {
		return eoi_pos - soi_pos + 1;
	}
	
	return 0;
}
