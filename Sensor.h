/**
 * Sensor.h
 *
 *  Created on: Oct 5, 2021
 *      Author: Victor Wheeler myapps@vicw.net
 *
 *      My Wrapper for the Adafruit DHT sensor library
 *      I use the DHT sensors in several projects all with similar functions. This was originally a simple class for
 *      containing the duplicate code defining, accessing and storing the DHT object values.
 *
 *
 *  12/2021 DHTAUTO
 *      Frequently I was re-flashing the ESP because I had the wrong DHT sensor defined. I have 2 types of sensors
 *      DHT11 and DHT22 I found that when configured for DHT22 with the device connected to the DHT11 read() would
 *      return NAN allowing the use of isnan() to determine the correct sensor at run time. Tunneling into the DHT
 *      code I discovered a private bool _lastresult that would indicate a read failure either way. I added accessor
 *      is_Valid() to the DHT class to expose this value. While in the DHT code I added a function set_sensor_typ()
 *      to redefine the sensor type because deleting and recreating the DHT object without a destructor to stop the
 *      interrupts had undesirable results.
 *
 */

#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "DHT.h"

// Experimental: DHTAUTO forces search for DHT11 or DHT22
#define DHTAUTO 100	/** < DHT Type 11 or 22 */
#define DHTERR	255	/** <  DHT Invalid or not connected */

/**
 *
 *	Structure for passing DHT sensor readings
 *	Using the DHT sensor library default of Celsius
 *
 */
#define INVALID NAN
typedef struct Th_temp {
	float h = INVALID;			// Humidity in percent
	float c = INVALID;			// Current reading Celsius
	float tmax = -999.99;		// Maximum readings over lifetime
	float tmin = 999.99;		// Minimum readings over lifetime
} Th_temp;

/**
 *
 *					Containment for the DHT object
 *
 */
class Sensor {
public:
	Sensor(uint8_t pin, uint8_t type);	// uint8_t count = 6);
	Th_temp read();						// Read the DHT sensor return Th_temp structure filled with new values
	float convertCtoF(float);			// celsius is the default convert for fahrenheit
	const char* sensor_type();			// Display selected sensor type as a c-string
	bool auto_set();					// Auto select sensor called every read until a valid sensor is found
private:
	bool dhtValid;
	uint8_t m_type;
	Th_temp Th_t;
	DHT *dht;
};

#endif /* _SENSOR_H_ */
