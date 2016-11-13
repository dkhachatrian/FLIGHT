// include headers

#include <WiFi101/src/WiFi101.h> //for WiFi
#include <WiFi\src\WiFiUdp.h>

WiFiUDP Udp;

unsigned wifi_status = WL_IDLE_STATUS;

const unsigned MAX_LENGTH = 255;
const unsigned BAUD_RATE = 9600; //bits-per-second

// sensor names



void setup()
{

	// setup_WiFi()

	Serial.begin(BAUD_RATE);

	while (!Serial) {
		; // wait for serial port to connect. Needed for native USB port only
	}

	// get wifi info
	char ssid[MAX_LENGTH];
	char pass[MAX_LENGTH];


	while (wifi_status != WL_CONNECTED)
	{


		// get SSID
		Serial.print("Please provide the SSID (name of WiFi network) to connect to, then press Enter or Return:\n");
		Serial.readBytesUntil('\n', ssid, MAX_LENGTH);
		// get pass

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

void loop()
{
	// collect_data()

	// prepare packet

	// for each sensor (connected via analog pins???? digital pins????)
		// collect data
		// add to string
		// store string in array, delimited by DELIMITER
	
	

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

// Waits until a serial connection is established (via native USB port)
// then asks for WiFi network credentials and attempts to connect
void setup_WiFi()
{
	//
}
