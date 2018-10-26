/*
 * Einfacher Test des Motors der Z-Achse
 * A. Wehner
 * 09.10.2018
 */
 
    // defines pins numbers
    const int stepPin = 7; 
    const int dirPin = 6; 
    const int sleepPin = 8; 

    int delaytime = 650;
     
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
      for(int x = 0; x < 800; x++) {
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
      for(int x = 0; x < 500; x++) {
        digitalWrite(stepPin,HIGH);
        delayMicroseconds(delaytime);
        digitalWrite(stepPin,LOW);
        delayMicroseconds(delaytime);
      }

      digitalWrite(sleepPin, LOW);  // Motor Aus 
      delay(5000);
    }
