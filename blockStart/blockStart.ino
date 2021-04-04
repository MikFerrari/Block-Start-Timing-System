#include <SoftwareSerial.h>
#include <LiquidCrystal.h>

SoftwareSerial HC12(2, 3); // HC-12 TX Pin, HC-12 RX Pin
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
const int pin = 6;
const int pinFotocellula = 5;
int ingresso;
int ingressoFotocellula;
bool pronto = false;
bool inCorsa = false;
float tempoCorrente = 0;
float tempoFinale = 0;

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  
  pinMode(pin,INPUT);
  pinMode(pinFotocellula,INPUT);
  
  Serial.begin(9600);
  HC12.begin(9600);
}

void loop() {
  ingresso = digitalRead(pin);
  ingressoFotocellula = digitalRead(pinFotocellula);
  
  lcd.setCursor(0, 0);
  lcd.print(ingresso);

  lcd.setCursor(0, 1);
  lcd.print(ingressoFotocellula);


  if(ingresso == 0) {
    pronto = true;
  }

  if(pronto == true && ingresso == 1) {
    tempoCorrente = millis();
    pronto = false;
    inCorsa = true;  
  }

  if(ingressoFotocellula == 0 && inCorsa == true) {
    tempoFinale = millis() - tempoCorrente;
    inCorsa = false;
  }

  // Serial.println(tempoFinale/1000);
  
  while(HC12.available()) {        // If HC-12 has data
    Serial.write(HC12.read());      // Send the data to Serial monitor
  }
  while(Serial.available()) {      // If Serial monitor has data
    HC12.write(Serial.read());      // Send that data to HC-12
  }
  
}
