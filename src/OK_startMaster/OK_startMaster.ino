/*
 * - False start detection
 * - Sounds for automatic start
 * - Reaction time detection
 * - Acoustic feedback when athlete starts running
 */

// Import library for HC-12 Radio Module and to generate sounds with buzzer
#include <SoftwareSerial.h>
#include <pitches.h>

// Variable declarations
SoftwareSerial HC12(2, 12); // HC-12 TX Pin, HC-12 RX Pin

// PIN declarations
const int start_switch_1 = 3;
const int start_switch_2 = 5; // not used
const int buzzer_pin = 8;
const int blockPin = 6;

// Status variables
int blockStatus;
int start_switch_1_status;
int start_switch_2_status; // not used
int start_switch_1_status_old;
int start_switch_2_status_old; // not used

// Time variables
const int initial_pause = 3000; // ms
int delay_SET;
float t0 = 0.0;
float dt = 0.0;
float reaction_time;

// Boolean flags
bool athleteReady = false;
bool start_command = false;
bool timer_started = false;
bool false_start = false;
bool gun_fired = false;

// SET command
const int duration_SET = 500;  // ms
const int tone_SET = NOTE_C5;

// START command (GUN)
const int duration_GUN = 250;  // ms
const int tone_GUN = NOTE_G5;

// RUN command (first strides, to make sure the athlete knows the timer has started)
const int duration_RUN = 100;  // ms
const int tone_RUN = NOTE_C6;
const int pause_RUN = 200; // ms
const int interval_RUN = 1500; // ms

// FALSE START command
const int duration_FALSE = 1500;  // ms
const int tone_FALSE = NOTE_C6;


void setup() {
  // Initialise Serial communication and Radio Module
  Serial.begin(9600);
  HC12.begin(9600);

  // Declare pins as inputs
  pinMode(blockPin,INPUT);
  pinMode(start_switch_1, INPUT);
  pinMode(start_switch_2, INPUT); // not used

  // Detect initial position of the switch
  start_switch_1_status = digitalRead(start_switch_1);
  start_switch_1_status_old = start_switch_1_status;
}


void loop() {
  // Read block and switch status
  blockStatus = digitalRead(blockPin); // Normally open signal
  start_switch_1_status = digitalRead(start_switch_1);

  // The athlete is ready on the blocks
  if(blockStatus == HIGH) {
    Serial.println("1");
    athleteReady = true;
    if (start_switch_1_status != start_switch_1_status_old) {
      Serial.println("2");
      start_command = true;
    }
  }

  // The gun was fired and the timer has started
  if(athleteReady == true && start_command == true) {
    Serial.println("3");
    // pause to get ready
    delay(3000);
    
    // SET command
    tone(buzzer_pin,tone_SET,duration_SET);

    delay_SET = 1500 + random(0,1000); // ms
    delay(delay_SET);

    // START command
    tone(buzzer_pin,tone_GUN,duration_GUN);
    HC12.write("s");
    reaction_time = millis();

    gun_fired = true;
    start_command = false;  
  }

  // The athlete has left the block
  if(athleteReady == true && blockStatus == LOW) {
    Serial.println("4");
    if(gun_fired == false) {
      Serial.println("5");
      false_start = true;
    }
    reaction_time = millis()-reaction_time;

    if(reaction_time >= 100 && false_start == false) {
      // Acoustic feedback for the athlete: has the timer started?
      t0 = millis();
      dt = t0;
      while (dt - t0 <= interval_RUN) {
        tone(buzzer_pin,tone_RUN,duration_RUN); // Start ok -> intermittent sound
        delay(pause_RUN);
        dt = millis();
      }
      HC12.write(reaction_time);
      Serial.println(reaction_time); // debug
    }
    else {
      tone(buzzer_pin,tone_FALSE,duration_FALSE); // False start -> continuous sound
      HC12.write("false");
      false_start = false;
    }
    
    athleteReady = false;
    gun_fired = false;
  }
  
  // Upload new position of the switch
  start_switch_1_status_old = start_switch_1_status;
  
  delay(5);
}
