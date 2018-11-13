/*
 * Z-Achsen Steuerung
 * Andreas Wehner
 * 26.10.2018
 * 
 * Distanzmessung funktioniert, 
 * ZAchse Bewegen funktioniert, auf Schalter fahren auch. 800 Steps entsprechen ca 1 mm.
 * Arbeitsbereich ZAchse: 0 - 72000 Steps. 0 oben, 72000 unten.
 * Drehmodul: 1 Umdrehung = 200*16 Steps = 3200 Schritte. 1 gon entspricht 8 Schritte
 * 
 * Initialisierung: Anfahren von Schalterposition um Arbeitsbereich festzulegen
 * 
 * Befehle die über die serielle Schnittstelle gesendet werden können: 
 * F      -> Fokusiert den Laser
 * M      -> Fährt auf Mittelposition
 * DXXXX  -> Stellt Drehmodul auf den eingegebenen Winkel ein (Gon, 0-399, sonst umrechnung
 * PXXXXX -> Bewegt die Z-Achse auf die entsprechende Position. Arbeitsbereich 0 - 72000
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
    stepperDreh.setMaxSpeed(600);
    stepperDreh.setAcceleration(1200);
    stepperDreh.setEnablePin(sleepPinDreh);
    stepperDreh.disableOutputs();
    stepperDreh.setCurrentPosition(0);
    
    // Z-Achse
    stepperZ.setMaxSpeed(3600);
    stepperZ.setAcceleration(1800);
    stepperZ.setSpeed(-3600);
    stepperZ.setEnablePin(sleepPinZ);
    stepperZ.disableOutputs();

    
    
    Serial.println("<Arduini Z-Achse initialisiert...>");
    
    
}

void loop() {

recvWithEndMarker();
spliReceivedMessage();

if(newData == true){
  newData = false;
  
  switch(index){
    
    case 'M':
      // Fahre auf Mittelposition
      Serial.println("<Fahre auf Mittelposition...>");
      moveZToPosition(36000);
      Serial.println("<Auf Mittelposition gesetzt>");
      break;
      
    case 'F':
      // Fokusiere Laser (enstspricht Fahre auf festgesetzte Höhe: 150 mm)
      Serial.println("<Fokusiere...>");
      moveToSpecificHight(150);
      Serial.print("<Laser fokusiert, Distanz zum Boden: ");
      Serial.print(distance);
      Serial.println(" mm>");
      break;
      
    case 'P':
      // Fahre auf die eingegebene Position

      moveZToPosition(numValue);
      Serial.print("<Auf Position ");
      Serial.print(numValue);
      Serial.println(" gesetzt>");
      break;

    case 'D':
      // Bewege Drehmodul auf die entsprechende Winkeleinstellung
      Serial.println("<Richte Drehmodul aus...>");
      drehmodulZuWinkel(numValue);
      Serial.print("<Drehmodul ausgerichtet auf ");
      Serial.print(numValue);
      Serial.println(" gon>");
      break;

    case 'T':
      // Bewege Z-Achse auf Trigger Position
      moveZToTrigger();                   // Fahren bis zum Trigger
      stepperZ.setCurrentPosition(-100);  // Position Setzen
      delay(500);
      
    moveZToPosition(36000);             // Mittelposition
      break;
     
    default:
      // Wenn Befehl nicht bekannt.
      Serial.println("<ERROR: Befehl nicht bekannt...>");
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
  if(newPosition < 72001 && newPosition > -1){   // Nur wenn innerhalb von Arbeitsbereich
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
  long stepsToGo = long(deltaHight)*800;
  long actualStepPosition = stepperZ.currentPosition();
  long newPosition = actualStepPosition - stepsToGo;
  moveZToPosition(newPosition);
  distance = distMeasurement(10); 
}

void moveDrehmodul(long newPos){
    stepperDreh.enableOutputs();
    stepperDreh.moveTo(newPos);
    stepperDreh.setSpeed(600);
    
    while(true){
      stepperDreh.runSpeedToPosition();
      if(stepperDreh.currentPosition() == newPos)
      break;
    }
    stepperDreh.disableOutputs();
}

void drehmodulZuWinkel(int newArc){         // Drehmodul auf Winkel bewegen (ganze Winkel zwischen 0 und 399 gon)
  if(newArc > 400){
    newArc = newArc - 400;
  }else if(newArc < 0){
    newArc = newArc + 400;
  }
  
  int diff = newArc - actualArc;
  
  if(diff <= 200){
    newArc = newArc;
  }else{
    newArc = newArc - 400;
  }
  
  int winkelSteps = newArc*8;   // Entspricht 1 gon
  moveDrehmodul(winkelSteps);
  actualArc = actualArc + diff;
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
