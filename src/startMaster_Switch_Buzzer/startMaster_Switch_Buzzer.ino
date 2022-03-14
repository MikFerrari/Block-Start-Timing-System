#include <pitches.h>

int start_switch_1 = 3;
int start_switch_2 = 5; // not used
int buzzer_pin = 8;

int start_switch_1_status;
int start_switch_2_status; // not used
int start_switch_1_status_old;
int start_switch_2_status_old; // not used
int delay_SET = 0;
float t0 = 0.0;
float dt = 0.0;

bool start_command = false;
bool timer_started = false;
 
// SET command
int duration_SET = 500;  // ms
int tone_SET = NOTE_C5;

// START command (GUN)
int duration_GUN = 250;  // ms
int tone_GUN = NOTE_G5;

// RUN command (first strides, to make sure the athlete knows the timer has started)
int duration_RUN = 100;  // ms
int tone_RUN = NOTE_C6;
int pause_RUN = 200; // ms
int interval_RUN = 1500; // ms


void setup() {
  Serial.begin(9600);
  pinMode(start_switch_1, INPUT);
  pinMode(start_switch_2, INPUT); // not used
  
  start_switch_1_status = digitalRead(start_switch_1);
  start_switch_1_status_old = start_switch_1_status;
}


void loop() {

  start_switch_1_status = digitalRead(start_switch_1);
  
  if (start_switch_1_status != start_switch_1_status_old) {
    start_command = true;
  }
  
  if (start_command == true) {
    // pause to get ready
    delay(3000);
    
    // SET command
    tone(buzzer_pin,tone_SET,duration_SET);

    delay_SET = 1500 + random(0,1000); // ms
    delay(delay_SET);

    // START command
    tone(buzzer_pin,tone_GUN,duration_GUN);

    start_command = false;
  }

  if (timer_started == true) {
    t0 = millis();
    dt = t0;
    while (dt - t0 <= interval_RUN) {
      tone(buzzer_pin,tone_RUN,duration_RUN);
      delay(pause_RUN);
      dt = millis();
    }
  }

  start_switch_1_status_old = start_switch_1_status;
}
