//________________________________________________________________________________________________________________
//  This module is for the motor arduino and is responsible for the following actions:
//          - using motor drivers to power the NEMA-17 steppers
//          - using limit switches to know the lower and upper bounds of the height of blinds
//          - using a limit switch to know the upper bound of the slat angle
//          - receive a signal from the ESP32 and the Sensory subsytems that tells the motors what to do
//________________________________________________________________________________________________________________

#include <math.h>

//include the libraries for the needed hardware components
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Wire.h>
#include <Adafruit_ILI9341.h>
#include <Adafruit_STMPE610.h>

//initialize the parts for the nRF24L01
RF24 radio(9, 10); // CE, CSN                                          depends on the pinout of the nano
const byte ESP_address[6] = "10011";              // ESP address for receiving state mode
const byte Sensor_address[6] = "01101";           // Sensor address for receiving temp and light values

//set up code for the received array and loop
//__________________________________________________________________________
float Sensor_Data[8];
float ESP_Data[4];

float temp_error; float light_error; float temperature; float light; 
float temp_angle; float temp_height; float light_angle; float light_height;

int t = 0; 
int p = 0;
int x = 0;
int y = 0;

float HeightStateP[] = { 0 , 0 };    // 0 means fully lowered
float AngleStateP[] = { 20 , 0 };    // 20 degree slat angle starting out
float HeightStateT[] = { 0 , 0 };    // 0 means fully lowered
float AngleStateT[] = { 20 , 0 };    // 20 degree slat angle starting out

float AngleDiff;
float HeightDiff;

float ManualState; float TempState; float PhotoState; float Manual_HeightState;
//__________________________________________________________________________

/*
//include for stepper drivers
#include "A4988.h"
#define MOTOR_STEPS 200
#define DIR1 8
#define STEP1 7
#define OPPDIR 9
#define DIR2 11
#define STEP2 10
#define MS1 6                                                                     //currently commented out because I want to try the other more accurate way of driving the stepper motors
#define MS2 5  
#define MS3 4  
#define EN1 3
#define EN2 2
A4988 stepper1(MOTOR_STEPS, DIR1, STEP1, EN1, MS1, MS2, MS3);
A4988 stepper2(MOTOR_STEPS, DIR2, STEP2, EN2, MS1, MS2, MS3);
*/

//stuff for limit switches
#define LimitSwitchTop 1                                                          // switch at the top of blinds 
#define LimitSwitchBottom 3                                                       // switch at the bottom of blinds






//This is how I want to drive the stepper motors
//Stepper pins 1 and 2 are for the vertical raising of the blinds
//Stepper pins 3 are for the slat angle changes 
//--------------------------------------------------------
// defines pins numbers
const int stepPin_1 = 2;          
const int dirPin_1 = 3;
const int stepPin_2 = 4; 
const int dirPin_2 = 5;
const int stepPin_3 = 6; 
const int dirPin_3 = 7;

float numSteps;
//--------------------------------------------------------


void setup() {
  // put your setup code here, to run once:
  //--------------------------------------------------------
  // Sets the two pins for each stepper as Outputs
  pinMode(stepPin_1,OUTPUT); 
  pinMode(dirPin_1,OUTPUT);
  pinMode(stepPin_2,OUTPUT); 
  pinMode(dirPin_2,OUTPUT);
  pinMode(stepPin_3,OUTPUT); 
  pinMode(dirPin_3,OUTPUT);
  //--------------------------------------------------------  


  //setup code for initializing the radio
  Serial.begin(115200);
  radio.begin();
  radio.openReadingPipe(0, ESP_address);                                              // this will read from radio signal from the ESP32
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  //blinds boot up by hitting the limit switches for bottom of blinds and the slat angle to begin in the same orientation each time
  while (analogRead(LimitSwitchBottom) <= 100){
    digitalWrite(dirPin_1,HIGH);
    digitalWrite(dirPin_2,LOW);
    digitalWrite(stepPin_1,HIGH);
    digitalWrite(stepPin_2,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin_1,LOW); 
    digitalWrite(stepPin_2,LOW);
    delayMicroseconds(500);                                                            // delay can be reduced or removed to improve performance
  }
  while (analogRead(LimitSwitchTop) <= 100){
    digitalWrite(dirPin_3,HIGH);
    digitalWrite(stepPin_3,HIGH);
    delayMicroseconds(500); 
    digitalWrite(stepPin_3,LOW); 
    delayMicroseconds(500);                                                                 // delay can be reduced or removed to improve performance
  }

  //this should get the slats to about perpendicular
  for(int x = 0; x < 33; x++) {
    digitalWrite(dirPin_3,LOW);
    digitalWrite(stepPin_3,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin_3,LOW); 
    delayMicroseconds(500);
  }

  Serial.println("starting motor subsystem...\n");
}










