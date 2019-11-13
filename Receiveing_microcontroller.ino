/*
 * this will be loaded onto my arduino uno that will have the nRF24L01, thermistors, and photoresistors hooked up to it
 */

//these are the required libraries used by the wireless module and
//the adafruit 2.8" tft touchscreen
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24_config.h>
#include <RF24.h>
//#include <Adafruit_GFX.h>
#include <Wire.h>
//#include <Adafruit_ILI9341.h>
//#include <Adafruit_STMPE610.h>
#include <dht.h>
dht DHT;
#define DHT11_PIN 5

//initialize the parts for the nRF24L01
RF24 radio(7, 8); // CE, CSN          9,8 for uno, 7,8 for nano
const byte address[5] = "0001";

int ThermistorPin1 = A1;
int Vo1, Vo2;
const int pResistor = A0; // Photoresistor at Arduino analog pin A0
int value;          // Store value from photoresistor (0-1023)
float R = 100000; // constant
float logR1, R1, R2, logR2;
float T1,T2, average, difference ;
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;

//needed parts for the thermistor readouts

//const int ThermistorIn = 0;        //two thermistors facing outside                                               (slot 1 on nano, slot 0 on uno)
int V1;                            //the readout from the thermistor pins
//float Resistor = 9500;            //resistor value for all thermistors will be the same
//float R1, logR1;               //initiating the varaibles that will be used throughout the program
//float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;     //constants for the conversion


int i = 0; 
int k = 0; 
float temp [20];
float photo [20];
float avg_temp = 75.00; 
float avg_light = 7.00; 

float temp_angle = 15.00; float light_angle = 15.00; 
float temp_height = 0.00; float light_height = 0.00;

float light_error; 
float temp_error;

//Needed parts for the photoresistor readouts
//const int Photo1 = 1;       //two photoresistors taking the last two analog slots of the Arduino nino nino Uno      (slot 0 for nano, slot 1 for uno)
float light_intensity; 

float temperature;

float motorState1 = 0.00; 
float motorState2 = 0.00;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.print("starting smart blinds sensory module...\n");
}

void loop() {

  V1 = analogRead(ThermistorPin1);
  R1 = R * (1023.0 / (float)V1 - 1.0); 
  logR1 = log(R1);                       
  temperature = (1.0 / (c1 + c2*logR1 + c3*logR1*logR1*logR1));
  temperature = temperature - 273.15;                                                     //temp in celsius nino nino
  temperature = (temperature * 9.0)/ 5.0 + 32.0;                                          //temp in farenheit nino nino

  light_intensity = analogRead(pResistor);
  light_intensity = (light_intensity / 1024.00);
  light_intensity = (100.00 - (light_intensity * 100.00));

  if(temperature <= 0.00){
    temp_error = 1.00;
    temperature = 0;
    Serial.print("error...no thermistor detected \n");  
  }
  if(light_intensity <= 0.19){
    light_error = 1.00;
    light_intensity = 0;
    Serial.print("error...no photoresistor detected \n");  
  }
  if(temperature >= 0.01){
    temp_error = 0.00;  
  }
  if(light_intensity >= 0.2){
    light_error = 0.00;  
  }

  if(i < 19){
    temp[i] = temperature;
    photo[i] = light_intensity;
    i++; 
  }
  if(i >= 19){
    temp[i] = temperature;
    photo[i] = light_intensity;
    i = 0; 
    k = 0;
    float sum_temp = 0.00; float sum_photo = 0.00;
    while(k <= 19){
      sum_temp = (sum_temp + temp[k]);
      sum_photo = (sum_photo + photo[k]);
      k++;
    }
    avg_temp = (sum_temp / 20.00);
    avg_light = (sum_photo / 20.00);
    k = 0;
    Serial.print("average temperature is "); Serial.print(avg_temp); Serial.print("\n");
    Serial.print("average light is "); Serial.print(avg_light); Serial.print("\n");

    
    //now we list our possible conditions for different blind height and angle values 
    //slightly warm
    if(avg_temp >= 75.00 && avg_temp < 85.00){
      temp_angle = (125.00+(avg_temp - 75.00)); //slats tilted upwards
      temp_height = 0.00; //fully lowered
    }
      
    //very warm
    if(avg_temp >= 85.00){
      temp_height = 0.00; //fully closed
      temp_angle = (165.00); //slats tilted upwards
    }
      
    //slightly cold
    if(avg_temp < 75.00 && avg_temp > 65.00){
      temp_angle = (55.00-(75.00 - avg_temp)); //slats tilted downwards
      temp_height = 0.00; //fully closed
    }
      
    //very cold
    if(avg_temp <= 65.00){
      temp_angle = (15.00); //slats tilted downwards
      temp_height = 0.00; //fully closed
    }

      
    //dim
    if(avg_light > 7.00 && avg_light < 17.00){
      light_angle = (55.00 - (17.00 - avg_light)); //slats tilted down
      light_height = 0.00; //fully lowered
    }
      
    //dark
    if(avg_light <= 7.00){
      light_height = 100.00; //fully raised
      light_angle = 90.00; //slats flat
    }
      
    //light
    if(avg_light < 30.00 && avg_light >= 17.00){
      light_angle = (125.00 - (avg_light - 17.00)); //slats tilted downwards
      light_height = 0.00; //fully closed
    }
      
    //bright
    if(avg_light >= 30.00){
      light_angle = (125.00); //slats tilted downwards
      light_height = 0.00; //fully closed
    }
    Serial.print("slat angle for photo mode: "); Serial.print(light_angle); Serial.print("\n");
    Serial.print("blind height for phot mode: "); Serial.print(light_height); Serial.print("\n");
    Serial.print("slat angle for temp mode: "); Serial.print(temp_angle); Serial.print("\n");
    Serial.print("blind height for temp mode: "); Serial.print(temp_height); Serial.print("\n");
      
   }

   radio.stopListening();
   float  UNO_Data[] = { temperature , light_intensity , temp_angle , temp_height , light_angle , light_height , temp_error , light_error };
   radio.write(&UNO_Data, sizeof(UNO_Data));
  
   Serial.print("transmitted data: \n"); 
   Serial.print(UNO_Data[0]); Serial.print(" "); Serial.print(UNO_Data[1]); //Serial.print(" ");
   //Serial.print(UNO_Data[2]); Serial.print(" "); Serial.print(UNO_Data[3]); Serial.print(" ");
   //Serial.print(UNO_Data[4]); Serial.print(" "); Serial.print(UNO_Data[5]); Serial.print(" ");
   //Serial.print(UNO_Data[6]); Serial.print(" "); Serial.print(UNO_Data[7]); Serial.print(" ");
   Serial.print("\n");
  
   delay(2000);
}
