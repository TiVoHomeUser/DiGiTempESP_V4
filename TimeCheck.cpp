/*
 * TimeCheck.cpp
 *
 *  Created on: Sep 21, 2021
 *      Author: victor.wheeler
 */

#include "TimeCheck.h"

/*
 * 			Management for defined timed intervals
 * 			A simple pseudo timer of timeIntervals
 *
 */
TimeCheck::TimeCheck(unsigned long timeInterval) {	// timeInterval is the milliseconds between events to count
	interval = timeInterval;
}



/*
 * 		Compares the built in millisecond counter with the last event
 * 		when the number of event intervals have elapsed resets the event timer then returns true
 */
bool TimeCheck::timeElapsed(int events) {			// events number of time intervals
	boolean retval = false;

	unsigned long currentmills = millis();
	if ((currentmills - lastmills) >= (interval * events)) {
		lastmills = currentmills;
		retval = true;
	}
	return retval;
}


/*
 *                                        Uptime()
 *           Compute up time return as a cstring days : hours : minutes : seconds
 *           Note:
 *            	max value for millis() is 4,294,967,295 or 49 days 17 hours 2 minutes and 47 Seconds.
 *             	After converting to seconds I am using an overflow counter to adjust.
 *             	should be good for a little over 136 years of uptime sorry does not do leapyear or leapsecond
 *
 */
//char* TimeCheck::uptime(){
//	static char tdstr[] = " 99:365:24:60:60 ";	// cstring to return
//	static unsigned int overflow = 0;			// Nnumber of 49.71 day overflows
//	static unsigned long lastmillis = 0;		// Used to check for overflow
//	unsigned long currentmillis = millis();
//	if(lastmillis > currentmillis){
//		overflow++;
//	}
//	lastmillis = currentmillis;
//
//	unsigned long total_seconds = (currentmillis / 1000);
//	total_seconds = total_seconds + ( overflow * 4294967 );	// adjust the seconds for millis() Roll over
//	unsigned long total_minutes = (total_seconds / 60);
//	unsigned int total_hours = (unsigned int) (total_minutes / 60);
//	unsigned int total_days = (total_hours / 24);
//
//	int years = total_days / 365;
//	int days = (total_days - (years * 365));
//	int hours = (total_hours - (total_days * 24));
//	int minutes = (total_minutes - (total_hours * 60));
//	int seconds = (total_seconds - (total_minutes * 60));
//
//	sprintf(tdstr, "%2d:%3d:%2d:%2d:%2d", years, days, hours, minutes, seconds);
//
//	return tdstr;
// }

