/*
 * BinLED.cpp
 *
 *  Created on: Sep 21, 2021
 *      Author: victor.wheeler
 */

#include "BinLED.h"

BinLED::BinLED(int pin) {
	led = pin;
	state = true;
	debug = true;
	Serial.print(F("LED_BUILTIN "));
	Serial.print(LED_BUILTIN);
	Serial.print(F("\tOUTPUT "));
	Serial.println(OUTPUT);
	pinMode(LED_BUILTIN, OUTPUT); // Initialize the LED_BUILTIN pin as an output
	pinMode(led, OUTPUT);
	digitalWrite(led, 0);
}

bool BinLED::toggle() {
	set_State(!state);
	return state;
}

void BinLED::set_State(bool state) {
	this->state = state;
	if (state) {
		digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
		if (debug)
			Serial.print(F("ON "));
	} else {
		digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
		if (debug)
			Serial.print(F("OFF "));
	}
}

