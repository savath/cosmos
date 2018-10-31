//TMP36 Pin Variables

int sensorPin = 0; //the analog pin the TMP36's Vout (sense) pin is connected to
                        //the resolution is 10 mV / degree centigrade with a
                        //500 mV offset to allow for negative temperatures

/*
 * setup() - this function runs once when you turn your Arduino on
 * We initialize the serial connection with the computer
 */
void setup()
{
  Serial.begin(9600);  //Start the serial connection with the computer
                       //to view the result open the serial monitor 
}
 
void loop()                     // run over and over again
{
  // gets voltage reading from the temperature sensor at analog in 0
  int reading = analogRead(sensorPin); 
  
  // converts reading to voltage, for 3.3v arduino use 3.3
  float voltage = reading * 5.0;
  voltage /= 1024.0; 

  // Print sensor 1
  Serial.println("Temperature Sensor 1");
  
  // Print out the voltage
  Serial.print(voltage);
  Serial.println(" volts");
  
  // converts voltage reading to temperature in degree Celsius
  float temperatureC = (voltage - 0.5) * 100 ;  //converting from 10 mv per degree wit 500 mV offset
                                                //to degrees ((voltage - 500mV) times 100)

  // Prints out temperature with unit Celcius
  Serial.print(temperatureC);
  Serial.println(" degrees C");
  
  // converts Celcius to Fahrenheit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;
  
  // Prints out temperature with unit Fahrenheit
  Serial.print(temperatureF);
  Serial.println(" degrees F");
  
  // Converts Celcius to Kelvin
  float temperatureK = temperatureC + 273;
  
  
  // Prints out temperature with unit Kelvin
  Serial.print(temperatureK);
  Serial.println(" degrees K");
  
  String cosmos_value =  "{\"device_tsen_temp_000\":" 
           + String(temperatureK) 
           + " K}";

  // prints out device tsen with temperature reading in Kelvin
  Serial.println(cosmos_value);
  Serial.println();
  
  delay(1000);  //waiting a second
}
