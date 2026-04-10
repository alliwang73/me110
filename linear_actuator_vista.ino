
#include <AccelStepper.h>

// Stepper Setup
#define DIR_PIN 2
#define PUL_PIN 3
AccelStepper stepper(AccelStepper::DRIVER, PUL_PIN, DIR_PIN);

// Pin assignments

const int button1Pin = 7;   // Preset 1
const int button2Pin = 8;   // Preset 2
const int button3Pin = 9;  // Preset 3
const int buttonHomePin = 10; //Say go home
const int homeSwitchPin = 11; // Home (limit) switch

 
// Ball screw & driver parameters
const long STEPS_PER_REV = 200;  // 1.8° step angle
const long MICROSTEPS = 8;       // Set on DM542T
const long MM_PER_REV = 10;       // Ball screw pitch (mm per revolution)

// Preset positions (in mm)
const float POS1_MM = -317.5;//Inches to mm scaled by 10 to show on video 
const float POS2_MM = -321.3;// Home and then tranlate to position slightly ahead of position one
const float POS3_MM = -356.85;// Not homing between 2 and 3

// Calculate steps for each position
long mmToSteps(float mm) {
  return (mm * STEPS_PER_REV * MICROSTEPS) / MM_PER_REV;
}
// State
long currentPosSteps = 0;
unsigned long lastPress = 0;
bool buttonPrev1 = HIGH;
bool buttonPrev2 = HIGH;
bool buttonPrev3 = HIGH;    
bool buttonPrevH = HIGH;
//Setup loop runs once at startup
void setup() {
  stepper.setMaxSpeed(2000);      // Adjust as needed
  stepper.setAcceleration(500);  // Adjust as needed
  
  pinMode(button1Pin, INPUT_PULLUP);//Input pullup engages 25kohm resistor. No need to add resistor
  
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(button3Pin, INPUT_PULLUP);
  pinMode(buttonHomePin, INPUT_PULLUP);
  pinMode(homeSwitchPin, INPUT_PULLUP);

  Serial.begin(9600);
  homeActuator();  
}

// Move actuator to a position (in mm)
void moveToPosition(long posMM) {
  long targetSteps = mmToSteps(posMM);
  stepper.moveTo(targetSteps);
  currentPosSteps = targetSteps;
}

// Home actuator using limit switch

void homeActuator() {
  Serial.println("Homing...");
  stepper.setMaxSpeed(1000); // Slow speed for homing
  stepper.setAcceleration(1000);

  // Retract until switch triggers
  stepper.moveTo(100000); // Move far enough to guarantee hitting switch
  while (digitalRead(homeSwitchPin) == HIGH) {
    stepper.run();//1 step each loop iteration, doesnt go to next step till loop fails
  }
  stepper.stop(); // Stop movement
  
  // Optionally, back off a few mm to release the switch
  stepper.moveTo(stepper.currentPosition() + mmToSteps(-10));
  while (stepper.distanceToGo() != 0) {
    stepper.run();
  }
  
  // Set current position as zero
  stepper.setCurrentPosition(0);
  currentPosSteps = 0;

  // Restore normal speed
  stepper.setMaxSpeed(2000);
  stepper.setAcceleration(1000);

  Serial.println("Homed. Position reset to zero.");//latest is at bottom? but definitiely at new line
}
void loop() {
  stepper.run();
  // Non-blocking stepper movement
  bool buttonCurr1 = digitalRead(button1Pin);
  if (buttonPrev1 == HIGH && buttonCurr1 == LOW && millis() - lastPress > 200) {
    moveToPosition(POS1_MM);
    lastPress = millis();
  }
  buttonPrev1 = buttonCurr1;
  bool buttonCurr2 = digitalRead(button2Pin);
  if (buttonPrev2 == HIGH && buttonCurr2 == LOW && millis() - lastPress > 200) {
    moveToPosition(POS2_MM);
    lastPress = millis();
    
  }
  buttonPrev2 = buttonCurr2;
  bool buttonCurr3 = digitalRead(button3Pin);
  if (buttonPrev3 == HIGH && buttonCurr3 == LOW && millis() - lastPress > 200) {
    moveToPosition(POS3_MM);
    lastPress = millis();
  }
  buttonPrev3 = buttonCurr3;
  bool buttonCurrH = digitalRead(buttonHomePin);
  if (buttonPrevH == HIGH && buttonCurrH == LOW && millis() - lastPress > 200){
    homeActuator();
    lastPress = millis();
  }
  buttonPrevH = buttonCurrH;//Updates button press so it doesnt think buttonprev is unpressed
}


