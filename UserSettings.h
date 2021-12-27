/*
 * UserSettings.h
 *
 *  Created on: Sep 21, 2021
 *      Author: victor.wheeler
 *
 */

#ifndef USERSETTINGS_H_
#define USERSETTINGS_H_

#define MY_HOSTNAME	"DigiTemp-AP"          // Name for this client


#ifndef APSSID
#define APSSID	"DigiTempESP"	// This WiFi Access Point name
#define APPSK	  "DigiTempPSK"	// This WiFi AP password

#endif

#define APIP	192,168,40,1	// "IP address when working as an Access Point

#define SCALE	true;		// 'C' true or 'F' false
#define	UPDATE_MINUTES 1	// How many minutes to try and cycle through all clients
#define WATCH_DOG  UPDATE_MINUTES * 5		// Minutes between data_page reads to trigger re-connect attempt

#include "DHT.h"
//#define DHTTYPE DHT22   	// there are multiple kinds of DHT sensors
//#define DHTTYPE DHT11
#define DHTTYPE DHTAUTO

#define DHTPIN 4     		// what digital pin the DHT22 is connected to
							// D4 = D2 on nodemcu D2 = D2 on D2 mini lite

#define BILED 13   		// Built in LED digital output

#endif /* USERSETTINGS_H_ */
