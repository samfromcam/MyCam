/*
 * camera.c
 *
 * Created: 2/20/2025 1:22:42 PM
 *  Author: snb5582
 */

#include "camera.h"

#define TWI_CLK (400000UL);

volatile uint32_t g_vsync_flag = false;
volatile char g_image_buffer[100000];
uint32_t g_image_length = 0;

volatile uint32_t soi_pos = 0; 
volatile uint32_t eoi_pos = 0; 

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

	/* Enable TWI peripheral */
	pmc_enable_periph_clk(BOARD_TWI_ID);

	/* Init TWI peripheral */
	opt.master_clk = sysclk_get_cpu_hz();
	opt.speed      = TWI_CLK;
	twi_master_init(BOARD_TWI, &opt);

	/* Configure TWI pins */
	#ifdef BOARD_TWI0
		gpio_configure_pin(TWI_DATA_GPIO, TWI_DATA_FLAG);
		gpio_configure_pin(TWI_CLK_GPIO, TWI_CLK_FLAGS);
	#endif

	/* Configure TWI interrupts */
	NVIC_DisableIRQ(BOARD_TWI_IRQn);
	NVIC_ClearPendingIRQ(BOARD_TWI_IRQn);
	NVIC_SetPriority(BOARD_TWI_IRQn, 0);
	NVIC_EnableIRQ(BOARD_TWI_IRQn);
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

// 	#if !defined(DEFAULT_MODE_COLORED)
// 		/* Samples only data with even index */
// 		p_pio->PIO_PCMR |= PIO_PCMR_HALFS;
// 		p_pio->PIO_PCMR &= ~((uint32_t)PIO_PCMR_FRSTS);
// 	#endif
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

	/* Configure Image sensor pins */
	/* Configure Camera sensor pins using new definitions */
	gpio_configure_pin(CAMERA_RST_GPIO, CAMERA_RST_FLAGS);
	/* Configure VSYNC, XCLK, and PCLK pins */
	gpio_configure_pin(CAMERA_HSYNC_GPIO, CAMERA_HSYNC_FLAGS);  // e.g., input or output flag as needed
	gpio_configure_pin(CAMERA_VSYNC_GPIO,  CAMERA_VSYNC_FLAGS);   // e.g., output for clock signal  // e.g., input for pixel clock

	/* Configure Camera Data Bus pins (PA24-PA31) */
	/* Assuming CAMERA_DATA_PINS is defined as 0xFF000000, which maps to PA24 to PA31 */
// 	for (uint32_t pin = PIO_PA24; pin <= PIO_PA31; pin++) {
// 		gpio_configure_pin(pin, CAMERA_DATA_FLAGS);  // Define CAMERA_DATA_FLAGS as needed
// 	}
	gpio_configure_pin(CAMERA_DATA_BUS_D0, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D1, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D2, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D3, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D4, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D5, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D6, CAMERA_DATA_FLAGS);
	gpio_configure_pin(CAMERA_DATA_BUS_D7, CAMERA_DATA_FLAGS);


	/* Init PCK1 to work at 24 Mhz initialize PLLB*/
	/* 96/4=24 Mhz */
	PMC->PMC_PCK[1] = (PMC_PCK_PRES_CLK_4 | PMC_PCK_CSS_PLLB_CLK);
	PMC->PMC_SCER = PMC_SCER_PCK1;
	while (!(PMC->PMC_SCSR & PMC_SCSR_PCK1)) {
	}

	// OV initialization
	while (ov_init(BOARD_TWI) == 1){}

	configure_camera();
	delay_ms(3000);

}

void configure_camera(void){
	ov_configure(BOARD_TWI, JPEG_INIT);
	ov_configure(BOARD_TWI, YUV422);
	ov_configure(BOARD_TWI, JPEG);
	ov_configure(BOARD_TWI, JPEG_320x240);
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
	while (!((PIOA->PIO_PCISR & PIO_PCIMR_RXBUFF) == PIO_PCIMR_RXBUFF)) {
	}

	/* Disable pio capture*/
	pio_capture_disable(PIOA);

	/* Reset vsync flag*/
	g_vsync_flag = false;
	
	if (find_image_len() == 1){
		return 1;
	}
	else{
		return 0;
	}

}


uint8_t find_image_len(void) {
	uint32_t i = 0;
	
	// Find SOI marker
	while (i < 100000) {
		if ((g_image_buffer[i] == 0xFF) && (g_image_buffer[i+1] == 0xD8)) {
			soi_pos = i;
			break;
		}
		i += 1;
	}

	// Find EOI marker
	while (i < 100000) {
		if ((g_image_buffer[i-1] == 0xFF) && (g_image_buffer[i] == 0xD9)) {
			eoi_pos = i;
			break;
		}
		i += 1;
	}

	if (i >= 100000){
		return 0;
	}
	
	g_image_length = eoi_pos - soi_pos + 1;
	
	return 1;
}
