#ifndef DATA_PACKET
#define DATA_PACKET


// leave uncommented to have microcontroller directly calculate
// relevant physiological data from the voltage readings
// (instead of having a separate recipient do this)
//
// For debugging only (performing this on the microcontroller
// will slow things down)
#define PERFORM_CALCULATIONS






#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif


#include "consts.h" //constants (which should probably be moved to a separate header...)
#include "typedefs.h"

// class Data implementation
class Data{

public:
  Data();
  void forget();
  bool add_ecg_data();
  bool add_pulse_ox_data();
  void update_temp();
  String package_data(time_t t);
//private:
  binType vals_ecg[LENGTH_ECG]; //ecg bin numbers
  binType vals_po[size_t(LENGTH_PO)]; // polled less often than ECG circuit
  binType val_temp; //temperature reading
  sizeType len_ecg;
  sizeType len_po;
//  sizeType len_temp; // always 1
};

#endif
