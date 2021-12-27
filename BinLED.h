/*
 * BinLED.h
 *
 *  Created on: Sep 21, 2021
 *      Author: victor.wheeler
 */

#ifndef VICW_BINLED_H_
#define VICW_BINLED_H_

#include <arduino.h>

class BinLED {
public:
	BinLED(int pin);
	bool toggle();
	void set_State(bool state);
	bool debug;
private:
	bool state;
	int led;
};

#endif /* VICW_BINLED_H_ */
