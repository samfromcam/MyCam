/*
 * camera.h
 *
 * Created: 2/20/2025 1:14:07 PM
 *  Author: snb5582
 */ 

#ifndef CAMERA_H_
#define CAMERA_H_
#include "camera_helper/ov2640.h"
#include <asf.h>

// Camera Pin Definitions
#define CAMERA_VSYNC_PIN       PIO_PA15
#define CAMERA_XCLK_PIN        PIO_PA17
#define CAMERA_PCLK_PIN        PIO_PA23
#define CAMERA_DATA_PINS       0xFF000000 //D0-D7 are on PA24-PA31
#define CAMERA_SCL_PIN		   PIO_PA4
#define CAMERA_SDA_PIN		   PIO_PA3

// TWI parameters
#define BOARD_TWI              TWI0
#define BOARD_TWI_ID           ID_TWI0
#define BOARD_TWI_ADDR         OV_I2C_SENSOR_ADDRESS 
#define BOARD_TWI_IRQn		   TWI0_IRQn

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