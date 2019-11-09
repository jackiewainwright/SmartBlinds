/*
 * Idea for new feature: Both top motors use the same pins. 
 * Problem: Motors need to warm up before running. Touching the heatsinks helps? Observing voltages with multimeter helps?
 * 
 */

#include "A4988.h"

#define MOTOR_STEPS 200
#define DIR1 5
#define STEP1 7
#define DIR2 3
#define STEP2 8
#define MS1 6
#define MS2 6  
#define MS3 6
// These two may not be connected. 
#define EN1 3
#define EN2 2

// Here we defined two switches for three functions. (Honestly, I think that'll be a problem. What if SWITCHT is pressed but we need to use SWITCHR?)
// Also, I haven't tried reading analog inputs on the Nano. I think this is how it's done?
#define SWITCHB 1
#define SWITCHT 2
#define SWITCHR 2
// This monitors 12V supply. It's either 0 or 3.22V. 
#define POWERMONITOR 5

// Stepper 1: Both raising motors. Both are always active (except for a physical glitch) at the same time. 
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
  stepper1.begin(10,16);
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
  delay(1000);
}

void loop() {
  Serial.println("Howdy");
  delay(100);
//  if (analogRead(SWITCHT)) < 128 { }
  stepper1.rotate(360); 
  stepper2.rotate(60);  
  stepper1.rotate(-360); 
  stepper2.rotate(-60);
  delay(1000);
  
}
