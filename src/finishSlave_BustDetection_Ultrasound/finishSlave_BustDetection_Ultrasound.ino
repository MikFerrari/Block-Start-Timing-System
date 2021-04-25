// Import libraries for LCD Display, HC-12 Radio Module and Ultrasound Sensor
#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
// #include <SR04.h>
#include <NewPing.h>

#define TRIG_PIN 6
#define ECHO_PIN 5
#define MAX_DISTANCE 400
#define BAUDRATE 9600

// Variable declarations
SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
// SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
NewPing sr04(TRIG_PIN,ECHO_PIN,MAX_DISTANCE);

int distance;
int ultrasoundStatus;
int threshold = 80; // If the athete passes within 80cm, the sensor detects it

bool athleteRunning = false;
bool finishLineReached = false;
bool acquisitionCompleted = false;

float initialTime = 0;
float totalTime = 0;
int i = 0;

const int nTimes = 300;  // Memorize times for 0.3s with 1ms resolution
float times[nTimes] = { };
byte ultrasoundValues[nTimes] = { };   

int count;
byte k = 0;
int maxCount = 0;
int maxIdx = 0;

void setup() {
  // Set up the LCD's number of columns and rows
  lcd.begin(16, 2);

  // Initialise Serial communication and Radio Module
  Serial.begin(BAUDRATE);
  HC12.begin(BAUDRATE);
}


void loop() {
  // Read sensor output to determine if the athlete has reached the finish line
  distance = sr04.ping_cm();
  if(distance < threshold && distance != 0) {
    ultrasoundStatus = LOW;
    Serial.println(distance);
  }
  else {
    ultrasoundStatus = HIGH;
  }

  // Receive start signal from master
  while(HC12.available() && athleteRunning == false) {
    Serial.println(char(HC12.read())); // Read HC12 data to empty its buffer
    initialTime = millis();
    HC12.end();
    lcd.clear();
    athleteRunning = true;
  }

  if(ultrasoundStatus == HIGH && athleteRunning == true && finishLineReached == false) {
    totalTime = (millis() - initialTime)/1000;
    // Print time on LCD Display  
    lcd.setCursor(0, 0);
    lcd.print("Total time [s]:");
    lcd.setCursor(0, 1);
    lcd.print(totalTime);
    Serial.println(totalTime);
  }

  if(ultrasoundStatus == LOW && athleteRunning == true && finishLineReached == false) {
    finishLineReached = true;
    i = 0;
  }

  if(finishLineReached == true && i < nTimes) {
    totalTime = (millis() - initialTime)/1000;
    times[i] = totalTime;
    ultrasoundValues[i] = ultrasoundStatus;
    // Serial.println(ultrasoundValues[i]); // For debugging
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
      if(ultrasoundValues[j] == LOW && ultrasoundValues[j] == ultrasoundValues[j-1]) {
        count++;
      }
      // Serial.println(count); // For debugging
      if(ultrasoundValues[j] == HIGH && ultrasoundValues[j-1] == LOW) {
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
    lcd.print(times[maxIdx]);
    // Serial.println(times[maxIdx]); // For debugging
    // Serial.println(maxCount); // For debugging
    // Serial.println(maxIdx); // For debugging
  
    acquisitionCompleted = false;
    athleteRunning = false;
    finishLineReached = false;
    HC12.begin(BAUDRATE);
  }
  
  // Serial.println(ultrasoundStatus); // For debugging
}
