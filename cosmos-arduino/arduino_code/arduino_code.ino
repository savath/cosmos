/*
 COSMOS Tutorial: Reading Data from Arduino
 
 Prints out test data
 
 */
void setup() { 
 //Initialize serial and wait for port to open:
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  
} 

void loop() { 
  int temp = "312.00";
  
  String cosmos_value =  "{\"device_tsen_temp_000\":" 
                + String(temp) 
                + ",\"device_telem_vdouble\":21.00}";

  Serial.println(cosmos_value);
  
  delay(1000);

} 
