// Import libraries for LCD Display and HC-12 Radio Module
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int photocellPin = 6;
int photocellStatus;

bool athleteRunning = false;

float initialTime = 0;
float totalTime = 0;


void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

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
  }

  // Print time on LCD Display  
  lcd.setCursor(0, 0);
  lcd.print("Total time [s]:");
  lcd.setCursor(0, 1);
  lcd.print(totalTime);
  
  // Serial.println(photocellStatus); // For debugging
}
