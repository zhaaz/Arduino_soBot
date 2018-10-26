/*
 * Einfacher Test des Endschalters der Z-Achse
 * A. Wehner
 * 09.10.2018
 */

// define pins numbers
  const int triggerPin = 9;
  int triggerVal = 0;

void setup() {
  pinMode(triggerPin, INPUT);
  Serial.begin(19200);
}

void loop() {
  // put your main code here, to run repeatedly:

  triggerVal = digitalRead(triggerPin);
  Serial.println(triggerVal);
  delay(10);
// triggerPin = 1 -> Trigger nicht ausgelöst
// triggerPin = 0 -> Trigger ausgelöst
}
