#ifndef _DiGiTempESP_V4_H_
#define _DiGiTempESP_V4_H_  1

//#include <ESP8266WiFi.h>
//#include <WiFiClient.h>
#include <ESP8266WebServer.h>
ESP8266WebServer server(80);
//WiFiServer server(80);
#include <ESP8266mDNS.h>        // mDNS Client or Bonjour Client library for the esp8266.
								// For lookup using hostname.local instead of IP address

#include "Arduino.h"
//#include <VicWUtils.h>
#include "BinLED.h"
#include "TimeCheck.h"
#include "UpTime.h"

#include "UserSettings.h"
#include "Sensor.h"


	const static bool DEBUG=true;

	BinLED* BiLED;						// Function to toggle built in led
	static const String SUID = String(ESP.getChipId());

	const char* ssid     = APSSID;
	const char* password = APPSK;
	String hostName;

	const int max_connection = 8;
	const int channel = 4;

	boolean is_AP;			// True we are an Access Point false just a client
	bool newClient = true;

	/*
	 *
	 * 			Storage for client data
	 *
	 */
	typedef struct _clist{
	  IPAddress ipAddress;
	  boolean   isAlive;				//refreshed with valid call to getData()
	  String    myHostName;
	  Th_temp   Th_t;					// Structure to store Sensor data
	  unsigned long last_check;			// Last time sensor was accessed
	} clist;

	clist clients[max_connection + 2];	// include space for local sensor

	void Load_Client_List(boolean reload = false);
	boolean checkStation(clist* addss);



	Sensor* sensor;
	static boolean scale = SCALE;		// Celsius or Fahrenheit

	TimeCheck* Minutes;					// function that tests elapsed time cycles
	TimeCheck* UpdateAPSeconds;			// Interval to update all clients when running as Access Point
	Uptime* uptime;						// Elapsed time from last start (boot)
	const int watch_Dog = WATCH_DOG;	// Time in out minutes before forcing reconnect to server
	int still_here;						// Used to keep leash on Watch dog

	/**
	 * 									Some info about the ESP and Sensor useful for debugging
	 * 									may be removed to free additional memory
	 */
	char* info(){
		static char retVal[100];
		retVal[0] = '\0';
		sprintf(retVal, "Uptime = %s\nStillHere = %d\n", uptime->toString(), still_here) ;
		if(scale) {
			sprintf(retVal+strlen(retVal), "Th_t = %4.2fF %4.2fF/%4.2fF ",
					sensor->convertCtoF(clients[0].Th_t.c),
					sensor->convertCtoF(clients[0].Th_t.tmax),
					sensor->convertCtoF(clients[0].Th_t.tmin));
		}else{
			sprintf(retVal+strlen(retVal), "Th_t = %3.1fC %3.1fC/%3.1fC ",
					clients[0].Th_t.c,
					clients[0].Th_t.tmax,
					clients[0].Th_t.tmin);
		}
		strcat(retVal, sensor->sensor_type());
		Serial.print(F("info()\nretVal.length = ")); Serial.println(strlen(retVal));
		return retVal;
	}
											// allow access from WEB page
	void info_Page(){
		server.send(200, "text/html" , info());
	}


	/*
	 *
	 * 					Function to create a "Button" for display the Sensor on the root page.
	 * 					Press forces a refresh of the Sensors data
	 * 					All Sensors are displayed on the AP root page with link to display Data Press on the AP Sensor
	 * 						will force enabling all logged IP's for refresh
	 *
	 */
	String button(IPAddress IP, Th_temp Th_t, String hostName, int staNo){
			  char tempFs[7];
			  char humis[5];
			  static String ptr;
			  ptr.clear();
			  	  ptr += "<p>";
			      ptr += "<a href=http://";
			      ptr += IP.toString();
			      ptr += "/getData";
			      ptr += ">";
			      if(hostName.length() < 1) {
			    	  ptr += IP.toString();
			      } else {
			    	  ptr += hostName;
			      }
			      ptr += "</a>";
			      ptr += "<br>";
			      ptr += "  {{HighLow}}  ";
			      if(Th_t.c == INVALID){
			    	  sprintf(tempFs, "--.--");
			    	  sprintf(humis, "--.-");
			      } else {
			        if(scale){
			           	sprintf(tempFs, "%02.1f ", Th_t.c);
			        } else {
			       		sprintf(tempFs, "%02.2f", sensor->convertCtoF(Th_t.c));
			        }

			       	sprintf(humis, "%02.1f", Th_t.h);
			      }
			    	ptr += "<a class=\"button button-off\" href=\"/stations";
			        ptr += "?id=";        ptr += String(staNo);        // ID for station refresh
			        ptr += "\"> ";
			        ptr += tempFs;
			        if(scale){
			        	ptr += "C ";
			        } else {
			        	ptr += "F ";
			        }
			        ptr += humis;
			        ptr += "%</a>";
			        char highlow[40];
			        if(Th_t.tmax <  -990){
			        	sprintf(highlow, "---- ----");
			        } else {
			        if(scale){
			        	sprintf(highlow, "High: %02.2fC\t\tLow: %02.2fC", Th_t.tmax, Th_t.tmin);
			        } else {
			        	sprintf(highlow, "High: %02.2fF\t\tLow: %02.2fF", sensor->convertCtoF(Th_t.tmax), sensor->convertCtoF(Th_t.tmin));
			        }
			        }
			        ptr.replace("{{HighLow}}", String(highlow));

			  return ptr;
		}


	/*
	 *													Root Page
	 *
	 */
	void root_Page() {
	if (DEBUG)
		Serial.println(F("RootPage()"));
	char connections[3];
	sprintf(connections, "%2d", WiFi.softAPgetStationNum());
	server.sendContent("<!DOCTYPE html> <html>\n"
			"<head>\n"
			"<link rel=\"shortcut icon\" href=\"#\" />" // Fix for "/favicon.ico not found" error
			"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n"
			"<title>DigiTemp AP Control</title>\n"
			"<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n"
			"body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;} h6 {color: #DFF2FB;}\n"
			".button {display: block;width: 80px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n"
			".button-on {background-color: #1abc9c;}\n"
			".button-on:active {background-color: #16a085;}\n"
			".button-off {background-color: #34495e;}\n"
			".button-off:active {background-color: #2c3e50;}\n"
			"p {font-size: 14px;color: #888;margin-bottom: 10px;}\n"
			"</style>\n"
			"<script type=\"text/javascript\">setTimeout(\"location.reload()\", 45000);</script>\n"
			"</head>\n");
	//
	String ptr = "<body>\n";
	ptr += "<h1>ESP8266 DigiTemp Server <a href=toggle> {{S}} </a> </h1>\n";
	ptr += "<h3>Using Access Point(AP) Mode ";
	ptr += connections;
	ptr += " clients</h3>\n";

	// Loop starts at one clients[0] is special case for local DHT
	for (int ii = 0; ii <= max_connection; ii++) {
//		for (int ii = 0; ii <= WiFi.softAPgetStationNum(); ii++) { //max_connection
		if (clients[ii].isAlive) {
			if (DEBUG) {
				Serial.print("AP Sta Num = ");
				Serial.println(WiFi.softAPgetStationNum());
			}
			ptr += button(clients[ii].ipAddress,clients[ii].Th_t, clients[ii].myHostName, ii);
		}
		if (scale) {
			ptr.replace("{{S}}", "C");
		} else {
			ptr.replace("{{S}}", "F");
		}
	}
	ptr += "<h6>";
	ptr += copyrite;
	ptr += " ";
	ptr += compiledate;
	ptr += "</h6>";
	ptr += "</body>\n";
	ptr += "</html>\n";
	server.sendContent(ptr);
	server.client().flush();
}

	/*
	 *
	 *
	 *
	 */
	void data_Page(){  // Server read data and Keep Alive
		if(DEBUG){ Serial.print(F("Hello from getData() StillHere = ")); Serial.println(still_here);}
	  still_here = watch_Dog;	// Contacted reset counter
	  String  content;
	  content  = "<!DOCTYPE html>\n";
	  content += "<html>\n";
	  content += "<head>\n";
	  content += "<meta charset=\"UTF 8\">\n";
	  content += "<title>DigiTemp data cookie {{myHostName}} </title>\n";
	  content += "</head>\n";
	  content += "<body>\n";
	  content += "<p>";
	  content += "Chip ID: {{UID}}<br>\n";
	  content += "Sensor: &nbsp; {{DHTTYP}}<br>\n";
	  content += "IP: &nbsp; &nbsp; &nbsp; &nbsp; &nbsp; {{MyIP}}<br>\n";
	  content += "Uptime: &nbsp; ";
	  content += uptime->toString();
	  content += "<br>\n";

	  content += "<br>\n";
	  content += "Humidity: {{Humid}} <br>\n";

	  content += "<br>\n";
	  content += "Temperature: {{TempC}} <br>\n";
	  content += " &nbsp; &nbsp; Max: {{TempMaxC}} <br>\n";
	  content += " &nbsp; &nbsp; Min: {{TempMinC}} <br>\n";

	  content += "<br>\n";
	  content += "Temperature: {{TempF}} <br>\n";
	  content += " &nbsp; &nbsp; Max: {{TempMaxF}} <br>\n";
	  content += " &nbsp; &nbsp; Min: {{TempMinF}} <br>\n";

	  content += "</p>\n";
	  content += "Hostname: {{myHostName}}\n<br>\n";	// Note: extra '\n' or <br> tag becomes part of the name
	  content += "<a href=\"http://"; //{{DiGiTempServerIP}}/\">DigiTemp Server</a>\n";
	  if(is_AP){
	  	  content += WiFi.softAPIP().toString();
	  }  else {
		  content += WiFi.gatewayIP().toString();
	  }
	  content += "/\">DigiTemp Server</a>\n";
	  content += "</body>";
	  content += "</html>";

	  char humid[20];
	  char tempC[20];
	  char tempF[20];
	  char tempMaxF[20];
	  char tempMinF[20];
	  char tempMaxC[20];
	  char tempMinC[20];
	  sprintf(tempC, "%02.2fC", clients[0].Th_t.c);
	  sprintf(tempF, "%04.2fF", sensor->convertCtoF(clients[0].Th_t.c));
	  sprintf(humid, "%02.1f%%", clients[0].Th_t.h);
	  sprintf(tempMaxF, "%04.2fF", sensor->convertCtoF(clients[0].Th_t.tmax));
	  sprintf(tempMinF, "%04.2fF", sensor->convertCtoF(clients[0].Th_t.tmin));
	  sprintf(tempMinC, "%04.2fC", clients[0].Th_t.tmin);
	  sprintf(tempMaxC, "%04.2fC", clients[0].Th_t.tmax);
	  content.replace("{{TempC}}", String(tempC));
	  content.replace("{{TempF}}", String(tempF));
	  content.replace("{{TempMaxF}}", String(tempMaxF));
	  content.replace("{{TempMinF}}", String(tempMinF));
	  content.replace("{{TempMaxC}}", String(tempMaxC));
	  content.replace("{{TempMinC}}", String(tempMinC));
	  content.replace("{{Humid}}", String(humid));
	  content.replace("{{UID}}", SUID);
	  content.replace("{{MyIP}}", server.client().localIP().toString()); //String(WiFi.localIP().toString()));
	  content.replace("{{myHostName}}", hostName);
	  content.replace("{{DHTTYP}}",sensor->sensor_type());

	  server.send(200, "text/html", content);
	  server.client().flush();

	  if(DEBUG){
		  Serial.println(F("getData() Called"));
		  Serial.println(WiFi.BSSIDstr());
		  Serial.println(WiFi.SSID());	//*
		  Serial.println(WiFi.hostname());
		  Serial.println(WiFi.psk());
		  Serial.println(WiFi.softAPSSID()); //**
		  Serial.println(WiFi.softAPmacAddress());
		  Serial.println(WiFi.softAPIP().toString());
		  Serial.println(WiFi.localIP().toString()); //
		  Serial.print(hostName); Serial.println(".local");
		  Serial.println( server.uri());
		  Serial.println(server.client().localIP().toString());
		  Serial.println(server.client().remoteIP().toString());
		  Serial.println(server.urlDecode(WiFi.SSID()));  //*
		  Serial.println(WiFi.gatewayIP().toString());
	  }
	}

	void toggleCF() { // toggle Temperature scale 'C' of 'F'
		if (DEBUG) {
			Serial.print(F("Hello from toggleCF Switching "));
			if (scale) {
				Serial.println(F("Celsius to Fahrenheit"));
			} else {
				Serial.println(F("Fahrenheit to Celsius"));
			}
		}

		scale = !scale;
	}

	void togglePage(){
		toggleCF();
		yield();
		String content;
		content += "<!DOCTYPE html> \n";
		content += "<html>\n";
		content += "<head>\n";
		content += "<script type=\"text/javascript\">setTimeout(location.replace(\"/\"), 100);</script>\n";
		content += "</head>\n";
		content += "</html>\n ";
		server.send(301, "text/html", content);
		server.client().flush();
		server.client().stop();
	}

	/*
	 *                    Page Not Found
	 *
	 */
	void notFoundPage(void) {
		//bool DEBUG = true;
		if(DEBUG){
			Serial.print(server.uri());
			Serial.println(F(" not found"));
		}
		server.send(404, F("text/plain"), F("404: Not found"));
		server.client().flush();
		server.client().stop();
	}

	/*
	 *
	 * 			reboot()
	 * 			Force a device (Server or Client) to reboot
	 *
	 */
	void rebootPage(void){
		Serial.println("Reboot called");
		server.send(102, "<!DOCTYPE html> <body> <h1> REBOOT </h1> <script language=\"JavaScript\" type=\"text/javascript\"> setTimeout(\"window.history.go(-1)\",10000); </script></body></html>"); // go back after 10 seconds 1000 = 1 second
		server.client().flush();
		ESP.restart();
		// ESP.reset();
	}

	/*
	 *
	 *
	 *
	 */
	void stationPage(void){
		  if(DEBUG) Serial.print(F("Hello from updateStation"));
		  if(server.hasArg("id")){
			  int Station_Number = server.arg("id").toInt();
			  if(DEBUG){  Serial.print(F(" ID=")); Serial.println(Station_Number); }
		  }
		  //	reset all
		  clients[0].Th_t = sensor->read(); //read_Sensor();
		  root_Page();
		  if(DEBUG) Serial.println();

	}

	/*
	 *                                            Setup mDNS responder
	 *			Allows this device to be found on local network by using {nyhostname}.local instead of IP address
	 *			using an mDNS Client or Bonjour
	 *
	 */
	void  setupmDNS() {
	  // Set up mDNS responder:
	  if (!MDNS.begin(hostName)) {
	    Serial.println(F("Error setting up MDNS responder!"));
	  }
	  Serial.print(F("mDNS responder started connect using http://"));
	  Serial.print(hostName);
	  Serial.println(F(".local"));
	}

	void do_serial(char r){
    switch(r){
    	case '?':
    		Serial.println(info());
    	    Serial.println(WiFi.localIP().toString());
    	    Serial.println(WiFi.softAPIP().toString());
    	    break;
    	case	'0':		// change the WatchDog timeout for testing
    	case	'1':
    	case	'2':
    	case	'3':
    	case	'4':
    	case	'5':
    	case	'6':
    	case	'7':
    	case	'8':
    	case	'9':
    		Serial.print(F("Changing still_here from "));
    		Serial.print(still_here);
    		Serial.print(" to ");
    		Serial.println(r);
    		still_here = r-'0';
    		break;
    	case	'r':		// Force sensor read
    	case	'R':
    		clients[0].Th_t = sensor->read(); //read_Sensor();
    		Load_Client_List(true);
    		break;
    	case	'w':
    	case	'W':
    		still_here = -1;
    		break;
    	case	'\n':
    	case	'\r':
    		// Do nothing
    		break;
    	default:
    		Serial.print(r);
    }
}


	// WiFiEventSoftAPModeStationConnected  Manage incoming device connection on ESP access point
	void onNewStation(WiFiEventSoftAPModeStationConnected sta_info) {
	  if(DEBUG){
		  Serial.print(F("New Station : "));
		  Serial.print(sta_info.aid);
		  Serial.print(" ");
	  }

	  newClient = true;
	}

	//WiFiEventStationModeDisconnected
	void onRemoveStation(WiFiEventSoftAPModeStationDisconnected sta_info) {
	  if(DEBUG){
		  Serial.println(F("Station Removed: "));
		  Serial.print(sta_info.aid);
		  Serial.print(" ");
	  }
	  newClient = true;
	}


