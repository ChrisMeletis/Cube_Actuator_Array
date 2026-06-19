#include <Arduino.h>
#include "Adafruit_NeoPixel_hacked.h"

#define LED_COUNT 6
#define LED_PIN 2

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRBW + NEO_KHZ8);
#define DATA_DELAY 910
uint8_t extrusion = 0;

#define ANIM_MOTOR_COUNT 6
uint8_t clear[ANIM_MOTOR_COUNT][2] = {
  {   0,   0,},
  {   0,   0,},
  {   0,   0,},
  {   0,   0,},
  {   0,   0,},
  {   0,   0,},
};

uint8_t slow_dual_wave[ANIM_MOTOR_COUNT][14] = {
  {   0,   0,   0,   0,  45,  90, 135, 150, 135,  90,  45,   0,   0,   0,},
  {   0,   0,  45,  90, 135, 150, 150, 135,  90,  45,   0,   0,   0,   0,},
  {   0,  45,  90, 135, 150, 150, 150, 135,  90,  45,   0,   0,   0,   0,},
  {   0,  45,  90, 135, 150, 150, 150, 135,  90,  45,   0,   0,   0,   0,},
  {   0,   0,  45,  90, 135, 150, 150, 135,  90,  45,   0,   0,   0,   0,},
  {   0,   0,   0,   0,  45,  90, 135, 150, 135,  90,  45,   0,   0,   0,},
};

uint8_t medium_wave[ANIM_MOTOR_COUNT][24] = {
  {   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  90, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,  90,   0,},
  {   0,   0,   0,   0,   0,   0,   0,   0,   0,  90, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,  90,   0,   0,   0,},
  {   0,   0,   0,   0,   0,   0,   0,  90, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,  90,   0,   0,   0,   0,   0,},
  {   0,   0,   0,   0,   0,  90, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,  90,   0,   0,   0,   0,   0,   0,   0,},
  {   0,   0,   0,  90, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,  90,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
  {   0,  90, 180, 180, 180, 180, 180, 180, 180, 180, 180, 180,  90,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,},
};

template<size_t N>
void playSequence(const uint8_t (&animation)[ANIM_MOTOR_COUNT][N]) {
	for (uint8_t animation_index = 0; animation_index < N; animation_index++) {
		strip.setPixelColor(0, strip.Color(0, 0, 0, animation[0][animation_index]));
		strip.setPixelColor(1, strip.Color(0, 0, 0, animation[1][animation_index]));
		strip.setPixelColor(2, strip.Color(0, 0, 0, animation[2][animation_index]));
		strip.setPixelColor(3, strip.Color(0, 0, 0, animation[3][animation_index]));
		strip.setPixelColor(4, strip.Color(0, 0, 0, animation[4][animation_index]));
		strip.setPixelColor(5, strip.Color(0, 0, 0, animation[5][animation_index]));
		strip.show();
		delay(DATA_DELAY);
	}
}

void setup() {
	strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
	strip.show();            // Turn OFF all pixels ASAP
	strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)
	Serial.begin(9600);
}


void loop() {
	playSequence(clear);
	playSequence(medium_wave);
	playSequence(clear);
	playSequence(slow_dual_wave);
}

