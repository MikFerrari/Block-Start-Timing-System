// Import libraries for LCD Display and HC-12 Radio Module
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const int blockPin = 6;
int blockStatus;

bool athleteReady = false;
bool athleteRunning = false;

float totalTime = 0;


void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Initialise Serial communication and Radio Module
  Serial.begin(9600);
  HC12.begin(9600);

  // Declare starting block pin
  pinMode(blockPin,INPUT);
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  blockStatus = digitalRead(blockPin);

  if(blockStatus == 0) {
    athleteReady = true;
  }

  if(athleteReady == true && blockStatus == 1) {
    HC12.write("start");
    athleteReady = false;
    athleteRunning = true;  
  }

  // Receive total elapsed time measured by the slave
  while(HC12.available()) {
    totalTime = HC12.read();
    Serial.write(totalTime); // For debugging
    athleteRunning = false;
  }

  // Print time on LCD Display  
  lcd.setCursor(0, 0); // Time computed by the master board
  lcd.print("Total time [s]:");
  lcd.setCursor(0, 1);
  lcd.print(totalTime/1000);

  // Serial.println(blockStatus); // For debugging
}
