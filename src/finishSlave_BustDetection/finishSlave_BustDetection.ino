// Import libraries for LCD Display and HC-12 Radio Module
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define BAUDRATE 9600

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const byte photocellPin = 6;
byte photocellStatus;

bool athleteRunning = false;
bool finishLineReached = false;
bool acquisitionCompleted = false;

float initialTime = 0;
float totalTime = 0;
int i = 0;

const int nTimes = 300;  // Memorize times for 0.3s with 1ms resolution
const byte bufferSize = 100; // Detect up to 100 voltage changes in photocell signal
float times[nTimes] = { };
byte photocellValues[nTimes] = { };   


void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Initialise Serial communication and Radio Module
  Serial.begin(BAUDRATE);
  HC12.begin(BAUDRATE);

  // Declare starting block pin
  pinMode(photocellPin,INPUT);
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  photocellStatus = digitalRead(photocellPin);

  // Receive start signal from master
  while(HC12.available() && athleteRunning == false && finishLineReached == false) {
    Serial.println(char(HC12.read())); // Read HC12 data to empty its buffer
    initialTime = millis();
    HC12.end();
    lcd.clear();
    athleteRunning = true;
  }

  if(photocellStatus == HIGH && athleteRunning == true && finishLineReached == false) {
    totalTime = (millis() - initialTime)/1000;
    // Print time on LCD Display  
    lcd.setCursor(0, 0);
    lcd.print("Total time [s]:");
    lcd.setCursor(0, 1);
    lcd.print(totalTime);
    Serial.println(totalTime);
  }

  if(photocellStatus == LOW && athleteRunning == true && finishLineReached == false) {
    finishLineReached == true;
    i = 0;
  }

  if(finishLineReached == true && i < nTimes) {
    totalTime = (millis() - initialTime)/1000;
    times[i] = totalTime;
    photocellValues[i] = photocellStatus;
    i++;
    if(i == nTimes) {
      acquisitionCompleted = true;
    }
  }

  if(acquisitionCompleted == true) {
    int count = 1;
    byte k = 0;
    int maxCount = 0;
    int maxIdx = 0;
    int counts[bufferSize] = { };
    
    for(int j = 1; j < nTimes; j++) {
      if(photocellValues[j] == LOW && photocellValues[j] == photocellValues[j-1]) {
        count++;
      }
      if(photocellValues[j] == HIGH && photocellValues[j-1] == LOW) {
        counts[k] = count;
        if(count > maxCount) {
          maxCount = count;
          maxIdx = j-count;
        }
        count = 1;
        k++;
      }
    }
    
    // Print time on LCD Display  
    lcd.setCursor(0, 0);
    lcd.print("Total time [s]:");
    lcd.setCursor(0, 1);
    lcd.print(times[maxIdx]);
    Serial.println(times[maxIdx]);
  
    acquisitionCompleted = false;
    athleteRunning = false;
    HC12.begin(BAUDRATE);
  }
  
  // Serial.println(photocellStatus); // For debugging
}
