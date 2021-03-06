#include <Arduino.h> // required by PlatformIO

// Import libraries for LCD Display and SoftwareSerial for Bluetooth Module
// BLUETOOTH DEVICE HC-05 - PSW: 1234

#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

#define BAUDRATE_SERIAL 2400
#define BAUDRATE_BLUETOOTH 9600
#define BAUDRATE_HC12 2400

// Variable declarations
SoftwareSerial bluetooth(5, 4); // RX Pin, TX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

const byte resetButtonPin = 13;
byte resetButtonStatus;
const byte photocellPin = 6;
byte photocellStatus;

bool athleteRunning = false;
bool finishLineReached = false;
bool acquisitionCompleted = false;
bool stayIdle = false;
bool erroneous_state = true;
bool waiting_state = true;

float initialTime = 0;
float totalTime = 0;
float reaction_time = 0;

int i = 0;
int mins = 0;
float secs = 0;

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


void printTime_lcd(float total_time, float reaction_time) {
  lcd.setCursor(0, 0);
  lcd.print("Time:");
  if(total_time<10) {
    lcd.setCursor(10, 0);
    lcd.print("0");
    lcd.setCursor(11, 0);
    lcd.print(totalTime, 3);
  } 
  else if(total_time<60) {
    lcd.setCursor(10, 0);
    lcd.print(total_time, 3);
  }
  else {
    mins = (int)total_time/60;
    secs = total_time-mins*60;

    if(mins<10) {
      lcd.setCursor(7, 0);
      lcd.print("0");
      lcd.setCursor(8, 0);
      lcd.print(mins); 
    }
    else {
      lcd.setCursor(7, 0);
      lcd.print(mins);
    }
    lcd.setCursor(9, 0);
    lcd.print(":");

    if(secs<10) {
      lcd.setCursor(10, 0);
      lcd.print("0");
      lcd.setCursor(11, 0);
      lcd.print(secs, 3); 
    }
    else {
      lcd.setCursor(10, 0);
      lcd.print(secs, 3);
    }  
  }
  lcd.setCursor(0, 1);
  lcd.print("Reaction:");
  lcd.setCursor(10, 1);
  lcd.print(reaction_time, 3);
  lcd.setCursor(15, 1);
  lcd.print("s");
}


void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Initialise Serial communication, Radio Module and Bluetooth Module
  Serial.begin(BAUDRATE_SERIAL);
  bluetooth.begin(BAUDRATE_BLUETOOTH);
  Serial1.begin(BAUDRATE_HC12); // On Arduino Leonardo Pins 1 and 0 are used as TX and RX -> USE THIS INSTEAD OF HC12

  // Declare starting block pin
  pinMode(photocellPin,INPUT);
  pinMode(resetButtonPin,INPUT);

  // serialFlush();

  // Print on LCD Display  
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("- B&F Timing -");

  for(int j = 1; j < 15; j++) {
    lcd.setCursor(j, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();

  // Print to Serial (bluetooth phone application)
  char readyMessage[17] = "Ready to start!";
  Serial.println(readyMessage);
  bluetooth.println(readyMessage);

  // Print on LCD Display 
  lcd.setCursor(4, 0);
  lcd.print("Ready to");
  lcd.setCursor(5, 1);
  lcd.print("start!");
}


void loop() {
  // Read block status to determine if the athlete is ready to start
  photocellStatus = digitalRead(photocellPin);
  resetButtonStatus = digitalRead(resetButtonPin);

  // Set flag for error checking
  erroneous_state = true;

  // Do nothing when no commands are received
  if(waiting_state == true)
    erroneous_state = false;

  // Receive start signal from master
  while(Serial1.available() && athleteRunning == false && stayIdle == false) {
    received = Serial1.read(); // Read data from Serial1 (antenna) to empty its buffer
    Serial.println(received); // debug
    lcd.clear();
    if(received == 's') {
      initialTime = millis();
      Serial.println("Started");
      bluetooth.println("Started");
      athleteRunning = true;
      waiting_state = false;

    }
    else if(received == 'f'){
      Serial.println("False Start");
      bluetooth.println("False Start");
      waiting_state = false;
    }
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
      printTime_lcd(totalTime,reaction_time);

      erroneous_state = false;
    }
  
    if(photocellStatus == LOW && athleteRunning == true && finishLineReached == false) {
      finishLineReached = true;
      i = 0;

      erroneous_state = false;
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

      erroneous_state = false;
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
      printTime_lcd(times[maxIdx],reaction_time);
      
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
      bluetooth.println(buff);
      Serial.print("Reaction time [s]: ");
      Serial.println(reaction_time,3); // 3 decimal digits
      bluetooth.print("Reaction time [s]: ");
      bluetooth.println(reaction_time,3); // 3 decimal digits

      // Reset
      Serial1.begin(BAUDRATE_HC12);
      received = '\0';
      received_reaction = '\0';

      erroneous_state = false;
    }

    erroneous_state = false;
  }
  
  else if (received == 'f') {
    // Print FALSE START on LCD Display  
    lcd.setCursor(2, 0);
    lcd.print("FALSE START!");
    stayIdle = true;
    received = '\0';

    erroneous_state = false;
  }
  
  if(stayIdle == true) {
    for(t = 0; t < idleDuration; t++) {  // Display result for idleDuration seconds, then set timer to 0
      resetButtonStatus = digitalRead(resetButtonPin);
      // Serial.println(resetButtonStatus); // debug
      delay(1);
      if (resetButtonStatus == HIGH)  // Stop waiting if the reset button is pressed
        break;       
    }
    
    // Print waiting message on LCD Display 
    lcd.clear(); 
    lcd.setCursor(4, 0);
    lcd.print("Ready to");
    lcd.setCursor(5, 1);
    lcd.print("start!");

    // Print to Serial (bluetooth phone application)
    char readyMessage[17] = "Ready to start!";
    Serial.println(readyMessage);
    bluetooth.println(readyMessage);
    
    stayIdle = false;
    waiting_state = true;
    
    // Dump any start command received during the IDLE status
    while(Serial1.available()) {
      Serial1.read();
    }

    erroneous_state = false;
  }

  // Check if execution lands in erroneous state
  if(erroneous_state == true) {
    while(true) {
      // Print ERROR message on LCD Display
      lcd.clear();
      lcd.setCursor(1, 0);
      lcd.print("Connection Lost");
      lcd.setCursor(2, 1);
      lcd.print("Press |Reset|");

      if (resetButtonStatus == HIGH) {  // Stop waiting if the reset button is pressed
        erroneous_state = false;
        break;
      }
    }
  }
}