/*
 *
 *
 *
 */
bool connectAP() {
	static bool isAP = false;
	String m_ssid;
	int32_t rssi;
	uint8_t encryptionType;
	uint8_t *bssid;
	int32_t channel;
	bool hidden;
	int scanResult;

	hostName = MY_HOSTNAME;
	unsigned short max_connection = 8;
	//IPAddress local_IP(192,168,40,1);
	//IPAddress gateway(192,168,40,1);
	IPAddress local_IP(APIP);
	IPAddress gateway = local_IP;
	IPAddress subnet(255,255,255,0);

	Serial.println(F("Starting WiFi scan..."));

	isAP = true;
	scanResult = WiFi.scanNetworks(/*async=*/false, /*hidden=*/true);
	if (scanResult <= 0) {
		Serial.println(F("No networks found"));
		isAP = true;
	} else { // if (scanResult > 0) {
		Serial.printf(PSTR("%d networks found:\n"), scanResult);
		// Print unsorted scan results
		for (int8_t i = 0; i < scanResult; i++) {
			WiFi.getNetworkInfo(i, m_ssid, encryptionType, rssi, bssid, channel,
					hidden);

			Serial.printf(
					PSTR(
							"  %02d: [CH %02d] [%02X:%02X:%02X:%02X:%02X:%02X] %ddBm %c %c %s\n"),
					i, channel, bssid[0], bssid[1], bssid[2], bssid[3],
					bssid[4], bssid[5], rssi,
					(encryptionType == ENC_TYPE_NONE) ? ' ' : '*',
					hidden ? 'H' : 'V', m_ssid.c_str());
			yield();
			if (m_ssid == ssid) {
				Serial.printf("%s == %s ", ssid, m_ssid.c_str());
				Serial.println(F(" working as client"));
				isAP = false;
				break;
			}
		} // for loop
	} // Scan network
	/* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
	 would try to act as both a client and an access-point and could cause
	 network-issues with your other WiFi-devices on your WiFi-network. */

	if (isAP) {
		WiFi.mode(WIFI_AP);
		Serial.println(F("\t\t**** WiFI AP ****"));
		Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");
		Serial.println(WiFi.softAP(ssid, password, channel, false, max_connection) ? "Ready" : "Failed!");

		static WiFiEventHandler e1 = WiFi.onSoftAPModeStationConnected(onNewStation);
		static WiFiEventHandler e2 = WiFi.onSoftAPModeStationDisconnected(onRemoveStation);

	} else {
		WiFi.mode(WIFI_STA);
		Serial.println(F("\t\t**** WiFI Station ****"));
		WiFi.begin(ssid, password);
		while (WiFi.status() != WL_CONNECTED) {
			delay(500);
			Serial.print(".");
			yield();
		}
	}


	Serial.printf("\n\rWiFi connected IP address: %s ",WiFi.localIP().toString().c_str());
	return isAP;
}

