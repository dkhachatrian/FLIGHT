// include headers

#include <WiFi101.h> //for WiFi
#include <WiFiUdp.h>
#include <SPI.h>


// input pin definitions/macros

#DEFINE RESOLUTION_ADC 12 //12-bit ADC

#DEFINE PIN_THERMISTOR 1
#DEFINE PIN_ECG_L 10
#DEFINE PIN_ECG_R 11
#DEFINE PIN_PULSEOX 15
#DEFINE PIN_PULSEOX_SWITCH 16 //if on(high voltage), UV range; else, blue (???)
#DEFINE LED_BUILTIN 13 //is a red LED


const int WiFi_pins[4] = { 8, 7, 4, 2 }; // as per documentation

//number of milliseconds to collect "continuous" data at a time
const time_t TIME_CONTINUOUS_ACQUISITION = 1000;


WiFiUDP Udp;

unsigned wifi_status = WL_IDLE_STATUS;

const unsigned MAX_LENGTH = 32000; //max packet size is 32 kb
const unsigned BAUD_RATE = 9600; //bits-per-second

// unsigned int localPort = 2390;      // local port to listen on



// sensor names
// TODO: move declarations to a header file

// will use enum, associate names with pins
// TODO: match with proper pin numbers
enum LED
{
	red = 13, //Pin #13 red LED for general purpose blinking
	green = 1,
	blue = 3
};





void setup()
{
  //use all bits of ADC (not default of 10)
  analogReadResolution(RESOLUTION_ADC);
  
	//Configure pins for Adafruit ATWINC1500 Feather
	WiFi.setPins(8, 7, 4, 2);
	setup_WiFi();

	// setup_WiFi()

  //initialize buffers/arrays to pass into functions
  char str_buf[];
  short vals_ecg[];
  short vals_po[];
  short val_temp;
  


}

void loop()
{
	// collect_data()

	// prepare packet

  //timestamp for start of loop
  time_t t = now();

	// for each sensor (connected via analog pins???? digital pins????)
		// collect data
		// add to string
		// store string in array, delimited by DELIMITER
    
  update_temp(&val_temp);

  update_continuous_readings(vals_ecg, vals_po, TIME_CONTINUOUS_ACQUISITION);
  
  //update_ecg(vals_ecg);
  //update_po(vals_po);

	// if still connected to WiFi
		//send_data()

		// if longer-term storage-packet is nonempty
			// send that
			// clear storage
		// prepare small packet with array as contents
		// send packet across WiFi
	// else
		// store_data()

		// store in a "longer-term" packet
		// light up warning LED
		// stop and setupWiFi() again?

	// wait a certain amount of time depending on acquisition rate (?)
}

// ping PIN_THERMISTOR to get new voltage binning for val_temp
// Does *not* convert to temperature
// (Temperature formula would be ???T)
void update_temp(short *val_temp) {
  *val_temp = analogRead(PIN_THERMISTOR);
  // convert to temp? Would need float instead of short
  return;
}

// ping the ECG pins and pulse-ox pins alternately for
void update_continuous_readings(short vals_ecg[], short vals_po[]);



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

	char ssid[MAX_LENGTH]; //  your network SSID (name)
	char pass[MAX_LENGTH]; // your network password (use for WPA, or use as key for WEP)


	// Print WiFi MAC address:
	printMacAddress();

	// scan for existing networks:
	Serial.println("Scanning available networks...");
	listNetworks();



	while (wifi_status != WL_CONNECTED)
	{

		// get SSID
		Serial.print("Please provide the SSID (name of WiFi network) to connect to, then press Enter or Return:\n");
		Serial.readBytesUntil('\n', ssid, MAX_LENGTH);
		// get pass
		Serial.print("Please input the password, then press Enter:\n");
		Serial.readBytesUntil('\n', pass, MAX_LENGTH);


		// attempt connection

		wifi_status = WiFi.begin(ssid, pass);

		// tell user if it couldn't connect
		if (wifi_status != WL_CONNECTED)
		{
			Serial.print("Network not found! Please double-check SSID and password.\n");
		}

	}
	// TODO: have green LED while connected and signal strength control intensity,
	// red LED when disconnected ?

	// tell user to unplug from computer, set up on patient
	// 

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
