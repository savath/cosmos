/*
 COSMOS Tutorial: Reading Data from Arduino
 
 Prints out test data
 
 */ 
 //include sensor libraries
#include <OneWire.h>
#include <DallasTemperature.h>
/********************************************************************/
// Data wire is plugged into pin 2 on the Arduino 
#define ONE_WIRE_BUS 2 
/********************************************************************/
// Setup a oneWire instance to communicate with any OneWire devices  
// (not just Maxim/Dallas temperature ICs) 
OneWire oneWire(ONE_WIRE_BUS); 
/********************************************************************/
// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);
/********************************************************************/ 

void setup() { 
 //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // Start up the library 
  sensors.begin(); 
  
} 


void loop() { 
 // call sensors.requestTemperatures() to issue a global temperature 
 // request to all devices on the bus 
/********************************************************************/
 sensors.requestTemperatures(); // Send the command to get temperature readings 
/********************************************************************/
  float temp = sensors.getTempFByIndex(0);
   // You can have more than one DS18B20 on the same bus.  
   // 0 refers to the first IC on the wire 
  String cosmos_value =  "{\"device_tsen_temp_001\":" 
                + String(temp) 
                + "}";
//String cosmos_value =String(temp) ;
  Serial.println(cosmos_value);
   delay(1000); 

} 
