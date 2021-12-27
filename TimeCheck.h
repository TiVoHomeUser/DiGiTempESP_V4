/*
 * TimeCheck.h
 *
 *  Created on: Sep 21, 2021
 *      Author: victor.wheeler
 */

#ifndef VICW_TIMECHECK_H_
#define VICW_TIMECHECK_H_
#include <arduino.h>


/*
 * 								Rough time check
 *
 * 		Test for elapsed event period
 *
 * 		 return
 * 		 	true
 * 		 		nn periods have passed from last event
 *
 * 		 	false
 * 		 		nn periods have not passed
 *
 */
class TimeCheck {

private:
	unsigned long interval = 0;
	unsigned long lastmills = 0;

public:
	const static unsigned long SECONDS = 1000;
	const static unsigned long MINUTES = SECONDS * 60;
	const static unsigned long HOURS = MINUTES * 60;
	const static unsigned long DAYS = HOURS * 24;

	TimeCheck(unsigned long timeInterval);	// timeInterval is the milliseconds between events to count
	bool timeElapsed(int);					// events number of time intervals

	/*
	 *  Reporting the uptime in a formatted string
	 *  Call:
	 *  	pointer to a block of 29 or more chars "366 Days 23 Hours 59 Minutes0"
	 *  returns:
	 *  	pointer to the formatted string
	 *
	 */
//	char* uptime();

};

#endif /* VICW_TIMECHECK_H_ */
