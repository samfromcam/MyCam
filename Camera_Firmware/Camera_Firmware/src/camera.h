/*
 * camera.h
 *
 * Created: 2/20/2025 1:14:07 PM
 *  Author: snb5582
 */ 


#ifndef CAMERA_H_
#define CAMERA_H_
#include "camera_lib/ov2640.h"

// Camera Pin Definitions
#define D0      PIO_PA24
#define D1      PIO_PA25
#define D2      PIO_PA26
#define D3      PIO_PA27
#define D4      PIO_PA28
#define D5      PIO_PA29
#define D6      PIO_PA30
#define D7      PIO_PA31
#define HREF    PIO_PA16
#define RST     PIO_PA20
#define PCLK    PIO_PA23
#define XCLK    PIO_PA17
#define SCL     PIO_PA4   
#define VSYNC   PIO_PA15
#define SDA     PIO_PA3

// Function Prototypes
void configure_twi(void);
void init_vsync_interrupts(void);


void pio_capture_init(Pio *p_pio, uint32_t ul_id)

// VSYNC Interrupt Handler
void VSYNC_Handler(uint32_t ul_id, uint32_t ul_mask);
void init_camera(void);
void configure_camera(void);
uint8_t find_image_len(void)

// Global Variables
extern volatile uint8_t frame_buffers[]; 
extern volatile uint8_t vsync_flag = 0;      
extern volatile uint8_t len_success;

volatile uint8_t image_started;
volatile uint8_t image_size;
volatile uint8_t image_ended;
volatile uint8_t start_pos;
volatile uint8_t end_pos;


#endif /* CAMERA_H_ */