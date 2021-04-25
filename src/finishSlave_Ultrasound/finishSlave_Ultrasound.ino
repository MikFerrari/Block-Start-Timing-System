// Import libraries for LCD Display and HC-12 Radio Module
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
int threshold = 100; // If the athete passes within 80cm, the sensor detects it

bool athleteRunning = false;

float initialTime = 0;
float totalTime = 0;


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

  if(ultrasoundStatus == HIGH && athleteRunning == true) {
    totalTime = (millis() - initialTime)/1000;
    // Print time on LCD Display  
    lcd.setCursor(0, 0);
    lcd.print("Total time [s]:");
    lcd.setCursor(0, 1);
    lcd.print(totalTime);
    // Serial.println(totalTime);
  }

  if(ultrasoundStatus == LOW && athleteRunning == true) {
    athleteRunning = false;
    HC12.begin(BAUDRATE);
  }
  
  // Serial.println(ultrasoundStatus); // For debugging
}
