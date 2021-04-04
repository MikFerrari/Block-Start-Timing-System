// Import library for HC-12 Radio Module
#include <SoftwareSerial.h>

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin

const int blockPin = 6;
int blockStatus;

bool athleteReady = false;


void setup() {
  // Initialise Serial communication and Radio Module
  Serial.begin(9600);
  HC12.begin(9600);

  // Declare starting block pin
  pinMode(blockPin,INPUT);
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  blockStatus = digitalRead(blockPin); // Normally closed signal

  if(blockStatus == LOW) {
    athleteReady = true;
  }

  if(athleteReady == true && blockStatus == HIGH) {
    HC12.write("s");
    athleteReady = false;  
  }

  // Serial.println(blockStatus); // For debugging
}
