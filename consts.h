#ifndef CONSTS
#define CONSTS

#include "typedefs.h"

// macros //



// Pins

#define RESOLUTION_ADC 12 //12-bit ADC

#define PIN_THERMISTOR A0
//#define PIN_ECG_L 10
//#define PIN_ECG_R 11
#define PIN_ECG A3
//#define PIN_PULSEOX 15
//#define PIN_PULSEOX_RED_SWITCH 16 //if on(high voltage), red range; else, IR (???)

// we have two different LEDs connected for pulse-ox
#define PIN_PULSEOX_LED_RED 13
#define PIN_PULSEOX_LED_IR 12
#define PIN_PULSEOX_PHOTODIODE A2 //will be read to receive voltages

#define LED_BUILTIN 13 //is a red LED


// constants //


//max packet size that website can receive is 32 kB
// we only have so much RAM, will make larger array size 5k (if 4-bit ints --> 20 kiB)
//
// NEW: to match with Integrated Circuit division, will collect 250 samples / second
// --> 1000 / sizeof(int) = 250 points every TIME_CTS ms.
const unsigned MAX_LENGTH = 1000;
//const unsigned MAX_JSON_SIZE = 1000;


const int WiFi_pins[4] = { 8, 7, 4, 2 }; // as per documentation

//number of milliseconds to collect "continuous" data at a time
const time_t TIME_CTS = 1000;

//number of milliseconds to wait between check pulse ox data
//assuming max heart rate of ~200 bpm =~ 3 Hz, doubling frequency and then some to go to 8 Hz (to avoid aliasing),
//and reciprocating gives a time interval of 1/8 s ~ 125 ms
const time_t TIME_PO = 125;


const sizeType LENGTH_PO = sizeType (2 * TIME_CTS/TIME_PO); //'2' because two values needed each reading (red and IR(???))
const sizeType LENGTH_ECG = sizeType(MAX_LENGTH/sizeof(binType));

//try to poll ECG at equal time intervals to fill up the ECG buffer up to LENGTH_ECG
// by the time TIME_CTS has passed
// so this describes interval between ECG polling
const time_t TIME_ECG = time_t(LENGTH_ECG / TIME_CTS);


const int BUFFER_SIZE = JSON_OBJECT_SIZE(6) + JSON_ARRAY_SIZE(LENGTH_PO) + JSON_ARRAY_SIZE(LENGTH_ECG);



const unsigned MAX_INPUT_LENGTH = 255;

// how fast the serial connection to e.g. laptop should go
const unsigned BAUD_RATE = 9600; //bits-per-second

// unsigned int localPort = 2390;      // local port to listen on


//declare buffers/arrays to pass into functions
//char str_buf[MAX_LENGTH];





//// String Labels and Delimiters

const char DELIMITER = ',';
const String LABEL_TIME = "TIMESTAMP";
const String LABEL_ECG = "ECG";
const String LABEL_PO = "PO";
const String LABEL_TEMP = "TEMP";
const String LABEL_END_OF_PACKET = "\n";




//// For #PERFORM_CALCULATIONS formulae

const float V_IN = 3.3;
const int N_BINS = pow(2,RESOLUTION_ADC);
//const float PO_COEFFS[] = {110.99, -43.724, 7.8091, -1.364, 0.2143, -0.0247, 0.0014}; // for order 6
// for order 2 polynomial interpolation. For ratios between A_red/A_IR
const float PO_COEFFS[] = {108.29, -36.474, 2.8541}; 








#endif
