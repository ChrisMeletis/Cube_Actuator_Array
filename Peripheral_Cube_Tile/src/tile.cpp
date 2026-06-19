#include <Arduino.h>
#include "declarations.h"

/* -------------------- VARIABLE DECLARATIONS -------------------- */
volatile bool frame[BITS_PER_RGBW_FRAME];
volatile uint16_t frame_index = 0;

void setup() {                
	pinMode(PB0, OUTPUT); 
	pinMode(LED_BUILTIN, OUTPUT);	//PB1
	pinMode(DATA_PIN, INPUT);		//PB2
	pinMode(MOTOR_PIN, OUTPUT);		//PB3
	pinMode(PB4, OUTPUT);
	digitalWrite(PB0,LOW);
	digitalWrite(LED_BUILTIN,LOW);
	digitalWrite(MOTOR_PIN,LOW);
	digitalWrite(PB4,LOW);
}

/*Loop for motor testing*/
// void loop(){
// 	static unsigned long lastUpdate = 0;
// 	static uint8_t angle = 0;
// 	static int direction = 1; // 1 for increasing, -1 for decreasing
// 	moveMotor(angle);
// 	if(lastUpdate<5000){lastUpdate++;}
// 	else{
// 		if(angle == 0){direction = false;}
// 		if(angle == 180){direction = true;}
// 		if(!direction){	angle++; }
// 		else{ angle--; }
// 		lastUpdate = 0;
// 	}
// }


bool discard_frame = false;
void loop(){
	static unsigned long bit_end = 0;
	static unsigned long bit_start = 0;
	static unsigned long move_start = 0;
	static uint8_t last_angle = 0;
	int16_t step = 0;
	
	
	if(PINB & (1 << DATA_PIN)){
		motor_control = false;
		if(frame_index > BITS_PER_RGBW_FRAME - 1){ PORTB = (1 << PB4); }// output rest of frame to next module			
		else{
			bit_start = millis();
			unsigned long pulse_width = micros();
			while(PINB & (1 << DATA_PIN) && (millis() - bit_start < FRAME_TIMEOUT_MILLIS)){ asm volatile("nop"); }	//Wait for falling edge with timeout
			if(micros() - pulse_width < 60){ frame[frame_index] = 0; }		// bit "0"
			else if(micros() - pulse_width < 100){ frame[frame_index] = 1; }	// bit "1"
			else{ discard_frame = true; }
			frame_index++;
		}
		bit_end = millis();
	}
	else if(!motor_control){
		if(frame_index > BITS_PER_RGBW_FRAME - 1 && (millis() - bit_end < FRAME_TIMEOUT_MILLIS)){ PORTB = (0 << PB4); }// output rest of frame to next module
		else{
			while(!(PINB & (1 << DATA_PIN)) && (millis() - bit_end < FRAME_TIMEOUT_MILLIS)){ asm volatile("nop"); }	//Wait for rising edge with timeout
			if(millis() - bit_end >= FRAME_TIMEOUT_MILLIS){	// return control to motor
				PORTB |= (1 << LED_BUILTIN);
				if(frame_index < BITS_PER_RGBW_FRAME - 1){ discard_frame = true; }
				decode_frame();
				step = abs((systemStatus.position - last_angle) / 45.0);
				motor_control = true;
				move_start = millis();		
			}
		}
	}
	
	/** Interpolate from previous position to new position
	 * The motor is free to move for 300ms, this means that it will have 15 pulses of 20ms
	 * We have to find the apropriate angle step to smoothly move from previous to new position.
	*/
	if(motor_control && (millis() - move_start < 900)){
		// moveMotor(systemStatus.position);
		moveMotorSmoothWind(systemStatus.position, step);
	}
	if(!motor_control){ last_angle = systemStatus.position; }
}



/** Moves the motor to the requested position
 * @param angle - the requested absolute angle in degrees 0 - 180
*/
void moveMotor(uint8_t angle){
	static unsigned long last_servo_pulse = millis();
	// Check valid angles
	if(angle > 180){ angle = 180; }

	// Generate servo pulse every 20ms
	if (millis() - last_servo_pulse >= SERVO_PERIOD) {
		last_servo_pulse = millis();

		int pw = map(angle, 0, 180, 300, 2300);
		PORTB |= (1 << MOTOR_PIN);
		delayMicroseconds(pw);
		PORTB &= ~(1 << MOTOR_PIN);
	}
}

/** Move motor to specified angle using "stepping"
 * The function will take one step closer to the requested angle each time its called
 * @param angle - the requested absolute angle in degrees 0 - 180
 * @param step_size - the angle step in degrees
*/
void moveMotorSmoothWind(uint8_t new_target, int step_size){
    static int currenta_ngle = 0;   // remembers last position across calls
	static unsigned long last_servo_pulse = millis();

	// Generate servo pulse every 20ms
	if (millis() - last_servo_pulse >= SERVO_PERIOD) {
		last_servo_pulse = millis();
    // If we're already at target → nothing to do
    if (currenta_ngle == new_target) {
		int pw = map(currenta_ngle, 0, 180, 300, 2300);
		PORTB |= (1 << MOTOR_PIN);
		delayMicroseconds(pw);
		PORTB &= ~(1 << MOTOR_PIN);
        return;
    }

	// Check arguments validity
	new_target = constrain(new_target, 0, 180);
	if(step_size == 0){ step_size = 1; }

    // Decide direction
    if (new_target > currenta_ngle) {
        currenta_ngle += step_size;
        if (currenta_ngle > new_target) currenta_ngle = new_target; // clamp
    } else {
        currenta_ngle -= step_size;
        if (currenta_ngle < new_target) currenta_ngle = new_target; // clamp
    }
	
	int pw = map(currenta_ngle, 0, 180, 300, 2300);
	PORTB |= (1 << MOTOR_PIN);
	delayMicroseconds(pw);
	PORTB &= ~(1 << MOTOR_PIN);
	}
}

/** Decodes the received frame
 * Updates the system values accordingly
*/
void decode_frame() {
	frame_index = 0;
	if(discard_frame){
		discard_frame = false;
		return;
	}
    for (int colour = 0; colour < 4; colour++) {
        uint8_t val = 0;
        for (int colour_bit = 0; colour_bit < 8; colour_bit++) {
            val |= (frame[colour * 8 + colour_bit] << (7 - colour_bit));
        }
        if (colour == 0){ systemStatus.green_intencity = val;}
        if (colour == 1){ systemStatus.red_intencity = val;}
        if (colour == 2){ systemStatus.blue_intencity = val;}
        if (colour == 3){ systemStatus.position = val;}
    }
}

void outputAngle(uint8_t angle){
	for(int i = 7; i >= 0; i--){
		if(angle & (1 << i)){
			PORTB |= (1 << PB4);
			delayMicroseconds(20);
			PORTB &= ~(1 << PB4);
			delayMicroseconds(20);
		}
		else{
			PORTB &= ~(1 << PB4);
			delayMicroseconds(40);
		}
	}
}