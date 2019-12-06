/***
 * Idea for new feature: Both top motors use the same pins. 
 * Problem: Motors need to warm up before running. Touching the heatsinks helps? Observing voltages with multimeter helps?
 * 
 */

// This file was made by Laurentiu Badea. It's hosted somewhere on Github.
#include "A4988.h"

#define MOTOR_STEPS 200
#define DIR1 4
#define STEP1 3
#define DIR2 5
#define STEP2 6
#define MS1 2  // Unused, the MSx pins are all connected to VCC
#define MS2 2
#define MS3 2
// These two may not be connected. 
#define EN1 1
#define EN2 1

#define SWITCHB 1
#define SWITCHT 2
#define SWITCHR 2
// This monitors 12V supply. It's either 0 or 3.22V. 
#define POWERMONITOR 5

// Stepper+ 1: Both raising motors. Both are always active (except for a physical glitch) at the same time. 
// One motor rotates the opposite direction, which is fine.
// If these motors go too far up, they start skipping, which is ok. If they go too far down, they start raising again. 
// There's two switches, one at the bottom and one at the top. This is SWITCHB and SWITCHT (bottom and top).
// Stepper 2: Rotating motor. This goes from 0 to ~150 degrees. 
// If the motor rotates too far, idk what happens. I'm kind of scared of that. Might break something. 
// There's one switch on one end. (Current plan is to share this with the upper raising switch.)
A4988 stepper1(MOTOR_STEPS, DIR1, STEP1, EN1, MS1, MS2, MS3);
A4988 stepper2(MOTOR_STEPS, DIR2, STEP2, EN2, MS1, MS2, MS3);

void setup() {
  Serial.begin(9600);
  // (rpm, microstep)
  // Microstep is always 16
  stepper1.begin(40,16);
  stepper2.begin(3,16);  
  delay(1000);
  stepper1.enable();
  stepper2.enable();
//  stepper1.disable();
  stepper1.rotate(10);
  stepper2.rotate(5);
  delay(1000);
  stepper1.rotate(-10);
  stepper2.rotate(-5);
//  delay(1000);
//  lift(30000);
//  delay(1000);
//  lift(30000);
//  delay(2000);
//  fall(30000);
//  delay(1000);
//  fall(30000);
}

void loop() {
  Serial.println("Howdy");
  delay(1000);
//  if (analogRead(SWITCHT)) < 128 { }
//  stepper1.rotate(10);
  lift(30000);
  delay(1000);
  fall(30000);
  delay(2000);
  rotf(300);
  delay(1000);
  rotr(210);
  delay(1000);
}

void lift(int n) {
  if ((n >= 10) && (analogRead(SWITCHT) > 300)) {
    stepper1.rotate(10);
    lift(n-10);
  }
}
void fall(int n) {
  if ((n >= 10) && (analogRead(SWITCHB) > 300)) {
    stepper1.rotate(-10);
    fall(n-10);    
  }
}

void rotr(int n) {
  if ((n >= 5)) {
    stepper2.rotate(5);
    rotr(n-5);
  }
}
void rotf(int n) {
  if ((n >= 5) && (analogRead(SWITCHR) > 300)) {
    stepper2.rotate(-5);
    rotf(n-5);
  }
}
