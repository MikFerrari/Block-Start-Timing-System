void setup() {
  Serial.begin(2400);             // Serial port to computer
  Serial1.begin(2400);               // Serial port to HC12
}

void loop() {
  while (Serial1.available()) {        // If HC-12 has data
    Serial.write(Serial1.read());      // Send the data to Serial monitor
  }
  while (Serial.available()) {      // If Serial monitor has data
    Serial1.write(Serial.read());      // Send that data to HC-12
  }
}
