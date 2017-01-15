// class Data implementation

#include "Data.h"


Data::Data(){
  forget();
}

//forget it had anything in its arrays
void Data::forget(){
  len_ecg = 0;
  len_po = 0;
}


bool Data::add_ecg_data(){
//  Serial.println("Entered add_ecg_data.");

  if(len_ecg >= LENGTH_ECG){
//    Serial.println("add_ecg_data is returning false!");
    return false;
  }

  #ifdef DUMMY_SENSORS
  vals_ecg[len_ecg] = generate_num();
  #else
  // populate ECG in between pulse_ox readings
  // store difference of V_right and V_left
  vals_ecg[len_ecg] = analogRead(PIN_ECG_R) - analogRead(PIN_ECG_L);
  #endif
  len_ecg+=1;
  return true;
}



// Polls for voltage readings corresponding to absorbances for red and IR
// will add in the order red then IR
// will have the PIN_PULSEOX_RED_SWITCH to LOW (pulseox LED set to IR (lower power))
// on function return
// updating of length is handled by caller -- len_po_cur is just used to find slots
// to place read values
//
// Returns false if there's not enough space
bool Data::add_pulse_ox_data(){

  if(len_po + 2 >= LENGTH_PO) return false;

  #ifdef DUMMY_SENSORS
  vals_po[len_po] = generate_num();
  vals_po[len_po+1] = generate_num();
  len_po += 2;
  return true;
  #endif

  
  digitalWrite(PIN_PULSEOX_RED_SWITCH, HIGH); //LED now shooting at red range
  vals_po[len_po] = analogRead(PIN_PULSEOX);
  digitalWrite(PIN_PULSEOX_RED_SWITCH, LOW); //LED now shooting at IR range
  vals_po[len_po + 1] = analogRead(PIN_PULSEOX);
  len_po += 2;
  return true;
}


// ping PIN_THERMISTOR to get new voltage binning for val_temp
// Does *not* convert to temperature
// (Temperature formula would be ???T)
void Data::update_temp() {

  #ifdef DUMMY_SENSORS
  val_temp = generate_num();
  return;
  #endif


  
  val_temp = analogRead(PIN_THERMISTOR);
  // convert to temp? Would need float instead of short
  return;
}


// packages the Data class into a string, timestamped at the front with t,
// to be sent over SSL
// t = timestamp for the packet
String Data::package_data(time_t t){
  String s = "";
  int i = 0;

  // "pretty" code for string formatting not trivial...
  // (would require a bunch of extra code/function calls)
  // so a 'dumb' way is used

  s = LABEL_TIME + String(t) + DELIMITER + LABEL_ECG;

  //s = label_1 + delimiter; ...

  // churn together ecg data
  for(i = 0; i < len_ecg; i++)
  {
    s += (String(vals_ecg[i]) + DELIMITER);
  }

  s += LABEL_PO;

  for(i = 0; i < len_po; i++)
  {
    s += (String(vals_po[i]) + DELIMITER);
  }

  s += LABEL_TEMP + String(val_temp) + LABEL_END_OF_PACKET;

  return s;
  
}



