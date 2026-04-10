#include <AccelStepper.h>

// Stepper Setup, confirm pins
#define STEP_PIN 2
#define DIR_PIN 3
AccelStepper stepper(1, STEP_PIN, DIR_PIN);

// Pins
const int buttontransl = 6;
const int ledlaserPin = 7;
const int buttonrot = 9;
// Variables
unsigned long lastPress = 0;

void setup() {
  //Serial.begin(9600);
  stepper.setMaxSpeed(15310);
  stepper.setAcceleration(15000);
  pinMode(ledlaserPin, OUTPUT);
  pinMode(buttontransl, INPUT_PULLUP);
  pinMode(buttonrot, INPUT_PULLUP);
  stepper.move(64000);
  
}

void loop() {
  
  
     // 180° rotation
    if(stepper.distanceToGo() != 0) 
      stepper.run();
}