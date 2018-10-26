/*
 * Test für Serial Read von Befehlen
 * Andreas Wehner 
 * 26.10.2018
 * 
 * String wird Seriel gelesen und in eine Kennziffer und eine Zahl zerlegt.
 */
 
const int numChars = 256;
char receivedChars[numChars];

char index;
long numValue; 

bool newData = false;

void setup() {
  Serial.begin(19200);
  Serial.println("<Arduino is ready>");
}

void loop() {
  // put your main code here, to run repeatedly:
  recvWithEndMarker();
  spliReceivedMessage();
  showNewData();
}


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

void showNewData(){
  if(newData == true){
    Serial.print("This just in ... ");
    Serial.println(receivedChars);
    newData = false;
  }
}

void spliReceivedMessage(){
  if(newData == true){
    index = receivedChars[0];
    numValue = atol(&receivedChars[1]);
    Serial.println(numValue);
  }
}
