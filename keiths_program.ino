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
//#include <Adafruit_GFX.h>
#include <Wire.h>
//#include <Adafruit_ILI9341.h>
//#include <Adafruit_STMPE610.h>

//initialize the parts for the nRF24L01
RF24 radio(9, 10); // CE, CSN                                          depends on the pinout of the nano
const byte ESP_address[6] = "10011";              // ESP address for receiving state mode
const byte Sensor_Address[6] = "00001";           // Sensor address for receiving temp and light values

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
int a = 0;

// Initial angle is kind of confusing. Higher angle moves toward the switch. The switch is at 90, but it should be double the initial movement.. Was I wrong? No, I merely flipped every angle. Should some angles be flipped?
float HeightStateP[] = { 0 , 0 };    // 0 means fully lowered
float AngleStateP[] = { 0 , 0 };    // 20 degree slat angle starting out  (I'd say 0 if you want it perpendicular)
float HeightStateT[] = { 0 , 0 };    // 0 means fully lowered
float AngleStateT[] = { 0 , 0 };    // 20 degree slat angle starting out

float AngleDiff;
float HeightDiff;

float StateSelect; float Manual_HeightState;
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
#define LimitSwitchTop 2                                                          // switch at the top of blinds 
#define LimitSwitchBottom 1                                                       // switch at the bottom of blinds

#define LIFTDELAY 400 
#define ROTDELAY 2000

#define MULT 1.038


/*  Peter's directions:
    For lifting, just do it trial and error honestly, it's easy to switch and hard to specify in text. 
    For rotating, LOW is toward switch. HIGH is outward.
    Steps: For x degrees, rotate 16*x*200/360 steps. 
    Where I changed the step counts, I've stuck 16* on it. 
    Where I changed rotating direction, I've commented with b.
    I also changed delays to LIFTDELAY and ROTDELAY and increased them, and I changed the switch directions and values.
*/

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


// Resets the blinds. End up lowered all the way and rotated to switch.
// Proposed changes: if the switch starts pressed, rotate a certain amount to unpress the switch first
void startup() {
  //blinds boot up by hitting the limit switches for bottom of blinds and the slat angle to begin in the same orientation each time
  Serial.println("Going to bottom");
  while (analogRead(LimitSwitchBottom) >= 800){
    digitalWrite(dirPin_1,HIGH);
    digitalWrite(dirPin_2,HIGH);
    digitalWrite(stepPin_1,HIGH);
    digitalWrite(stepPin_2,HIGH); 
    delayMicroseconds(LIFTDELAY); 
    digitalWrite(stepPin_1,LOW); 
    digitalWrite(stepPin_2,LOW);
    delayMicroseconds(LIFTDELAY);                                                            // delay can be reduced or removed to improve performance
  }
  Serial.println("Rotating to beginning");
  while (analogRead(LimitSwitchTop) >= 800){
    digitalWrite(dirPin_3,LOW);     // b
    digitalWrite(stepPin_3,HIGH);
    delayMicroseconds(ROTDELAY); 
    digitalWrite(stepPin_3,LOW); 
    delayMicroseconds(ROTDELAY);                                                                 // delay can be reduced or removed to improve performance
  }  

  // Initial angle is 60. Angle to step multiplier is 16*1.11
  for(int x = 0; x < 16*MULT*60; x++) {   
    digitalWrite(dirPin_3,HIGH);    // b
    digitalWrite(stepPin_3,HIGH); 
    delayMicroseconds(ROTDELAY); 
    digitalWrite(stepPin_3,LOW); 
    delayMicroseconds(ROTDELAY);
  }
}


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

  pinMode(8,OUTPUT); // output LED for debug
  digitalWrite(8,HIGH);
  delay(100);
  digitalWrite(8,LOW);
  //--------------------------------------------------------  

  for(int x = 0; x < 5; x++) {   
    digitalWrite(dirPin_1,HIGH);
    digitalWrite(stepPin_1,HIGH);  
    digitalWrite(dirPin_2,HIGH);
    digitalWrite(stepPin_2,HIGH); 
    digitalWrite(dirPin_3,HIGH);
    digitalWrite(stepPin_3,HIGH); 
    delayMicroseconds(ROTDELAY); 
    digitalWrite(stepPin_1,LOW); 
    digitalWrite(stepPin_2,LOW); 
    digitalWrite(stepPin_3,LOW); 
    delayMicroseconds(ROTDELAY);
  }

  //setup code for initializing the radio
  Serial.begin(115200);
  Serial.println("Howdy");
  radio.begin();
  radio.openReadingPipe(0, Sensor_Address);                                              // this will read from radio signal from the ESP32
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  Serial.println("starting motor subsystem...\n");
}








