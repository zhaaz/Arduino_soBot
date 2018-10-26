/*
 * Einfacher Test des Drehmodul Motors 
 * A. Wehner
 * 09.10.2018
 */
 
    // defines pins numbers
    const int dirPin = 2; 
    const int stepPin = 3; 
    const int sleepPin = 4; 

    const int delaytime = 750;  //us
     
    void setup() {
      // Sets the two pins as Outputs
      pinMode(stepPin,OUTPUT); 
      pinMode(dirPin,OUTPUT);
      pinMode(sleepPin,OUTPUT);
    }
    void loop() {
      digitalWrite(dirPin,HIGH); // Enables the motor to move in a particular direction
      digitalWrite(sleepPin, HIGH); // Motor ein
      // Makes 200 pulses for making one full cycle rotation
      for(int x = 0; x < 2000; x++) {
        digitalWrite(stepPin,HIGH); 
        delayMicroseconds(delaytime); 
        digitalWrite(stepPin,LOW); 
        delayMicroseconds(delaytime); 
      }
      
      digitalWrite(sleepPin, LOW);  // Motor Aus 
      delay(5000); // One second delay
      
      digitalWrite(sleepPin, HIGH);  // Motor Aus 
      
      digitalWrite(dirPin,LOW); //Changes the rotations direction
      // Makes 400 pulses for making two full cycle rotation
      for(int x = 0; x < 2000; x++) {
        digitalWrite(stepPin,HIGH);
        delayMicroseconds(delaytime);
        digitalWrite(stepPin,LOW);
        delayMicroseconds(delaytime);
      }

      digitalWrite(sleepPin, LOW);  // Motor Aus 
      delay(5000);
    }
