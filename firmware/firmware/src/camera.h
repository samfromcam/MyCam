/*
 * camera.h
 *
 * Created: 2/20/2025 1:14:07 PM
 *  Author: snb5582
 */

#ifndef CAMERA_H_
#define CAMERA_H_
#include <asf.h>
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

// Camera Pin Definitions
#define CAMERA_VSYNC_PIN       PIO_PA15
#define CAMERA_XCLK_PIN        PIO_PA17
#define CAMERA_PCLK_PIN        PIO_PA23
#define CAMERA_HSYNC_GPIO      PIO_PA16_IDX
#define CAMERA_VSYNC_GPIO      PIO_PA15_IDX
#define CAMERA_VSYNC_FLAGS         (PIO_PULLUP | PIO_IT_RISE_EDGE)
#define CAMERA_HSYNC_FLAGS     (PIO_PULLUP | PIO_IT_RISE_EDGE)
#define CAMERA_DATA_PINS       0xFF000000 //D0-D7 are on PA24-PA31
#define CAMERA_SCL_PIN   PIO_PA4
#define CAMERA_SDA_PIN   PIO_PA3
#define CAMERA_RST_FLAGS   (PIO_OUTPUT_1 | PIO_DEFAULT)
#define CAMERA_RST_GPIO        PIO_PA20_IDX
#define CAMERA_RST_MASK   PIO_PA20
#define CAMERA_RST_PIO         PIOA
#define CAMERA_RST_ID          ID_PIOA
#define CAMERA_RST_TYPE        PIO_OUTPUT_1
#define CAMERA_DATA_BUS_PIO   PIOA
#define CAMERA_DATA_FLAGS		(PIO_PULLUP | PIO_IT_RISE_EDGE)

// TWI parameters
#define BOARD_TWI              TWI0
#define BOARD_TWI_ID           ID_TWI0
#define BOARD_TWI_IRQn   TWI0_IRQn

// TWI Data pins definition
#define TWI_DATA_GPIO   PIO_PA3_IDX
#define TWI_DATA_FLAG   (PIO_PERIPH_A | PIO_PULLUP)

#define TWI_CLK_GPIO           PIO_PA4_IDX
#define TWI_CLK_FLAGS         (PIO_PERIPH_A | PIO_PULLUP)



/** Configure TWI0 pins (for camera communications). */
#define BOARD_TWI0
/** Configure PCK0 pins (for camera communications). */
#define BOARD_PCK1

#define PIN_PCK1 (PIO_PA17_IDX)
#define PIN_PCK1_FLAGS (PIO_PERIPH_B | PIO_DEFAULT)
// Function declarations
void vsync_handler(uint32_t ul_id, uint32_t ul_mask);
void init_vsync_interrupts(void);
void configure_twi(void);
void pio_capture_init(Pio *p_pio, uint32_t ul_id);
uint8_t pio_capture_to_buffer(Pio *p_pio, uint8_t *uc_buf, uint32_t ul_size);
void init_camera(void);
void configure_camera(void);
uint8_t start_capture(void);
uint8_t find_image_len(void);

// Variable declarations
volatile uint8_t g_vsync_flag;
uint8_t g_image_buffer[];
uint32_t g_image_length;


#endif /* CAMERA_H_ */