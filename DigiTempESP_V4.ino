#define copyrite " &#169; December 2021 VictorWheeler myapps@vicw.net use, modify and distribute without restrictions"
#define compiledate __DATE__

#include "DiGiTempESP_V4.h"

/**
 * Concept:
 * 	Create a Web page Server that displays temperatures read from sensors both local and remote Clients
 *
 * 		Server:		( First active client )
 * 			- Creates a private WiFi Access point
 * 			- Is itself a client.
 * 			- Publish a root html page containing all connected client's data readings
 *
 * 		Client:
 * 			- looks for Access Point if not found becomes AP
 * 			- Reads data from it's local sensor at frequent intervals.
 * 			- Publish a /Getdata WEB page that contains last read values.
 * 			- Contacts AP if not contacted within a reasonable period of time (Watchdog).
 * 			- Publish a root html page for display temperatures read from the local sensor.
 *
 *		TODO: collect from other clients becoming a MESH network that can pass data
 * 			    between clients.
 *
 *		Files:
 *			- DigiTempESP_V4	Global Setup and Outer Main loop
 *			- DigiTemp.h		#includes, global settings and functions
 *			- Settings.h		User configurable settings
 *			- Sensor.h,cpp		Containment for DHT sensor class
 *			- DHT.h,cpp			Modified files from the Adafruit Industries DHT library
 *			- VicWUtils.h		Common tested Functions
 *
 *
 */

int biled = BILED;
void setup()
{
	delay(200);	// Some chips need to stabilize
	Serial.begin(74880); // 74880 seems to be the boot default for the chips I use
	Serial.println(copyrite); Serial.println(compiledate);

	/* Visual confirmation that all is well Toggle Function for the Built in LED */
	BiLED = new BinLED(BILED);			// Built in LED Toggle Function

	/* Setup Temperature sensor */
	sensor  = new Sensor(DHTPIN, DHTTYPE);
	/* Connect to the AP or become the AP Client should never see a newClient */
	is_AP = newClient = connectAP();
	/* Add Network connections (Html links for client) */
	  server.on("/info", info_Page);
	  server.on("/", root_Page);
	  server.on("/getData", data_Page);  // Server read data and Keep Alive
	  server.on("/reboot", rebootPage);
	  server.on("/stations", stationPage);
	  server.on("/toggle", togglePage);
	  //server.onNotFound(notFoundPage); // No need ESP8266WebServer handles this

	/* Start the Network and Announce we are here */
	server.begin();
	setupmDNS();

	/* Start the watch-dog and timers */
	Minutes = new TimeCheck(TimeCheck::MINUTES);
	UpdateAPSeconds = new TimeCheck(TimeCheck::SECONDS);
	uptime = new Uptime();
 	still_here = -1;					// Let WD timeout announce we are here
	clients[0].myHostName = hostName;
	if(is_AP) clients[0].myHostName += ".";
 	yield();
 	clients[0].Th_t = sensor->read();	// First read else takes UPDATE_MINUTES b4 data is updated
 	clients[0].isAlive = true;
 	clients[0].ipAddress = WiFi.localIP();
}


unsigned int client_num = 0;
void loop() {
	/* Listen for and process connections */
	server.handleClient();
	MDNS.update();

	/* Read the local sensor at intervals */
	if (Minutes->timeElapsed(UPDATE_MINUTES)) {		// Do every x minute(s)
		BiLED->toggle();
		clients[0].Th_t = sensor->read();
		if (DEBUG) {
			Serial.println(info());
		}
		if (!is_AP) {
			if (still_here-- <= 0) {	// Clients reconnect if /GetData is not accessed
				Serial.println(F("*** Watch Dog timeout ***"));
				if (WiFi.reconnect()) {	// Broken returns true even for no connections
					if(WiFi.isConnected()){
						Serial.println(F("isConnected()"));
						still_here = watch_Dog;
					} else {
						Serial.println(F("is *NOT* Connected()"));
					}
				}
				if( still_here < -2 ) {	// Become the Access Point
					Serial.println(F("*** No AP lets become one ***"));
					is_AP = newClient = connectAP();
				}
			}	// still_here <= 0
		}	// is not AP
	}	// Update Minutes

	/*							Access Point process clients								*/
	if (is_AP) {
		if (newClient) {
			Serial.println(F("***** NEW CLIENT *****"));
			Load_Client_List(true);
			newClient = false;
		}

		int connections = WiFi.softAPgetStationNum();
		if (connections <= 0)	connections = 1;			//Prevent divide by 0
		/* At n/clients interval read a single remote sensor */
		if (UpdateAPSeconds->timeElapsed(((UPDATE_MINUTES) * 60) / connections)) {
			if (client_num > WiFi.softAPgetStationNum()) client_num = 0;
			read_Client(client_num++);
		}
	}	// AP


	/* Process any input from the Serial monitor */
	if (Serial.available() > 0) {
		do_serial(Serial.read());
	} // Serial.avaiable()

}
