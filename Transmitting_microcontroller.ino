//#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>



int ThermistorPin1 = A2;
RF24 radio(7, 8); // CE, CSN
const byte address[5] = "0001";
int Vo1;
 float pResistor_2 = A3; // Photoresistor at Arduino analog pin A0
float light_intensity;          // Store light_intensity from pfloathotoresistor (0-1023)
float R = 100000; // constant
float logR1, R1, logR2;
float temperature;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;



void setup() {
 
Serial.begin(9600);

 radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening();

}

void loop() {

// Thermistor 1
 Vo1 = analogRead(ThermistorPin1);
  R1= R * (1023.0 / (float)Vo1 - 1.0);
  logR1 = log(R1);
  temperature = (1.0 / (c1 + c2*logR1 + c3*logR1*logR1*logR1));
  temperature = temperature - 273.15;
  temperature = (temperature * 9.0)/ 5.0 + 32.0; 

  Serial.print("Temperature1: "); 
  Serial.print(temperature);
  Serial.println(" F"); 
  delay (500);
  


  // Photo resistor
 light_intensity = analogRead(pResistor_2);
  Serial.print("Light intensity: "); 
  Serial.print(light_intensity);
  delay (500);

  //transmit data

   float UNO_Data[2] = {temperature, light_intensity};
  radio.write(&UNO_Data, sizeof(UNO_Data));

  //radio.printDetails();

  
}
