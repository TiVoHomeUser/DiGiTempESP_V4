/*
 * Uptime.h
 *
 *  Created on: Sep 28, 2021
 *      Author: victor.wheeler
 */

#ifndef VICW_UPTIME_H_
#define VICW_UPTIME_H_
#include <arduino.h>

/**
 *                                        Uptime
 *           Compute up time return as a cstring days : hours : minutes : seconds
 *           Note:
 *            	max value for millis() is 4,294,967,295 or 49 days 17 hours 2 minutes and 47 Seconds.
 *             	After converting to seconds I am using an overflow counter to adjust.
 *             	should be good for a little over 136 years of uptime sorry does not do leapyear or leapsecond
 *
 */
class Uptime {
public:
	Uptime();
	char* toString();
private:
	 char retstr[16]; 				// " 99:365:24:60:60 "  cString to return
	 unsigned int overflow;		// Number of 49.71 day overflows
	 unsigned long lastmillis;	// Used to check for overflow
};

#endif /* VICW_UPTIME_H_ */
