#include <AccelStepper.h>

AccelStepper Zaxis(1,7,6);  // (Modus, StepPin, DirPin)

void setup(){
  Zaxis.setMaxSpeed(500);   // Steps pro Sekunde
  Zaxis.setAcceleration(100);
  Zaxis.setSpeed(500);
  
}

void loop(){
  Zaxis.runSpeed();
}
