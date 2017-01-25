#ifndef FLIGHT_TEST
#define FLIGHT_TEST

// packages

#include <WiFi101.h> //for WiFi
#include <WiFiUdp.h>
#include <SPI.h>

#include "typedefs.h"
#include "consts.h"




// function declarations //

void update_continuous_readings(time_t interval);

void add_pulse_ox_data(sizeType len_po_cur);

void setup_WiFi();

void update_temp(binType *val_temp);

void printWifiStatus();

void listNetworks();

void printEncryptionType(int thisType);

void printMacAddress();



/// IP Addresses

// the dns server ip
IPAddress dnServer(192, 168, 0, 1);
// the router's gateway address:
IPAddress gateway(192, 168, 0, 1);
// the subnet:
IPAddress subnet(255, 255, 255, 0);

//the IP address is dependent on your network
//
//// current ip address for my computer (testing)
//// (need to open up port as well using PortListener)
//IPAddress server(192, 168, 0, 102);
//unsigned port = 8080; 

// home network dynamically allocates IP addresses
// so laptop IP changes way too often...
// connect to google instead
// Google's IP address for ping tests is 8.8.8.8
//char server[] = "arduino.cc";
//int port = 443; //for HTTPS

//
//IPAddress server(192,168,0,102);
//int port = 777;
//
//IPAddress server(127,0,0,1);
//int port = 80;

IPAddress server(192,168,1,1);
int port = 80;







#endif
