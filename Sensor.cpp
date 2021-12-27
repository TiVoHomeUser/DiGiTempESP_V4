/*
 * Sensor.cpp
 *
 *  Created on: Oct 5, 2021
 *      Author: Victor Wheeler myapps@vicw.net
 *
 *	Last update:
 *      20211209	Auto select
 *      	Detects and connects either DHT11 or DHT22
 *
 */

#include "Sensor.h"

Sensor::Sensor(uint8_t pin, uint8_t type) {
	m_type = type;
	dht = NULL;
	dhtValid = false;

	/*
	 *  I found DHT22 returns NAN if setup as DHT11
	 * Experimental: DHTAUTO forces search for DHT type.
	 * Later with a modified the DHT Library that now exposes the read fail using that to call auto-set from read().
	 *
	 */
	if(DHTAUTO == type){
		m_type = DHT22;
		dht = new DHT(pin,m_type);
		dht->begin();
	} else {
		dht = new DHT(pin,type);
		dht->begin();
	}
}

bool Sensor::auto_set(){
	if(!dhtValid){
//		Serial.println(F("*************************** DH22 *********************************")); Serial.flush(); yield();
		m_type = DHT22;
		dht->set_sensor_typ(m_type);
		dht->readHumidity(true);
		dhtValid = dht->is_Valid(); //!dht->sensorerror;
//		Serial.print(F("is_Valid() = *")); if(dht->is_Valid()) Serial.println("TRUE*"); else  Serial.println("FALSE*"); Serial.flush(); yield();
	}

	if(!dhtValid){
//		Serial.println(F("*************************** DH11 *********************************")); Serial.flush(); yield();
		m_type = DHT11;
		dht->set_sensor_typ(m_type);
		dht->readHumidity(true);
		dhtValid = dht->is_Valid(); //!dht->sensorerror;
//		Serial.print(F("is_Valid() = *")); if(dht->is_Valid()) Serial.println("TRUE*"); else  Serial.println("FALSE*"); Serial.flush(); yield();
	}
	if(!dhtValid){
		m_type = DHTERR;
	}
	return dhtValid;
}

/*
 * 		While there is room returning the Sensor type as a string for display
 *
 */
const char* Sensor::sensor_type() {
	static String retval;
	retval.clear();
	switch(m_type) {
	case DHT11:
		retval = "DHT11";
		break;
	case DHT12:
		retval = "DHT12";
		break;
	case DHT21:
		retval = "DHT21";
		break;
	case DHT22:
		retval = "DHT22";
		break;
//	case AM2301:				// same as DHT21
//		retval = "AM2301";
//		break;
	case DHTAUTO:
		retval = "DHTAUTO";
		break;
	case DHTERR:
		retval = "DHTERR";
		break;
	default:
		retval = "UnKwn";
	}
	return retval.c_str();
}

/**
 *
 * 		Working default is celsius using DHT's built in conversion for fahrenheit
 *
 */
float Sensor::convertCtoF(float t) {
	return dht->convertCtoF(t);
}

/**
 *
 * 			Read DHT sensor return a Th_temp structure with refreshed values
 *
 * 		Reading temperature or humidity takes about 250 milliseconds!
 * 		Sensor readings may also be up to 2 seconds 'old'
 *
 */
Th_temp Sensor::read() {
//Serial.println(F("----------- Sensor Read() ---------------")); Serial.flush(); yield();
	if(!dhtValid){
		dhtValid = auto_set();
	}
//Serial.print(F("1: is_Valid = **")); if(dht->is_Valid()) Serial.println(F(" TRUE**")); else Serial.println(F(" FALSE**")); Serial.flush(); yield();
	Th_t.h = dht->readHumidity();
//Serial.print(F("2: is_Valid = **")); if(dht->is_Valid()) Serial.println(F(" TRUE**")); else Serial.println(F(" FALSE**")); Serial.flush(); yield();

	// Read temperature as Celsius (the default)
	Th_t.c = dht->readTemperature();
//Serial.print(F("3: is_Valid = **")); if(dht->is_Valid()) Serial.println(F(" TRUE**")); else Serial.println(F(" FALSE**")); Serial.flush(); yield();

	// Track High / Low
	if (Th_t.c > Th_t.tmax) {
		Th_t.tmax = Th_t.c;
	}
	if (Th_t.c < Th_t.tmin) {
		Th_t.tmin = Th_t.c;
	}

	return Th_t;
}