/*
 *
 *         load client array with IP and isAlive boolean.
 *         reload
 *          - true force isAlive active
 *          - false keep current client's isAlive status
 *
 */
void Load_Client_List(boolean reload) {
Serial.print(F("============================ Load_Client_List ========================="));
if(reload) Serial.println(F(" TRUE")); else Serial.println(F(" FALSE")); Serial.flush(); yield();
  Serial.printf("New Station Number of connections : %i\n", WiFi.softAPgetStationNum());
  int ipidx = 0;
  // Special case for this AP enabled with DHT always stored in location 0
  clients[ipidx].ipAddress = WiFi.softAPIP();
  if(reload){ clients[ipidx].isAlive = true; }
  Serial.print(ipidx); Serial.print(":\t"); Serial.println(clients[ipidx].ipAddress.toString());
  ipidx++;  // ready for remote clients

  // Add the remote clients
  struct station_info *station_list = wifi_softap_get_station_info();
  while (station_list != NULL) {
    clients[ipidx].ipAddress = IPAddress((&station_list->ip)->addr);
    Serial.print(ipidx); Serial.print(":\t"); Serial.println(clients[ipidx].ipAddress.toString());
    if(reload){ clients[ipidx].isAlive = true; }  // Default to active until get_Data() is verified
    station_list = STAILQ_NEXT(station_list, next);
    ipidx++;
  }
  /* clear the remaining unused locations */
  while( ipidx <= max_connection){
	  clients[ipidx].ipAddress=IPAddress();
	  clients[ipidx].myHostName.clear();
	  clients[ipidx].isAlive = false;
	  clients[ipidx].last_check = 0;
	  clients[ipidx].Th_t.c = INVALID;		// Flag uninitialized
	  clients[ipidx].Th_t.h = INVALID;		// Flag uninitialized
	  ipidx++;
  }
  wifi_softap_free_station_info();
}