int i=-1;  // debug, lets us change state every so often
int r = 0;
void loop() {
//  delay(32000);
  i++;
  delay(100);
  // put your main code here, to run repeatedly:

//________________________________________________________________________________
// receive the two sets of data from the App subsystem and the Sensors Subsystem  
//  radio.stopListening();
//  radio.openReadingPipe(0, Sensor_Address);
//  radio.setPALevel(RF24_PA_MIN);
//  radio.startListening();

//  if(radio.available()){ 
//    radio.read(&ESP_Data, sizeof(ESP_Data));
//
//    ManualState = ESP_Data[0];
//    TempState = ESP_Data[1];
//    PhotoState = ESP_Data[2];
//    Manual_HeightState = ESP_Data[3];
//
//    radio.stopListening();
//    radio.openReadingPipe(0, Sensor_address);                                              
//    radio.setPALevel(RF24_PA_MIN);
//    radio.startListening();
//    
    radio.startListening();

    if(radio.available()){
      radio.read(&Sensor_Data, sizeof(Sensor_Data));
      
      temperature = Sensor_Data[0];
      light = Sensor_Data[1];
      temp_angle = Sensor_Data[2] - 90;
      temp_height = Sensor_Data[3];
      light_angle = Sensor_Data[4] - 90;
      light_height = Sensor_Data[5];
      StateSelect = Sensor_Data[6];
      Manual_HeightState = Sensor_Data[7];
      
      Serial.println("Reveived Data: ");
      Serial.print(Sensor_Data[0]); Serial.print(" ");
      Serial.print(Sensor_Data[1]); Serial.print(" ");
      Serial.print(temp_angle); Serial.print(" ");
      Serial.print(Sensor_Data[3]); Serial.print(" ");
      Serial.print(light_angle); Serial.print(" ");
      Serial.print(Sensor_Data[5]); Serial.print(" ");
      Serial.print(Sensor_Data[6]); Serial.print(" ");
      Serial.print(Sensor_Data[7]); Serial.println(" ");
      
      //Serial.println(temperature);
      //Serial.println("available");
    } 

//  }
//__________________________________________________________________________________
//
//  if (i==0) {
//    Serial.println("i=0");
//    TempState = 1;
//    temp_angle = 60;
//  }
//
//  if (i==300) {
//    Serial.println("i=3");
//    temp_angle = 0;
//  }
//
//  if (i==600) {
//    Serial.println("i=6");
//    temp_angle = -60;
//  }
//
//  if (i==900) {
//    Serial.println("i=9");
//    temp_angle = 80;
//    temp_height = 1;
//  }
//
//  if(i==1200) {
//    Serial.println("i=12");
//    temp_height = 0;
//  }

// now to determine the angle and blind height difference 
    if(StateSelect == 1){  // should be 1
      Serial.println(" Temperature Mode Selected...");
      startup();
      // Debug: 
//      delay(1000);
//      digitalWrite(dirPin_3,HIGH);                  // b
//      numSteps = 16*round((45)*MULT); 
//      Serial.print("(never runs) negative rot numSteps: ");  // This line is odd, why is it here?
//      Serial.println(numSteps);
//      for(int x = 0; x < numSteps; x++) {
//        digitalWrite(stepPin_3,HIGH); 
//        delayMicroseconds(ROTDELAY); 
//        digitalWrite(stepPin_3,LOW); 
//        delayMicroseconds(ROTDELAY);
//      }
      a=0;
      while(StateSelect == 1){
        radio.startListening();

        if(radio.available()){
          radio.read(&Sensor_Data, sizeof(Sensor_Data));
          temperature = Sensor_Data[0];
          light = Sensor_Data[1];
          temp_angle = Sensor_Data[2] - 90;
          temp_height = Sensor_Data[3];
          light_angle = Sensor_Data[4] - 90;
          light_height = Sensor_Data[5];
          StateSelect = Sensor_Data[6];
          Manual_HeightState = Sensor_Data[7];
      
        Serial.println("Reveived Data: ");
        Serial.print(Sensor_Data[0]); Serial.print(" ");
        Serial.print(Sensor_Data[1]); Serial.print(" ");
        Serial.print(temp_angle); Serial.print(" ");
        Serial.print(Sensor_Data[3]); Serial.print(" ");
        Serial.print(light_angle); Serial.print(" ");
        Serial.print(Sensor_Data[5]); Serial.print(" ");
        Serial.print(Sensor_Data[6]); Serial.print(" ");
        Serial.print(Sensor_Data[7]); Serial.println(" ");
        }

        
      t++;
      if(t >= 1){
        t=0;
        //Serial.print("x=");  // debug
        //Serial.println(x);
        if(a == 0){   
          AngleStateT[1] = temp_angle;
          HeightStateT[1] = temp_height;
          a=1;
          AngleDiff = AngleStateT[1] - AngleStateT[0];
          HeightDiff = HeightStateT[1] - HeightStateT[0];
//          Serial.print("Approaching angle and height: ");
//          Serial.print(temp_angle);
//          Serial.print(" ");
//          Serial.print(temp_height);
//          Serial.print(" with changes: ");
//          Serial.print(AngleDiff);
//          Serial.print(" ");
//          Serial.println(HeightDiff);
          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,HIGH);                  // b
            numSteps = 16*round((-AngleDiff)*MULT); 
            Serial.print("(never runs) negative rot numSteps: ");  // This line is odd, why is it here?
            Serial.println(numSteps);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,LOW);                   // b
            numSteps = 16*round((AngleDiff)*MULT);   
            Serial.print("positive rot numSteps: ");
            Serial.println(numSteps);            
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,LOW);
            Serial.println("(never runs) going up");
            while(analogRead(LimitSwitchTop) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(LIFTDELAY);
            }
          }
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,HIGH);
            Serial.println("going down");
            while(analogRead(LimitSwitchBottom) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(LIFTDELAY);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
        
          //--------------------------------------------------------
        }
        else if(a == 1){
          AngleStateT[0] = temp_angle;
          HeightStateT[0] = temp_height;
          a=0;
          AngleDiff = AngleStateT[0] - AngleStateT[1];
          HeightDiff = HeightStateT[0] - HeightStateT[1];

          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,HIGH);                // b
            numSteps = 16*round((-AngleDiff)*MULT);
            Serial.print("negative rot numSteps: ");
            Serial.println(numSteps);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,LOW);                 // b
            numSteps = 16*round((AngleDiff)*MULT);
            Serial.print("(never runs) positive rot numSteps: ");
            Serial.println(numSteps);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,LOW);
            Serial.println("going up");
            while(analogRead(LimitSwitchTop) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(LIFTDELAY);
            }
          }
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,HIGH);
             Serial.println("(never runs) going down");
            while(analogRead(LimitSwitchBottom) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(LIFTDELAY);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
          //--------------------------------------------------------
        }
      }
       
      }
    }

    else if(StateSelect == 2){  // should be 2
            Serial.println(" Light Mode Selected...");

      startup();
      y=0;
      while(StateSelect == 2){
        
         radio.startListening();
        if(radio.available()){
          radio.read(&Sensor_Data, sizeof(Sensor_Data));
          temperature = Sensor_Data[0];
          light = Sensor_Data[1];
          temp_angle = Sensor_Data[2] - 90;
          temp_height = Sensor_Data[3];
          light_angle = Sensor_Data[4] - 90;
          light_height = Sensor_Data[5];
          StateSelect = Sensor_Data[6];
          Manual_HeightState = Sensor_Data[7];

          Serial.println("Reveived Data: ");
        Serial.print(Sensor_Data[0]); Serial.print(" ");
        Serial.print(Sensor_Data[1]); Serial.print(" ");
        Serial.print(temp_angle); Serial.print(" ");
        Serial.print(Sensor_Data[3]); Serial.print(" ");
        Serial.print(light_angle); Serial.print(" ");
        Serial.print(Sensor_Data[5]); Serial.print(" ");
        Serial.print(Sensor_Data[6]); Serial.print(" ");
        Serial.print(Sensor_Data[7]); Serial.println(" ");
        }
        p++;
      if(p >= 1){
        p=0;
        if(y == 0){
          AngleStateP[1] = light_angle;
          HeightStateP[1] = light_height;
          y=1;
          AngleDiff = AngleStateP[1] - AngleStateP[0];
          HeightDiff = HeightStateP[1] - HeightStateP[0];

          //--------------------------------------------------------
          if(AngleDiff < 0.0){
            digitalWrite(dirPin_3,HIGH);                // b
            numSteps = 16*round((-AngleDiff)*MULT);
            Serial.print("negative rot numSteps: ");
            Serial.println(numSteps);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,LOW);                 // b
            numSteps = 16*round((AngleDiff)*MULT);
            Serial.print("positive rot numSteps: ");
            Serial.println(numSteps);            
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,LOW);
            while(analogRead(LimitSwitchTop) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(LIFTDELAY);
            }
          }
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,HIGH);
            while(analogRead(LimitSwitchBottom) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(LIFTDELAY);
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
            digitalWrite(dirPin_3,HIGH);                  // b
            numSteps = 16*round((-AngleDiff)*MULT);
            Serial.print("negative rot numSteps: ");
            Serial.println(numSteps);            
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          }
          else if(AngleDiff > 0.0){
            digitalWrite(dirPin_3,LOW);                   // b
            numSteps = 16*round((AngleDiff)*MULT);
            Serial.print("positive rot numSteps: ");
            Serial.println(numSteps);
            for(int x = 0; x < numSteps; x++) {
              digitalWrite(stepPin_3,HIGH); 
              delayMicroseconds(ROTDELAY); 
              digitalWrite(stepPin_3,LOW); 
              delayMicroseconds(ROTDELAY);
            }
          
          }
          else if(AngleDiff = 0.0){
            //do nothing because no angle change
          }
          if(HeightDiff > 0.0){
            digitalWrite(dirPin_1,LOW);
            digitalWrite(dirPin_2,LOW);
            while(analogRead(LimitSwitchTop) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW);
              digitalWrite(stepPin_2,LOW); 
              delayMicroseconds(LIFTDELAY);
            }
          } 
          else if(HeightDiff < 0.0){
            digitalWrite(dirPin_1,HIGH);
            digitalWrite(dirPin_2,HIGH);
            while(analogRead(LimitSwitchBottom) >= 800){
              digitalWrite(stepPin_1,HIGH);
              digitalWrite(stepPin_2,HIGH); 
              delayMicroseconds(LIFTDELAY); 
              digitalWrite(stepPin_1,LOW); 
              digitalWrite(stepPin_2,LOW);
              delayMicroseconds(LIFTDELAY);
            }
          }
          else if(HeightDiff = 0.0){
            //do nothing, aka no change
          }
          //-------------------------------------------------------------

        }
      }
      
      }
    }

    
    else if(StateSelect == 3){  // should be 3
            Serial.println(" Manual Mode Selected...");

      startup();
      a=0;
      
      while(StateSelect == 3){
         radio.startListening();
        if(radio.available()){
          radio.read(&Sensor_Data, sizeof(Sensor_Data));
          temperature = Sensor_Data[0];
          light = Sensor_Data[1];
          temp_angle = Sensor_Data[2] - 90;
          temp_height = Sensor_Data[3];
          light_angle = Sensor_Data[4] - 90;
          light_height = Sensor_Data[5];
          StateSelect = Sensor_Data[6];
          Manual_HeightState = Sensor_Data[7];

          Serial.println("Reveived Data: ");
        Serial.print(Sensor_Data[0]); Serial.print(" ");
        Serial.print(Sensor_Data[1]); Serial.print(" ");
        Serial.print(temp_angle); Serial.print(" ");
        Serial.print(Sensor_Data[3]); Serial.print(" ");
        Serial.print(light_angle); Serial.print(" ");
        Serial.print(Sensor_Data[5]); Serial.print(" ");
        Serial.print(Sensor_Data[6]); Serial.print(" ");
        Serial.print(Sensor_Data[7]); Serial.println(" ");
        }
        
      if(Manual_HeightState == 1){
      
        HeightStateP[0] = 1;    // 0 means fully lowered
        AngleStateP[0] = 0;    // 20 degree slat angle starting out
        HeightStateT[0] = 1;    // 0 means fully lowered
        AngleStateT[0] = 0;    // 20 degree slat angle starting out
        x=0; y=0; 

        digitalWrite(dirPin_1,LOW);
        digitalWrite(dirPin_2,LOW);
        while(analogRead(LimitSwitchTop) >= 800){
          digitalWrite(stepPin_1,HIGH);
          digitalWrite(stepPin_2,HIGH); 
          delayMicroseconds(LIFTDELAY); 
          digitalWrite(stepPin_1,LOW);
          digitalWrite(stepPin_2,LOW); 
          delayMicroseconds(LIFTDELAY);
        }      
      }
    
      else if(Manual_HeightState == 0){
      
        HeightStateP[0] = 0;    // 0 means fully lowered
        AngleStateP[0] = 20;    // 20 degree slat angle starting out
        HeightStateT[0] = 0;    // 0 means fully lowered
        AngleStateT[0] = 20;    // 20 degree slat angle starting out
        x=0; y=0;

        digitalWrite(dirPin_1,HIGH);
        digitalWrite(dirPin_2,HIGH);
        while(analogRead(LimitSwitchBottom) >= 800){
          digitalWrite(stepPin_1,HIGH);
          digitalWrite(stepPin_2,HIGH); 
          delayMicroseconds(LIFTDELAY); 
          digitalWrite(stepPin_1,LOW); 
          digitalWrite(stepPin_2,LOW);
          delayMicroseconds(LIFTDELAY);
        }
      }
        
      }  
    }
}
