#include <ArduinoJson.h>

// toggles (mainly for tests)



// leave uncommented to have microcontroller directly calculate
// relevant physiological data from the voltage readings
// (instead of having a separate recipient do this)
//
// For debugging only (performing this on the microcontroller
// will slow things down)
#define PERFORM_CALCULATIONS




// if defined, generates numbers as fake sensor voltage readings
// and uses built-in values for ssid and pass to
// quickly connect to wifi
#define DUMMY_SENSORS

// if defined, tries connecting with wifi
// otherwise, testing values through serial
// (e.g. test packet string contents)
#define USING_WIFI

// just making sure things print as expected
//#define BAREBONES


// seems like we're using Firebase as our server now?
// so have that toggle for now
#define USING_FIREBASE










//#include <AES.h>

// include headers


#include "Flight_Test.h"
//#include "SimpleTimer.h"
#include "Data.h"
#include "typedefs.h"
#include "consts.h"

#include <SPI.h>
#include <WiFi101.h>
//
//#include <Event.h>
//#include <Timer.h> // from http://playground.arduino.cc/Code/Timer



#ifdef DUMMY_SENSORS

binType x = 0;

// generates numbers linearly mod 4096
binType generate_num(){
  binType result = x;
  #ifdef PERFORM_CALCULATIONS
  x = (x+1);
  #else
  x = (x+1)%4096;
  #endif
  return result;
}

#endif




//WiFiUDP Udp;
//WiFiSSLClient client; // SSL handles encryption for us!
WiFiClient client; // testing with 127.0.0.1

//SimpleTimer simpleTimer; // for ECG/pulse-ox timing

Data readings;


unsigned wifi_status = WL_IDLE_STATUS;

//IPAddress server(192,168,1,1);
char server[MAX_INPUT_LENGTH];
String auth_code;
int port;



void setup()
{
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }

  Serial.println("Entered setup.");
  //use all bits of ADC (not default of 10)
  analogReadResolution(RESOLUTION_ADC);


  #ifdef USING_WIFI
	//Configure pins for Adafruit ATWINC1500 Feather
	WiFi.setPins(8, 7, 4, 2);
	setup_WiFi();
  #endif
  setup_server();

}


