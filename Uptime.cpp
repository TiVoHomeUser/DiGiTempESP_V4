/*
 * Uptime.cpp
 *
 *  Created on: Sep 28, 2021
 *      Author: victor.wheeler
 */

#include "Uptime.h"

Uptime::Uptime() {
	retstr[0] = '\0'; //" 99:365:24:60:60 ";	// cstring to return
	overflow = 0;			// Nnumber of 49.71 day overflows
	lastmillis = 0;		// Used to check for overflow
}

char* Uptime::toString(){
	//static char tdstr[] = " 99:365:24:60:60 "; //  cString to return
	unsigned long currentmillis = millis();
	if(lastmillis > currentmillis){
		overflow++;
	}
	lastmillis = currentmillis;

	unsigned long total_seconds = (currentmillis / 1000);
	total_seconds = total_seconds + ( overflow * 4294967 );	// adjust the seconds for millis() Roll over
	unsigned long total_minutes = (total_seconds / 60);
	unsigned int total_hours = (unsigned int) (total_minutes / 60);
	unsigned int total_days = (total_hours / 24);

	int years = total_days / 365;
	int days = (total_days - (years * 365));
	int hours = (total_hours - (total_days * 24));
	int minutes = (total_minutes - (total_hours * 60));
	int seconds = (total_seconds - (total_minutes * 60));

	sprintf(retstr, "%2d:%3d:%2d:%2d:%2d", years, days, hours, minutes, seconds);

	return retstr;
}

