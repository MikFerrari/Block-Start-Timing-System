// Import library for HC-12 Radio Module
#include <SoftwareSerial.h>

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin

const int photocellPin = 6;
int photocellStatus;

bool athleteRunning = false;

float initialTime = 0;
float totalTime = 0;


void setup() {
  // Initialise Serial communication and Radio Module
  Serial.begin(9600);
  HC12.begin(9600);

  // Declare starting block pin
  pinMode(photocellPin,INPUT);
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  photocellStatus = digitalRead(photocellPin);

  // Receive start signal from master
  while(HC12.available() && athleteRunning == false) {
    initialTime = millis();
    athleteRunning = true;
  }

  if(photocellStatus == 0 && athleteRunning == true) {
    totalTime = millis() - initialTime;
    athleteRunning = false;
    // Send to master total elapsed time
    HC12.write(totalTime);
  }
  
  Serial.println(photocellStatus); // For debugging
}
