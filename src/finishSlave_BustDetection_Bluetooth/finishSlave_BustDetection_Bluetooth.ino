// Import libraries for LCD Display and HC-12 Radio Module + Bluetooth Module
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define BAUDRATE 9600

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
SoftwareSerial bluetooth(1, 5); // RX Pin, TX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const byte photocellPin = 6;
byte photocellStatus;

bool athleteRunning = false;
bool finishLineReached = false;
bool acquisitionCompleted = false;

float initialTime = 0;
float totalTime = 0;
int i = 0;

const int nTimes = 200;  // Memorize times for 0.2s with 1ms resolution
float times[nTimes] = { };
byte photocellValues[nTimes] = { };   

int count;
byte k = 0;
int maxCount = 0;
int maxIdx = 0;


void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}


void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Initialise Serial communication, Radio Module and Bluetooth Module
  Serial.begin(BAUDRATE);
  bluetooth.begin(BAUDRATE);
  HC12.begin(BAUDRATE);

  // Declare starting block pin
  pinMode(photocellPin,INPUT);

  // serialFlush();
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  photocellStatus = digitalRead(photocellPin);
  // Serial.println(photocellStatus);  // For debugging

  // Receive start signal from master
  while(HC12.available() && athleteRunning == false) {
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
    lcd.print(totalTime, 3);
    Serial.println(totalTime);
  }

  if(photocellStatus == LOW && athleteRunning == true && finishLineReached == false) {
    finishLineReached = true;
    i = 0;
  }

  if(finishLineReached == true && i < nTimes) {
    totalTime = (millis() - initialTime)/1000;
    times[i] = totalTime;
    photocellValues[i] = photocellStatus;
    // Serial.println(photocellValues[i]); // For debugging
    // Serial.println(times[i]); // For debugging
    i++;
    if(i == nTimes) {
      acquisitionCompleted = true;
    }
  }

  if(acquisitionCompleted == true) {
    count = 0;
    k = 0;
    maxCount = 0;
    maxIdx = 0;
    
    for(int j = 1; j < nTimes; j++) {
      if(photocellValues[j] == LOW && photocellValues[j] == photocellValues[j-1]) {
        count++;
      }
      // Serial.println(count); // For debugging
      if(photocellValues[j] == LOW && photocellValues[j-1] == HIGH) {
        if(count > maxCount) {
          maxCount = count;
          maxIdx = j-count-1;
        }    
        count = 0;
        k++;
      }
    }
      
    // Print time on LCD Display  
    lcd.setCursor(0, 0);
    lcd.print("Total time [s]:");
    lcd.setCursor(0, 1);
    lcd.print(times[maxIdx], 3);
    // Serial.println(times[maxIdx]); // For debugging
    // Serial.println(maxCount); // For debugging
    // Serial.println(maxIdx); // For debugging
  
    acquisitionCompleted = false;
    athleteRunning = false;
    finishLineReached = false;

    // Print to Serial (bluetooth phone application)
    char buff[20] = "Total time [s]: ";
    char floatBuff[10];
    byte nIntDigits = 3;
    byte nDecimalDigits = 3;
    dtostrf(times[maxIdx], nIntDigits, nDecimalDigits, floatBuff);
    strcat(buff, floatBuff);
    Serial.println(buff);
    
    HC12.begin(BAUDRATE);
  }
  
}
