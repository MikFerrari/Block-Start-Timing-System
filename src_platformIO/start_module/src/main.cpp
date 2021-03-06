#include <Arduino.h> // required by PlatformIO

/*
 * - False start detection
 * - Sounds for automatic start
 * - Reaction time detection
 * - Acoustic feedback when athlete starts running
 */

// Import library for HC-12 Radio Module and to generate sounds with buzzer
#include <SoftwareSerial.h>

#include <pitches.h> // Include to use buzzer

// Include to use speaker -> SIGNAL MUST COME FROM PIN 11 IN ORDER TO MAKE THE LIBRARY WORK!
#include <PCM.h>
// #include "Sound_SET.h"
#include "Sound_SET_short.h"
// #include "Sound_GUN.h"
#include "Sound_GUN_short.h"

#define BAUDRATE_SERIAL 2400
#define BAUDRATE_HC12 2400

// Variable declarations
SoftwareSerial HC12(2, 12); // HC-12 TX Pin, HC-12 RX Pin

// PIN declarations
const int start_switch = 5;
const int buzzer_pin = 11;
const int blockPin = 6;

// Status variables
int blockStatus;
int start_switch_status;

// Time variables
const int cycle_delay = 5; // ms
int get_ready_counter = 0;  // ms
const int get_ready_max = 3000/cycle_delay; // ms
int set_counter = 0;  // ms
int set_counter_max = 1500/cycle_delay;  // ms
float t0 = 0.0;
float dt = 0.0;
float reaction_time;

// Boolean flags
bool athleteReady = false;
bool start_command = false;
bool timer_started = false;
bool false_start = false;
bool gun_fired = false;
bool set_sound_done = false;


// Use buzzer to give SET and START commands
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
  Serial.begin(BAUDRATE_SERIAL);
  HC12.begin(BAUDRATE_HC12);

  // Declare pins as inputs
  pinMode(blockPin,INPUT);
  pinMode(start_switch, INPUT);
}


void loop() {
  // Read block and switches status
  blockStatus = digitalRead(blockPin); // Normally open signal
  start_switch_status = digitalRead(start_switch);

  // The athlete is ready on the blocks
  if(blockStatus == HIGH) {
    // Serial.println("1"); // debug
    athleteReady = true;
    if (start_switch_status == HIGH) {
      // Serial.println("2"); // debug
      start_command = true;
    }
  }

  // The gun was fired and the timer has started
  if(athleteReady == true && start_command == true && false_start == false) {
    // Serial.println("3"); // debug
    
    // Pause to get ready
    if(get_ready_counter < get_ready_max) {
      get_ready_counter += 1;
    }
    else {
      if (set_sound_done == false) {      
        // SET command
        set_sound_done = true;
        // tone(buzzer_pin,tone_SET,duration_SET); // Using buzzer
        startPlayback(sounddata_data_SET, sizeof(sounddata_data_SET)); // Using speaker
      }
      
      if(set_counter < set_counter_max) {
        set_counter += 1;
      }
      else {        
        // START command
        // tone(buzzer_pin,tone_GUN,duration_GUN); // Using buzzer
        startPlayback(sounddata_data_GUN, sizeof(sounddata_data_GUN)); // Using speaker
        HC12.write('s'); // Timer starts once the sound of the pistol has been played
        Serial.println('s'); // debug
        reaction_time = millis();
        gun_fired = true;
        start_command = false;
      }
    }
  }

  // The athlete has left the block
  if(athleteReady == true && blockStatus == LOW) {
    // Serial.println("4"); // debug
    if(gun_fired == false) {
      // Serial.println("5"); // debug
      false_start = true;
    }
    
    reaction_time = (millis()-reaction_time)/1000;

    if(reaction_time >= 0.100 && false_start == false) {
      /*
      // Acoustic feedback for the athlete: has the timer started?
      t0 = millis();
      dt = t0;
      while (dt - t0 <= interval_RUN) {
        tone(buzzer_pin,tone_RUN,duration_RUN); // Start ok -> intermittent sound
        delay(pause_RUN);
        dt = millis();
      }
      */
      HC12.write('t'); // Tell the finish module that the athlete has left the block,
                       // so that it can compute the reaction time as well
      Serial.println(reaction_time,3); // debug
      delay(3000); // to prevent immediate false start after correct start due to pressure plate elasticity
    }
    else {
      HC12.write('f');
      Serial.println('f'); // debug
      false_start = false;
      // tone(buzzer_pin,tone_FALSE,duration_FALSE); // False start -> continuous  (using buzzer)
      startPlayback(sounddata_data_GUN, sizeof(sounddata_data_GUN)); // Using speaker -> double gun sound
      delay(1000);
      startPlayback(sounddata_data_GUN, sizeof(sounddata_data_GUN)); // Using speaker
    }

    // Reset values for new start
    set_counter_max = (1500 + random(0,1000))/cycle_delay; // ms
    get_ready_counter = 0;
    set_counter = 0;
    set_sound_done = false;
    start_command = false;
    athleteReady = false;
    gun_fired = false;
  }

  /*
  // debug
  Serial.print(get_ready_counter);
  Serial.print(", ");
  Serial.println(set_counter);
  */
  // debug
  // Serial.print(start_switch_status);
  
  delay(cycle_delay);
}