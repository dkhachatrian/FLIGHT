// include headers

#include "Flight_Test.h"





WiFiUDP Udp;

unsigned wifi_status = WL_IDLE_STATUS;


binType vals_ecg[ECG_BUF_LEN]; //ecg bin numbers
binType vals_po[size_t(MAX_LENGTH/10)]; // polled less often than ECG circuit
binType val_temp; //temperature reading





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
  //use all bits of ADC (not default of 10)
  analogReadResolution(RESOLUTION_ADC);
  
	//Configure pins for Adafruit ATWINC1500 Feather
	WiFi.setPins(8, 7, 4, 2);
	setup_WiFi();

	// setup_WiFi()


  


}

void loop()
{
	// collect_data()

	// prepare packet

  //timestamp for start of loop

  sizeType len_ecg = 0;
  sizeType len_po = 0;
  
  unsigned long t = millis();

	// for each sensor (connected via analog pins???? digital pins????)
		// collect data
		// add to string
		// store string in array, delimited by DELIMITER
    
  update_temp(&val_temp);

  update_continuous_readings(&len_ecg, &len_po);
  
  // package_data_into_sendable_string()

  // encrypt string(?)

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
void update_temp(binType *val_temp) {
  *val_temp = analogRead(PIN_THERMISTOR);
  // convert to temp? Would need float instead of short
  return;
}

// ping the ECG pins and pulse-ox pins alternately for ECG and pulse-ox measurements
// replaces arguments with length of arrays at the end of the function call
void update_continuous_readings(size_t* len_ecg, size_t* len_po){

  ctrType i = 0; //ecg counter
  ctrType j = 0; //po counter
  ctrType j_max = LENGTH_PO;
  unsigned long t = millis();



  // polling time elapsed with millis() will slow down acquisition ...
  // TODO: test how fast hardware can be polled? Then can use i to determine when to switch

  while(j < j_max) // stop after filling up pulse_ox array
  {
    
    while(((millis() - t) < TIME_PO) && (i < ECG_BUF_LEN)){
      // populate ECG in between pulse_ox readings
      // store difference of V_right and V_left
      vals_ecg[i] = analogRead(PIN_ECG_R) - analogRead(PIN_ECG_L);
      i++;
    }
    // if here, either time elapsed or reached end of buffer
    // return if end of buffer
    if(i >= ECG_BUF_LEN) { break; }

    //otherwise, add to pulse_ox reading array
    add_pulse_ox_data(j);
    j+=2; //'2' because two readings added to pulse-ox (see documentation for add_pulse_ox_data
    // update interval
    t = millis();
    

    
  }

  // update lengths
  *len_ecg = i;
  *len_po = j;

  return;
}


// Polls for voltage readings corresponding to absorbances for red and IR
// will add in the order red then IR
// will have the PIN_PULSEOX_RED_SWITCH to LOW (pulseox LED set to IR (lower power))
// on function return
// updating of length is handled by caller -- len_po_cur is just used to find slots
// to place read values
void add_pulse_ox_data(sizeType len_po_cur){
  digitalWrite(PIN_PULSEOX_RED_SWITCH, HIGH); //LED now shooting at red range
  vals_po[len_po_cur] = analogRead(PIN_PULSEOX);
  digitalWrite(PIN_PULSEOX_RED_SWITCH, LOW); //LED now shooting at IR range
  vals_po[len_po_cur + 1] = analogRead(PIN_PULSEOX);
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
