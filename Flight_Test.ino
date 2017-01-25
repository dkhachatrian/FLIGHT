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
//#define USING_WIFI

// just making sure things print as expected
//#define BAREBONES













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


//binType vals_ecg[LENGTH_ECG]; //ecg bin numbers
//binType vals_po[size_t(MAX_LENGTH/10)]; // polled less often than ECG circuit
//binType val_temp; //temperature reading



//// sensor names
//// TODO: move declarations to a header file
//
//// will use enum, associate names with pins
//// TODO: match with proper pin numbers
//enum LED
//{
//	red = 13, //Pin #13 red LED for general purpose blinking
//	green = 1,
//	blue = 3
//};





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

}


void loop()
{
  #ifdef BAREBONES
  Serial.println("Hi I'm in loop");
  #else
  
	// collect_data()

	// prepare packet

  //timestamp for start of loop
  unsigned long t = millis();


  // collect data
  Serial.println("Before update_temp");
  readings.update_temp();
  Serial.println("Before update_continuous_readings");
  update_continuous_readings(TIME_CTS);

//  Serial.println("Before simpleTimer.run()...");
//
//  simpleTimer.run();

  // wait until simpleTimers are finished before running
  // technically, with these simpleTimers set up, can do other work
  // in the meantime
  // TODO: utilize dead time here...


//  while(simpleTimer.getNumTimers() != 0) {
//    Serial.println("In simpleTimer loop");
//    }

  
  // package_data_into_sendable_string()

  // package data into one string
//  package_data_to_str();

  String data_str = readings.package_data(t);
  char buf[1000];
  data_str.toCharArray(buf, 1000);

  #ifdef USING_WIFI

  Serial.println(data_str);
  
  // close any connection before send a new request.
  // This will free the socket on the WiFi shield
  client.stop();

  
  // if there's a successful connection:
  if (client.connect(server, port)) {
//  // if still connected to WiFi
//  if(wifi_status == WL_CONNECTED && client.connected()){
    // send data
    // (ideally will be slower...)
//    int str_len = data_str.length();
//    for(int i = 0; i < str_len; i++){
//      client.write(data_str[i]);
//    }
    Serial.println("Connected to server! Printing buf...");
    client.println("Hi there!");
    client.println(data_str);
//    client.println(buf);
//    Serial.println(buf);
//    for(int i = 0; i < 5; i++){
//      client.println(String(i));
//      client.println(buf);
//    }
  }
  else {
    Serial.println("Didn't connect to server...");
  }

//  client.stop();
//  delay(5000);
//  client.stop();

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

  #else //debug over Serial
  Serial.println(data_str);
//  delay(1000); //give time to notice different line
  #endif

  readings.forget();

  #endif
}





//// to make SimplesimpleTimer.simpleTimer.setsimpleTimer happy
//// (try to fix code to not need this?)
//void add_ecg_data_wrapper(){
//  readings.add_ecg_data();
//}
//void add_po_data_wrapper(){
//  readings.add_pulse_ox_data();
//}


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




// std not in Arduino libraries... 
//
//  ExampleFunction f1 = std::bind(Data::add_ecg_data, &readings, std::placeholders::_1);
//  ExampleFunction f2 = std::bind(Data::add_pulse_ox_data, &readings, std::placeholders::_1);
//  int ecg_timeId = simpleTimer.setsimpleTimer(TIME_ECG, f1, LENGTH_ECG);
//
//  int po_timeId = simpleTimer.setsimpleTimer(TIME_PO, f2, LENGTH_PO);
//
  
//  Serial.println("Before ecg_timeId...");
//  
//  int ecg_timeId = simpleTimer.setTimer(TIME_ECG, add_ecg_data_wrapper, LENGTH_ECG);
//
//  Serial.println("Before po_timeId...");
//  //'/2' because two readings per call
//  int po_timeId = simpleTimer.setTimer(TIME_PO, add_po_data_wrapper, LENGTH_PO/2); 


//  ctrType i = 0; //ecg counter
//  ctrType j = 0; //po counter
//  ctrType j_max = LENGTH_PO;
//  unsigned long t = millis();
//
//
//
//  // polling time elapsed with millis() will slow down acquisition ...
//  // TODO: test how fast hardware can be polled? Then can use i to determine when to switch
//
//  while(j < j_max) // stop after filling up pulse_ox array
//  {
//    
//    while(((millis() - t) < TIME_PO) && (i < LENGTH_ECG)){
//
//
//    }
//    // if here, either time elapsed or reached end of buffer
//    // return if end of buffer
//    if(i >= LENGTH_ECG) { break; }
//
//    //first, add to pulse_ox reading array
//    add_pulse_ox_data(j);
//    j+=2; //'2' because two readings added to pulse-ox (see documentation for add_pulse_ox_data
//    // update interval
//    t = millis();
//    
//
//    
//  }
//
//  // update lengths
//  *len_ecg = i;
//  *len_po = j;
//
//  return;
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