/*
 *
 *   Find the substring "key" in the string s and return the value associated with it.
 *   value is the chars following the white space after the key
 *
 */
float  parser(const String* s, const char* key) {
  float retVal = NAN;

  // Search for key
  if (s->indexOf(key) >= 0) {   // Found key
	unsigned int ptr = s->indexOf(key);
    //compute end of key
    ptr += strlen(key);

    // remove white space
    while (isspace(s->charAt(ptr)) && ptr < s->length()) {
      ptr++;
    } // ptr now at start of value or EOLN
    unsigned int stptr = ptr;    // start of the value

    // find end of value
    while ((isdigit(s->charAt(ptr)) || s->charAt(ptr) == '.') && ptr < s->length()) {
      ptr++;
    }
    if(ptr > stptr){ 	// Not a number
    	retVal = s->substring(stptr, ptr).toFloat();
    }
  }
  return retVal;
}

/*
 *
 * 				Update remote station values
 *
 */
boolean read_Client(int Station_Number){
	boolean retVal = false;
	//Load_Client_List(true);	// Some reason the clist is not loaded on new client so update clist each call

	if(DEBUG){
		Serial.print(F("Read Client "));
		Serial.print(Station_Number);
		Serial.print(" / ");
		Serial.println(WiFi.softAPgetStationNum());
	}

	if (clients[Station_Number].isAlive) {
		retVal = clients[Station_Number].isAlive = checkStation(&clients[Station_Number]);
	}

	if(DEBUG){ if(retVal) Serial.println(F(" is Active ")); else Serial.println(F(" is Not Alive "));}


	if(DEBUG) for(int s=0; s <= max_connection; s++){
		Serial.print(s);
		Serial.print(F("\tHostname: ")); if(clients[s].myHostName == "" ) Serial.print("\t\t\t"); else Serial.print(clients[s].myHostName);
		if(clients[s].isAlive) Serial.print(F("\tAlive"));	else Serial.print(F("\tDead "));
		Serial.print(F("\tIP Address: ")); Serial.print(clients[s].ipAddress);
		Serial.print(F("\tLast read: ")); Serial.println( (int) ((millis() - clients[s].last_check) / 1000));
	}

	return retVal;
}

