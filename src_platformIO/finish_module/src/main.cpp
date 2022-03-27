#include <Arduino.h> // required by PlatformIO

// Import libraries for LCD Display and SoftwareSerial for Bluetooth Module
// BLUETOOTH DEVICE HC-05 - PSW: 1234

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define BAUDRATE 9600

// Variable declarations
SoftwareSerial bluetooth(4, 5); // RX Pin, TX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const byte resetButtonPin = 13;
byte resetButtonStatus;
const byte photocellPin = 6;
byte photocellStatus;

bool athleteRunning = false;
bool finishLineReached = false;
bool acquisitionCompleted = false;
bool stayIdle = false;

float initialTime = 0;
float totalTime = 0;
int i = 0;
float reaction_time = 0;

char received;
char received_reaction;

const int nTimes = 200;  // Memorize times for 0.2s with 1ms resolution
float times[nTimes] = { };
byte photocellValues[nTimes] = { };   

int count;
byte k = 0;
int maxCount = 0;
int maxIdx = 0;
int idleDuration = 30*1000; // Wait 30 seconds
int t = 0;

// parameters to print to Serial (bluetooth phone application)
byte nIntDigits = 3;
byte nDecimalDigits = 3;
    

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
  Serial1.begin(BAUDRATE); // On Arduino Leonardo Pins 1 and 0 are used as TX and RX -> USE THIS INSTEAD OF HC12

  // Declare starting block pin
  pinMode(photocellPin,INPUT);
  pinMode(resetButtonPin,INPUT);

  // serialFlush();

  // Print on LCD Display  
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("- B&F Timing -");
  lcd.setCursor(1, 1);
  lcd.print("Ready to start!");

  // Print to Serial (bluetooth phone application)
  char readyMessage[17] = "Ready to start!";
  Serial.println(readyMessage);
    
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  photocellStatus = digitalRead(photocellPin);
  resetButtonStatus = digitalRead(resetButtonPin);

  // Receive start signal from master
  while(Serial1.available() && athleteRunning == false && stayIdle == false) {
    received = Serial1.read(); // Read data from Serial1 (antenna) to empty its buffer
    Serial.println(received); // debug
    if(received == 's') {
      initialTime = millis();
      Serial.println("Started");
      athleteRunning = true;

    }
    else if(received == 'f'){
      Serial.println("False Start");
    }
    lcd.clear();
  }

  if(received == 's' && stayIdle == false) {
    while(Serial1.available()) {
      received_reaction = Serial1.read(); // Read data from Serial1 (antenna) to empty its buffer
      if(received_reaction == 't') {
        reaction_time = (millis() - initialTime)/1000;  
      }
      Serial.println(reaction_time,3); // debug
      Serial1.end();
    }
  
    if(photocellStatus == HIGH && athleteRunning == true && finishLineReached == false) {
      totalTime = (millis() - initialTime)/1000;
      // Print time on LCD Display  
      lcd.setCursor(0, 0);
      lcd.print("Time:");
      lcd.setCursor(7, 0);
      lcd.print(totalTime, 3);
      lcd.setCursor(0, 1);
      lcd.print("Reaction:");
      lcd.setCursor(11, 1);
      lcd.print(reaction_time, 3);
  
      // Send time to bluetooth device and display it
      // Serial.println(totalTime);
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
      lcd.print("Time:");
      lcd.setCursor(7, 0);
      lcd.print(times[maxIdx], 3);
      lcd.setCursor(0, 1);
      lcd.print("Reaction:");
      lcd.setCursor(11, 1);
      lcd.print(reaction_time, 3);
      
      // Serial.println(times[maxIdx]); // For debugging
      // Serial.println(maxCount); // For debugging
      // Serial.println(maxIdx); // For debugging
    
      acquisitionCompleted = false;
      athleteRunning = false;
      finishLineReached = false;
      stayIdle = true;
  
      // Print to Serial (bluetooth phone application)
      char buff[18] = "Total time [s]: ";
      char floatBuff[10];
      dtostrf(times[maxIdx], nIntDigits, nDecimalDigits, floatBuff);
      strcat(buff, floatBuff);
      Serial.println(buff);
      Serial.print("Reaction time [s]: ");
      Serial.println(reaction_time,3); // 3 decimal digits

      // Reset
      Serial1.begin(BAUDRATE);
      received = '\0';
      received_reaction = '\0';
    }

  }
  
  else if (received == 'f') {
    // Print FALSE START on LCD Display  
    lcd.setCursor(2, 0);
    lcd.print("FALSE START!");
    stayIdle = true;
    received = '\0';
  }
  

  if(stayIdle == true) {
    for(t = 0; t < idleDuration; t++) {  // Display result for idleDuration seconds, then set timer to 0
      resetButtonStatus = digitalRead(resetButtonPin);
      // Serial.println(resetButtonStatus); // debug
      delay(1);
      if (resetButtonStatus == HIGH) {  // Stop waiting if the reset button is pressed
        break;
      }
    }
    
    // Print time on LCD Display  
    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("Ready to");
    lcd.setCursor(5, 1);
    lcd.print("start!");

    // Print to Serial (bluetooth phone application)
    char readyMessage[17] = "Ready to start!";
    Serial.println(readyMessage);
    
    stayIdle = false;
    
    // Dump any start command received during the IDLE status
    while(Serial1.available()) {
      Serial1.read();
    }
  }
}