// get info for server to connect to
void setup_server()
{
  
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  //get server info

  #ifndef DUMMY_SENSORS
  char server[MAX_INPUT_LENGTH]; //  your network SSID (name)
  char auth_code[MAX_INPUT_LENGTH]; // your network password (use for WPA, or use as key for WEP)
  char port_str[10];
  port = 0;
  #else
  // test network, set up by other microcontroller
  char server[] = "flight-time-525c8.firebaseio.com"; //  your network SSID (name)
  char auth_code[] = ""; // your network password (use for WPA, or use as key for WEP)
  int port = 443;
  #endif



  while (true)
  {
    #ifndef DUMMY_SENSORS

    // get server name
    Serial.print("Please provide the name of the server to connect to, then press Enter or Return:\n");
    Serial.readBytesUntil('\n', server, MAX_INPUT_LENGTH);
    // get auth code
    Serial.print("Please input the authorization code to talk to the server, then press Enter.\n"
    Serial.print("(Note: will not be able to test authorization code until readings are being sent, so input carefully):\n");
    Serial.readBytesUntil('\n', auth_code, MAX_INPUT_LENGTH);
    // get port number
    while(port == 0)
    {
      Serial.print("Please input the port the server will be listening on, then press Enter");
      Serial.print(" (hint: standard HTTP port is 80 and standard HTTPS port is 443":\n");
      Serial.readBytesUntil('\n', port_str, MAX_INPUT_LENGTH);
      port = atoi(port_str); //if atoi fails, returns 0
    }

    #endif


    // attempt connection

    Serial.println("Attempting to connect to server " + String(server)+ " on port " +String(port) + " ...");
    int i = 0;
  
    while(!client.connected() && i < 3)
    {
      client.connect(server, port);
      i +=1;
    }
  
    if(!client.connected())
    {
      Serial.println("Connecting to server failed...");
    }
     else if (client.connected())
     {
      // we're good!
      // get out of loop
      Serial.println("Connection to server succeeded!");
      break;
     }
  }
}


void loop()
{
  #ifdef BAREBONES
  Serial.println("Hi I'm in loop");
  #else
  
	// prepare packet

  //timestamp for start of loop
  unsigned long t = millis();


  // collect data
  Serial.println("Before update_temp");
  readings.update_temp();
  Serial.println("Before update_continuous_readings");
  update_continuous_readings(TIME_CTS);

  // wait until simpleTimers are finished before running
  // technically, with these simpleTimers set up, can do other work
  // in the meantime
  // TODO: utilize dead time here...


  String data_str = readings.package_data(t);

  #ifdef USING_WIFI

  Serial.println(data_str);
  
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  
  // if there's a successful connection:
  if (client.connect(server, port)) {
    Serial.println("Connected to server! Printing data_str...");
    client.println(data_str);
  }
  else {
    Serial.println("Didn't connect to server...");
  }

  #else //debug over Serial
  Serial.println(data_str);
//  delay(1000); //give time to notice different line
  #endif

  readings.forget();

  #endif
}





// ping the ECG pins and pulse-ox pins alternately for ECG and pulse-ox measurements
// will collect readings for interval number of milliseconds
// replaces arguments with length of arrays at the end of the function call
void update_continuous_readings(time_t interval){

  
  bool updated_ecg = true;
  bool updated_po = true;
  unsigned long t_total = millis();
  unsigned long t_ecg = millis();
  unsigned long t_po = millis();

  while(millis() - t_total < interval)
  {
    //after enough time
    if(updated_ecg && (millis()-t_ecg >= TIME_ECG)){
      updated_ecg = readings.add_ecg_data(); //do the thing
      t_ecg = millis(); //reset the timer
    }
    if(updated_po && (millis()-t_po >= TIME_PO)){
      updated_po = readings.add_pulse_ox_data();
      t_po = millis();
    }
  }

  Serial.println("Exiting update_continuous_readings.");


  // prepared simpleTimer for loop()

  return;



}




// Waits until a serial connection is established (via native USB port)
// then asks for WiFi network credentials and attempts to connect.
// (Currently does not listen for any incoming packets.)
void setup_WiFi()
{
	Serial.begin(BAUD_RATE); // sets bandwidth for serial connection

	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// get wifi info

  #ifndef DUMMY_SENSORS
	char ssid[MAX_INPUT_LENGTH]; //  your network SSID (name)
	char pass[MAX_INPUT_LENGTH]; // your network password (use for WPA, or use as key for WEP)
  #else
  // test network, set up by other microcontroller
  char ssid[] = "wifi101-network";
  char pass[] = "1234567890";
  #endif

	// Print WiFi MAC address:
	printMacAddress();

	// scan for existing networks:
	Serial.println("Scanning available networks...");
	listNetworks();



	while (wifi_status != WL_CONNECTED)
	{
    #ifndef DUMMY_SENSORS

		// get SSID
		Serial.print("Please provide the SSID (name of WiFi network) to connect to, then press Enter or Return:\n");
		Serial.readBytesUntil('\n', ssid, MAX_LENGTH);
		// get pass
		Serial.print("Please input the password, then press Enter:\n");
		Serial.readBytesUntil('\n', pass, MAX_LENGTH);
    #endif


		// attempt connection

//		wifi_status = WiFi.begin(ssid, pass); //for encrypted networks 
    wifi_status = WiFi.begin(ssid); //for non-encrypted networks
    delay(10000); //give time to connect (necessary?)

		// tell user if it couldn't connect
		if (wifi_status != WL_CONNECTED)
		{
			Serial.print("Network not found! Please double-check SSID and password.\n");
		}
    else //show info for what it connected it to
    {
      Serial.println("Connected! Displaying information... \n");
      printWifiStatus();
    }
	}
	// TODO: have green LED while connected and signal strength control intensity,
	// red LED when disconnected ?

	// tell user to unplug from computer, set up on patient
	// 
//
//  Serial.println("Attempting to connect to server on port " +String(port) + " ...");
//
//  int i = 0;
//
//  while(!client.connected() && i < 10)
//  {
//    client.connect(server, port);
//    i +=1;
//  }
//
//  if(!client.connected())
//    Serial.println("Connecting to server failed...");
//   else
//    Serial.println("Connection to server succeeded!");

}



void printWifiStatus() {
	// print the SSID of the network you're attached to:
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());

	// print your WiFi shield's IP address:
	IPAddress ip = WiFi.localIP();
	Serial.print("IP Address: ");
	Serial.println(ip);

	// print the received signal strength:
	long rssi = WiFi.RSSI();
	Serial.print("signal strength (RSSI):");
	Serial.print(rssi);
	Serial.println(" dBm");
}




void listNetworks() {
	// scan for nearby networks:
	Serial.println("** Scan Networks **");
	int numSsid = WiFi.scanNetworks();
	if (numSsid == -1)
	{
		Serial.println("Couldn't get a wifi connection");
		while (true);
	}

	// print the list of networks seen:
	Serial.print("number of available networks:");
	Serial.println(numSsid);

	// print the network number and name for each network found:
	for (int thisNet = 0; thisNet < numSsid; thisNet++) {
		Serial.print(thisNet);
		Serial.print(") ");
		Serial.print(WiFi.SSID(thisNet));
		Serial.print("\tSignal: ");
		Serial.print(WiFi.RSSI(thisNet));
		Serial.print(" dBm");
		Serial.print("\tEncryption: ");
		printEncryptionType(WiFi.encryptionType(thisNet));
		Serial.flush();
	}
}

void printEncryptionType(int thisType) {
	// read the encryption type and print out the name:
	switch (thisType) {
	case ENC_TYPE_WEP:
		Serial.println("WEP");
		break;
	case ENC_TYPE_TKIP:
		Serial.println("WPA");
		break;
	case ENC_TYPE_CCMP:
		Serial.println("WPA2");
		break;
	case ENC_TYPE_NONE:
		Serial.println("None");
		break;
	case ENC_TYPE_AUTO:
		Serial.println("Auto");
		break;
	}
}

void printMacAddress() {
	// the MAC address of your Wifi shield
	byte mac[6];

	// print your MAC address:
	WiFi.macAddress(mac);
	Serial.print("MAC: ");
	Serial.print(mac[5], HEX);
	Serial.print(":");
	Serial.print(mac[4], HEX);
	Serial.print(":");
	Serial.print(mac[3], HEX);
	Serial.print(":");
	Serial.print(mac[2], HEX);
	Serial.print(":");
	Serial.print(mac[1], HEX);
	Serial.print(":");
	Serial.println(mac[0], HEX);
}
