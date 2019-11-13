#include <printf.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 5






RF24 radio(7, 8); // CE, CSN
const byte address[5] = "0001";


int ThermistorPin1 = A1;
int Vo1, Vo2;
const int pResistor = A0; // Photoresistor at Arduino analog pin A0
int value;          // Store value from photoresistor (0-1023)
float R = 100000; // constant
float logR1, R1, R2, logR2;
float T1,T2, average, difference ;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

void setup() {
 Serial.begin(9600);

 
pinMode(pResistor, INPUT);// Set pResistor - A0 pin as an input (optional)

radio.begin();
  radio.openReadingPipe(0,address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
//DHT 
int chk = DHT.read11(DHT11_PIN);
Serial.print("\nHumidity = ");
Serial.print(DHT.humidity);
Serial.println("%");
delay (2000);



// Thermistor 1
 Vo1 = analogRead(ThermistorPin1);
  R1= R * (1023.0 / (float)Vo1 - 1.0);
  logR1 = log(R1);
  T1 = (1.0 / (c1 + c2*logR1 + c3*logR1*logR1*logR1));
  T1 = T1 - 273.15;
  T1 = (T1 * 9.0)/ 5.0 + 32.0; 

  Serial.print("Temperature 1: "); 
  Serial.print(T1);
  Serial.println(" F"); 
  



  // Photo resistor
  value = analogRead(pResistor);
   Serial.print("Light intensity:"); Serial.print(" ");
  Serial.print(value);
  


// receiver
  float UNO_Data[2]; 
   radio.read(&UNO_Data, sizeof(UNO_Data));
    Serial.print("\nTransmitted temperature of system 2: ");
     Serial.print(UNO_Data[0]);
    Serial.println(" F"); 
       radio.read(&value, sizeof(value));
Serial.print("\nTransmitted light intensity of system 2: ");
     Serial.print(UNO_Data[0]);
    delay (1000);
    

  // Determine open/close the bline

  //average = (UNO_Data[0]+T1_3)/2;
  //Serial.print("averaae: "); 
 // Serial.print(average);
 // Serial.println(" F"); 
  //Serial.println (" ");
  //delay(2000); //Small delay

// difference = (average - 70);


//



//Close the blind if it's too bright or rain outside
  //  if (value >= 700 || DHT.humidity >= 90)
    //{
    //digitalWrite(ledPin, LOW);  //Turn led off => close the blind
  //}
  
  //else if (T1<70 && difference >= 53.6) // if temperature outside below 70 and temperature inside too hot => close blind
  //{
    //digitalWrite(ledPin, LOW); //Turn led off => close the blind
  //}

 //else if (T1>70 && difference <= -53.6) // if temperature outside above 70 and temperature inside too cold => open blind
 //{
  //  digitalWrite(ledPin, HIGH); //Turn led on => open the blind
  //}
  //else
  //{ 
    //    digitalWrite(ledPin, HIGH); //Turn led on => open the blind
  //}
  
 

 
 
}
