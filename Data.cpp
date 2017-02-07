#include <ArduinoJson.h> //for packaging data

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
//  vals_ecg[len_ecg] = analogRead(PIN_ECG_R) - analogRead(PIN_ECG_L);
    #ifndef PERFORM_CALCULATIONS
    
    vals_ecg[len_ecg] = analogRead(PIN_ECG);
    
    #else
    // no processing done currently
    vals_ecg[len_ecg] = analogRead(PIN_ECG);
    #endif
  
  #endif
  len_ecg+=1;
  return true;
}



// Polls for voltage readings corresponding to absorbances for red and IR
// will add in the order red then IR
// Will leave pins off at the end
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

  // poll for red
  digitalWrite(PIN_PULSEOX_LED_RED, HIGH); //only red LED is on
  //delay(1); //necessary?
  #ifndef PERFORM_CALCULATIONS
    vals_po[len_po] = analogRead(PIN_PULSEOX_PHOTODIODE);
  #else
    float v_red = analogRead(PIN_PULSEOX_PHOTODIODE);
  #endif

  // poll for IR
  digitalWrite(PIN_PULSEOX_LED_RED, LOW);
  digitalWrite(PIN_PULSEOX_LED_IR, HIGH); // now only IR LED is on
  //delay(1); //necessary?
  #ifndef PERFORM_CALCULATIONS
    vals_po[len_po + 1] = analogRead(PIN_PULSEOX_PHOTODIODE);
  #else
    float v_ir = analogRead(PIN_PULSEOX_PHOTODIODE);
  #endif
  
  // cleanup
  digitalWrite(PIN_PULSEOX_LED_IR, LOW); //turn off LEDs

  #ifdef PERFORM_CALCULATIONS
  float ratio = v_red / v_ir;
  float percentO2 = 0;
  for(int i = 0; i < 3; i++)
    percentO2 += (PO_COEFFS[i] * pow(ratio, i));

  vals_po[len_po] = percentO2;
  vals_po[len_po+1] = v_ir; //just to see how low the numbers are
//  Serial.println(String(percentO2));
  #endif
  
  len_po += 2; //update length of array
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

  #ifndef PERFORM_CALCULATIONS
    val_temp = analogRead(PIN_THERMISTOR);
  #else
    // what follows is some magic code adapted from an online source
    // by the Integrated Circuit division, in order to convert
    // voltage binning to temperature
    //
    // TODO: parse to understand...
     binType Temp;
     Temp = log(((204750000/analogRead(PIN_THERMISTOR)) - 50000));
     Temp = 1 / (0.0003709216250 + (0.0002769715465 + (-0.00000001079695539 * Temp * Temp ))* Temp );
     Temp = Temp - 273.15;              // Convert Kelvin to Celsius
  // Temp = (Temp * 9.0)/ 5.0 + 32.0; // Celsius to Fahrenheit - comment out this line if you need Celsius
     val_temp = Temp;
  #endif
  
  // convert to temp? Would need float instead of short
  return;
}


// packages the Data class into a string, timestamped at the front with t,
// to be sent over SSL
// t = timestamp for the packet
String Data::package_data(time_t t){

  // using ArduinoJSON
  // with dynamicJSONBuffer suggested to have ~5000 bytes
  // resource: https://github.com/bblanchon/ArduinoJson/wiki/Encoding%20JSON

  //
  // Step 1: Reserve memory space
  //
  StaticJsonBuffer<BUFFER_SIZE> jsonBuffer; //suggests it starts with 5000 bytes from the heap
  
  //
  // Step 2: Build object tree in memory
  //
  JsonObject& root = jsonBuffer.createObject();
  root[LABEL_TIME] = t;
  root[LABEL_TEMP] = val_temp;

  int i = 0;
  // pulse-ox
  JsonArray& po = root.createNestedArray(LABEL_PO);
  for(i = 0; i < len_po; i++)
    po.add(vals_po[i]);
    
  // ecg
  JsonArray& ecg = root.createNestedArray(LABEL_ECG);
  for(i = 0; i < len_ecg; i++)
    ecg.add(vals_ecg[i]);
  
//  return root;
  //
  // Step 3: Generate the JSON string
  //
//  root.printTo(Serial); // to be done when sending to server
  String result;
  root.printTo(result);
  return result;




  
//  String s = "";
//  int i = 0;
//
//  // "pretty" code for string formatting not trivial...
//  // (would require a bunch of extra code/function calls)
//  // so a 'dumb' way is used
//
//  s = LABEL_TIME + String(t) + DELIMITER + LABEL_ECG;
//
//  //s = label_1 + delimiter; ...
//
//  // churn together ecg data
//  for(i = 0; i < len_ecg; i++)
//  {
//    s += (String(vals_ecg[i]) + DELIMITER);
//  }
//
//  s += LABEL_PO;
//
//  for(i = 0; i < len_po; i++)
//  {
//    s += (String(vals_po[i]) + DELIMITER);
//  }
//
//  s += LABEL_TEMP + String(val_temp) + LABEL_END_OF_PACKET;
//
//  return s;
//  
}



