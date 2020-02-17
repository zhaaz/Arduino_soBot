/*
 * Z-Achsen Steuerung
 * Andreas Wehner
 * 26.10.2018
 * 
 * Distanzmessung funktioniert, 
 * ZAchse Bewegen funktioniert, auf Schalter fahren auch. 
 * Arbeitsbereich ZAchse: 0 - 9000 Steps. 0 oben, 9000 unten.
 * Drehmodul: 1 Umdrehung = 200*2 Steps = 400 Schritte. 1 gon entspricht 1 Schritte
 * Aber: Getriebe 1:13,74
 * 
 * Initialisierung: Anfahren von Schalterposition um Arbeitsbereich festzulegen
 * 
 * Befehle die über die serielle Schnittstelle gesendet werden können: 
 * FXXX    -> Fokusiert den Laser auf Abstand von XXX mm)
 * M      -> Fährt auf Mittelposition
 * RXXXX  -> Relative Drehung Drehmodul (in Gon)
 * PXXXXX -> Bewegt die Z-Achse auf die entsprechende Position. Arbeitsbereich 0 - 9000
 * T      -> Bewegung auf Trigger Position
 */

#include <AccelStepper.h>

const int dirPinDreh = 2; 
const int stepPinDreh = 3;
const int sleepPinDreh = 4;

const int dirPinZ = 6;
const int stepPinZ = 7;
const int sleepPinZ = 8;

const int triggerPinZ = 9;

const int echoPinDist = 10;
const int trigPinDist = 11;

AccelStepper stepperDreh(1, stepPinDreh, dirPinDreh);   // Modus, StepPin, DirPin 
AccelStepper stepperZ(1, stepPinZ, dirPinZ);

float distance;

const int numChars = 256;
char receivedChars[numChars];

char index;
long numValue; 

bool newData = false;

int actualArc = 0; // Gon


void setup() {
  
    pinMode(sleepPinDreh, OUTPUT);
    pinMode(sleepPinZ, OUTPUT);
    pinMode(triggerPinZ, INPUT);
    pinMode(echoPinDist, INPUT);
    pinMode(trigPinDist, OUTPUT);

    Serial.begin(19200);
    
    // Drehmodul 
    stepperDreh.setMaxSpeed(3000);
    stepperDreh.setAcceleration(6000);
    stepperDreh.setEnablePin(sleepPinDreh);
    stepperDreh.disableOutputs();
    stepperDreh.setCurrentPosition(0);
    
    // Z-Achse
    stepperZ.setMaxSpeed(1200);
    stepperZ.setAcceleration(600);
    stepperZ.setSpeed(-3600);
    stepperZ.setEnablePin(sleepPinZ);
    // stepperZ.disableOutputs();

    
    
    Serial.println("Z bereit");
    
    
}

void loop() {

recvWithEndMarker();
spliReceivedMessage();

if(newData == true){
  newData = false;
  
  switch(index){
    
    case 'M':
      // Fahre auf Mittelposition
      moveZToPosition(4500);
      Serial.println("Z ok [M] - Auf Mittelposition gefahren.");
      break;
      
    case 'F':
      // Fokusiere Laser (enstspricht Fahre auf festgesetzte Höhe: 150 mm)
      moveToSpecificHight(numValue);
      Serial.println("Z ok [F] - Laser fokusiert");
      break;
      
    case 'P':
      // Fahre auf die eingegebene Position
      moveZToPosition(numValue);
      Serial.print("Z ok [P] - Auf Position ");
      Serial.print(numValue);
      Serial.println(" gefahren.");
      break;


    case 'R':
      // Move Drehmodul um Winkel (Relativ zur aktuellen Position)
      Serial.print("R");
      Serial.println(numValue);  
      stepperDreh.enableOutputs();
      stepperDreh.setCurrentPosition(0);   
      stepperDreh.runToNewPosition((int) -(numValue*13.74));
      stepperDreh.disableOutputs();
      Serial.println("ok");
      break;
      
    case 'T':
      // Bewege Z-Achse auf Trigger Position
      moveZToTrigger();                   // Fahren bis zum Trigger
      stepperZ.setCurrentPosition(-25);  // Position Setzen
      delay(500);      
      moveZToPosition(4500);             // Mittelposition
      Serial.println("Z ok [T] - Arbeitsbereich festgelegt.");
      break;
     
    default:
      // Wenn Befehl nicht bekannt.
      Serial.println("Z ERROR: Befehl nicht bekannt...");
      break;

      
  }
}

}

/*
 * /////////////////////////////////////////////////////////////////////////////////
 * //  METHODEN
 * /////////////////////////////////////////////////////////////////////////////////
 */


float distMeasurement(int numberOfMeasurements){  
      
    long sumDuration = 0;
    long duration;
    for(int i = 0; i < numberOfMeasurements; i++){
      digitalWrite(trigPinDist, LOW);
      delayMicroseconds(2);
      digitalWrite(trigPinDist, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPinDist, LOW);
      duration = pulseIn(echoPinDist, HIGH);
      sumDuration = sumDuration + duration;
      delay(200);
    }
    float meanDuration = sumDuration/numberOfMeasurements;
    float distance = meanDuration*0.034/2*10;               // Distanz in mm
    return distance;
}

void moveZToPosition(long newPosition){
  if(newPosition < 9001 && newPosition > -1){   // Nur wenn innerhalb von Arbeitsbereich
    stepperZ.enableOutputs();
    stepperZ.runToNewPosition(newPosition);
    stepperZ.run();
    stepperZ.disableOutputs();
  }else{
    Serial.println("<ERROR: Angegebene Position außerhalb des Arbeitsbereichs>");
  }
}

void moveZToTrigger(){
  stepperZ.enableOutputs();
  long currentMillis = 0;
  long lastMillis = 0;
  int triggerVal = 1;
  
  while(true){
    currentMillis = millis();
    stepperZ.runSpeed();    
    if(digitalRead(triggerPinZ) == 0){
      break;
    }
  }
  stepperZ.disableOutputs();
}

void moveToSpecificHight(int specHight){
  // SpecHight in mm
  float actualHight = distMeasurement(10);
  float deltaHight = specHight - actualHight;
  long stepsToGo = long(deltaHight)*100;    // davor 800 mit 16el steps
  long actualStepPosition = stepperZ.currentPosition();
  long newPosition = actualStepPosition - stepsToGo;
  moveZToPosition(newPosition);
  distance = distMeasurement(10); 
}

void moveDrehmodul(long newPos){
    stepperDreh.enableOutputs();
    stepperDreh.moveTo(newPos);
    stepperDreh.setSpeed(3000);
    
    while(true){
      stepperDreh.runSpeedToPosition();
      if(stepperDreh.currentPosition() == newPos)
      break;
    }
    stepperDreh.disableOutputs();
}




  

// Für serielle Kommunikation

void recvWithEndMarker(){
  static int ndx = 0;
  char endMarker = '\n';
  char rc;

  while(Serial.available() > 0 && newData == false){
    rc = Serial.read();

    if(rc != endMarker){
      receivedChars[ndx] = rc; 
      ndx++;
      if(ndx >= numChars){
        ndx = numChars - 1;
      }
    }else{
      receivedChars[ndx] = '\0';  // Terminate String
      ndx = 0;
      newData = true;
    }
  }
}

void spliReceivedMessage(){
  if(newData == true){
    index = receivedChars[0];
    numValue = atol(&receivedChars[1]);
  }
}
