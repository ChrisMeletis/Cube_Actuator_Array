#pragma once
#include <Arduino.h>

/* Data definitions */

#define FRAME_TIMEOUT_MICROS 500
#define FRAME_TIMEOUT_MILLIS 5
#define BITS_PER_RGBW_FRAME 32

/* Pins definitions */

#define MOTOR_PIN PB3
#define DATA_PIN PB2

/* Other definitions*/
#define SERVO_PERIOD 20 //ms

/**/

struct System_Scratchpad{
	uint8_t   red_intencity = 0;
	uint8_t   green_intencity = 0;
	uint8_t   blue_intencity = 0;
    uint8_t   position = 0;
} __attribute__((packed));
System_Scratchpad systemStatus;	/** GLOBAL: Stores the last measured lux and humidity values. */
uint8_t current_command = 0;	/** GLOBAL: Variable to store the requested command*/

bool motor_control = false;


/*FUNCTION FORWARD DECLARATIONS*/

void moveMotor(uint8_t angle);
void moveMotorSmoothWind(uint8_t new_target, int step_size);
void decode_frame();
void outputAngle(uint8_t angle);