void loop() {
  // put your main code here, to run repeatedly:

//________________________________________________________________________________
// receive the two sets of data from the App subsystem and the Sensors Subsystem  
  radio.stopListening();
  radio.openReadingPipe(0, ESP_address);                                              
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  if(radio.available()){ 
    radio.read(&ESP_Data, sizeof(ESP_Data));

    ManualState = ESP_Data[0];
    TempState = ESP_Data[1];
    PhotoState = ESP_Data[2];
    Manual_HeightState = ESP_Data[3];

    radio.stopListening();
    radio.openReadingPipe(0, Sensor_address);                                              
    radio.setPALevel(RF24_PA_MIN);
    radio.startListening();
    
    if(radio.available()){
      radio.read(&ESP_Data, sizeof(ESP_Data));
      
      temperature = Sensor_Data[0];
      light = Sensor_Data[1];
      temp_angle = Sensor_Data[2];
      temp_height = Sensor_Data[3];
      light_angle = Sensor_Data[4];
      light_height = Sensor_Data[5];
      temp_error = Sensor_Data[6];
      light_error = Sensor_Data[7]; 
    }  
  }
//__________________________________________________________________________________



// now to determine the angle and blind height difference 
    if(TempState == 1){
      t++;
      if(t >= 59){
        t=0;
        if(x == 0){
          AngleStateT[1] = temp_angle;
          HeightStateT[1] = temp_height;
          x=1;
          AngleDiff = AngleStateT[1] - AngleStateT[0];
          HeightDiff = HeightStateT[1] - HeightStateT[0];
          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,LOW);
            numSteps = round(((0-1)*AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,HIGH);
            numSteps = round((AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,HIGH);
            while(analogRead(LimitSwitchTop) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,LOW);
            while(analogRead(LimitSwitchBottom) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
        
          //--------------------------------------------------------
        }
        else if(x == 1){
          AngleStateT[0] = temp_angle;
          HeightStateT[0] = temp_height;
          x=0;
          AngleDiff = AngleStateT[0] - AngleStateT[1];
          HeightDiff = HeightStateT[0] - HeightStateT[1];

          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,LOW);
            numSteps = round(((0-1)*AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,HIGH);
            numSteps = round((AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,HIGH);
            while(analogRead(LimitSwitchTop) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,LOW);
            while(analogRead(LimitSwitchBottom) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
          //--------------------------------------------------------
        }
      }
    }

    else if(PhotoState == 1){
      p++;
      if(p >= 59){
        p=0;
        if(y == 0){
          AngleStateP[1] = light_angle;
          HeightStateP[1] = light_height;
          y=1;
          AngleDiff = AngleStateP[1] - AngleStateP[0];
          HeightDiff = HeightStateP[1] - HeightStateP[0];

          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,LOW);
            numSteps = round(((0-1)*AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,HIGH);
            numSteps = round((AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,HIGH);
            while(analogRead(LimitSwitchTop) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,LOW);
            while(analogRead(LimitSwitchBottom) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
          //-------------------------------------------------------------

        
        }
        else if(y == 1){
          AngleStateP[0] = light_angle;
          HeightStateP[0] = light_height;
          y=0;
          AngleDiff = AngleStateP[0] - AngleStateP[1];
          HeightDiff = HeightStateP[0] - HeightStateP[1];

          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,LOW);
            numSteps = round(((0-1)*AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,HIGH);
            numSteps = round((AngleDiff)/1.9);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(500);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,HIGH);
            while(analogRead(LimitSwitchTop) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(500);
            }
          } 
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,LOW);
            while(analogRead(LimitSwitchBottom) <= 100){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(500); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(500);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
          //-------------------------------------------------------------

        }
      }
    }
  
    else if(ManualState == 1){
      if(Manual_HeightState == 1){
      
        HeightStateP[0] = 1;    // 0 means fully lowered
        AngleStateP[0] = 0;    // 20 degree slat angle starting out
        HeightStateT[0] = 1;    // 0 means fully lowered
        AngleStateT[0] = 0;    // 20 degree slat angle starting out
        x=0; y=0; 

        digitalWrite(dirPin_1,LOW);
        digitalWrite(dirPin_2,HIGH);
        while(analogRead(LimitSwitchTop) <= 100){
          digitalWrite(stepPin_1,HIGH);
          digitalWrite(stepPin_2,HIGH); 
          delayMicroseconds(500); 
          digitalWrite(stepPin_1,LOW);
          digitalWrite(stepPin_2,LOW); 
          delayMicroseconds(500);
        }      
      }
    
      else if(Manual_HeightState == 0){
      
        HeightStateP[0] = 0;    // 0 means fully lowered
        AngleStateP[0] = 20;    // 20 degree slat angle starting out
        HeightStateT[0] = 0;    // 0 means fully lowered
        AngleStateT[0] = 20;    // 20 degree slat angle starting out
        x=0; y=0;

        digitalWrite(dirPin_1,HIGH);
        digitalWrite(dirPin_2,LOW);
        while(analogRead(LimitSwitchBottom) <= 100){
          digitalWrite(stepPin_1,HIGH);
          digitalWrite(stepPin_2,HIGH); 
          delayMicroseconds(500); 
          digitalWrite(stepPin_1,LOW); 
          digitalWrite(stepPin_2,LOW);
          delayMicroseconds(500);
        }
      }  
    }
    delay(1000);
}