/**
 *                                                 checkStation()
 *                  verify active stations client array still exists and fill structure clist with data
 *
 *
 */
boolean checkStation(clist* addss) {
  addss->last_check = millis();

  if (addss->ipAddress == WiFi.softAPIP()) {	// Local station
    return true; //checkLocalStation(addss);    // Access to self through network not feasible
  }

  String url = "/getData";
  String host = addss->ipAddress.toString();

  Serial.print("Connecting to ");
  Serial.print(host + url);

  WiFiClient client;

  //Serial.print("looking for 404 ");  //Serial.println(client.connect(host, 80));
  if (!client.connect(host, 80)) {
    Serial.println(F(" *fail* ")); Serial.println(client.status());
    addss->isAlive = false;
    return false;
  } else {
    Serial.println(F(" Success "));
    addss->isAlive = true;
  }

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n" +
               "\r\n"
              );
  delay(10); yield();
  int ptr;
  int eoln;
  float temp;
  String temps;
  addss->myHostName = addss->ipAddress.toString();	// default
  while (client.connected() || client.available()) {
    if (client.available()) {
  	  String line = client.readStringUntil('\n');
      if(line.indexOf("404") > 0){
        Serial.println(F(" ************************** 4 0 4 ********************************* "));
        addss->isAlive = false;
        client.stop();
        return false;
      } else {
        // Humidity
        ptr = line.indexOf("Humidity:");    // Quick check for now need to modify parser to return boolean and something else for the retVal
        if (ptr >= 0) {
          temp = parser(&line, "Humidity:");
          addss->Th_t.h = temp;
        }

        // Temperature (both)
        ptr = line.indexOf("Temperature:");
        if (ptr >= 0 && (line.indexOf("C") >ptr)) {
          temp = parser(&line, "Temperature:");
          addss->Th_t.c = temp;
        }

        ptr = line.indexOf("Max:");
        if (ptr >= 0 && (line.indexOf("C") >ptr) ) {
          temp = parser(&line, "Max:");
          addss->Th_t.tmax = temp;
        }

        ptr = line.indexOf("Min:");
        if (ptr >= 0 && (line.indexOf("C") > ptr)) {
          temp = parser(&line, "Min:");
          addss->Th_t.tmin = temp;
        }

        // Hostname
        ptr = line.indexOf("Hostname:");
        if (ptr >= 0) {
          eoln = line.length();
          if (eoln > ptr) { // JIC WTGBIN
            temps = line.substring((ptr + 10), eoln);
            addss->myHostName = temps;
          }
        }
      }
    }
    yield();
  } // while connected
  client.stop();
  return true;
}

#endif	// _DiGiTempESP_V4_H_
