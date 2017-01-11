#ifndef FLIGHT_TEST
#define FLIGHT_TEST

// packages

#include <WiFi101.h> //for WiFi
#include <WiFiUdp.h>
#include <SPI.h>

#include "typedefs.h"

// macros //

#define RESOLUTION_ADC 12 //12-bit ADC

#define PIN_THERMISTOR 1
#define PIN_ECG_L 10
#define PIN_ECG_R 11
#define PIN_PULSEOX 15
#define PIN_PULSEOX_RED_SWITCH 16 //if on(high voltage), red range; else, IR (???)
#define LED_BUILTIN 13 //is a red LED


// constants //

const int WiFi_pins[4] = { 8, 7, 4, 2 }; // as per documentation

//number of milliseconds to collect "continuous" data at a time
const time_t TIME_ECG = 1000;

//number of milliseconds to wait between check pulse ox data
//assuming max heart rate of ~200 bpm =~ 3 Hz, doubling frequency and then some to go to 8 Hz (to avoid aliasing),
//and reciprocating gives a time interval of 1/8 s ~ 125 ms
const time_t TIME_PO = 125;


const short LENGTH_PO = short (2 * TIME_ECG/TIME_PO); //'2' because two values needed each reading (red and IR(???))


//max packet size that website can receive is 32 kB
// we only have so much RAM, will make larger array size 5k (if 4-bit ints --> 20 kiB)
const unsigned MAX_LENGTH = 5000; 

// how fast the serial connection to e.g. laptop should go
const unsigned BAUD_RATE = 9600; //bits-per-second

// unsigned int localPort = 2390;      // local port to listen on


//declare buffers/arrays to pass into functions
//char str_buf[MAX_LENGTH];


const sizeType ECG_BUF_LEN = sizeType(MAX_LENGTH/sizeof(binType));



//// String Labels and Delimiters

const char DELIMITER = '\t';
const String LABEL_TIME = "TIMESTAMP:";
const String LABEL_ECG = "ECG:";
const String LABEL_PO = "PO:";
const String LABEL_TEMP = "TEMP:";
const String LABEL_END_OF_PACKET = "\n";





// function declarations //

void update_continuous_readings(sizeType* len_ecg, sizeType* len_po);

void add_pulse_ox_data(sizeType len_po_cur);

void setup_WiFi();

void update_temp(binType *val_temp);

void printWifiStatus();

void listNetworks();

void printEncryptionType(int thisType);

void printMacAddress();






// claas Data implementation

// use struct -- keeps bytes continuous
// (easier to chunk for encryption)
struct Data{
  binType vals_ecg[ECG_BUF_LEN]; //ecg bin numbers
  binType vals_po[size_t(ECG_BUF_LEN/10)]; // polled less often than ECG circuit
  binType val_temp; //temperature reading
  sizeType len_ecg;
  sizeType len_po;
//  sizeType len_temp; // always 1
};






#endif
