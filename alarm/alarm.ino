#include <Wire.h>
#include "rgb_lcd.h"

#define PIR_MOTION_SENSOR_1 5 // the digital pin of the PIR sensor
const int buzzerPin = 8;  // digital pin number of the buzzer

int MotionState1;         // the state of the Motion SENSOR (PIR)
int buzzerState = LOW;    // the state of the buzzer

bool alarmEnabled = false;  
int triggerEnabled = LOW;

rgb_lcd lcd; // initialise lcd screen

void setup() {
  // setup pin modes
  pinMode(PIR_MOTION_SENSOR_1, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, OUTPUT);
  // set default values to outputs
  digitalWrite(buzzerPin, buzzerState);
  digitalWrite(buttonPin, buttonState);

  Serial.begin(9600);
  // initialise lcd screen
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Burglar System  ");
  lcd.setCursor(0, 1);
  lcd.print("Inactive.       ");
}

void loop() {
  // Reading Data from Sensor
  MotionState1 = digitalRead(PIR_MOTION_SENSOR_1);

  // initialise previous variables for comparison
  int prevTriggerEnabled = triggerEnabled;
  bool prevAlarmEnabled = alarmEnabled;

  // handle messages from python script
  if (Serial.available() > 0) {
    // parse message
    String msg = Serial.readString();
    int delimiter = msg.indexOf("_");
    String msg1 = msg.substring(0, delimiter);
    String msg2 = msg.substring(delimiter + 1);

    // handle message if enabled changed
    if (msg1 == "Enabled: True" || msg2 == "Enabled: True"){
      alarmEnabled = true;
    } else if (msg1 == "Enabled: False" || msg2 == "Enabled: False") {
      alarmEnabled = false;
    }

    // handle message if triggered changed
    if (msg1 == "Triggered: True" || msg2 == "Triggered: True"){
      triggerEnabled = HIGH;
    } else if (msg1 == "Triggered: False" || msg2 == "Triggered: False") {
      triggerEnabled = LOW;
    }
  }

  // check if motion detected
  if (alarmEnabled) {
    if (MotionState1 == HIGH){
      triggerEnabled = HIGH;
    }
  }

  // send motion message to python script
  if (triggerEnabled == HIGH){
    Serial.println("Triggered: True");
  } else {
    Serial.println("Triggered: False");
  }

  // write to buzzer outputs
  if ((millis()/500) % 3 != 0) {
    digitalWrite(buzzerPin, triggerEnabled);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // write to lcd screen if values have changed
  if(alarmEnabled != prevAlarmEnabled || triggerEnabled != prevTriggerEnabled) {
    if (alarmEnabled == false) {
      lcd.setRGB(255, 255, 255);
      lcd.setCursor(0, 1);
      lcd.print("Inactive.       ");
    } 
    else if (triggerEnabled == LOW) {
      lcd.setRGB(0, 255, 0);
      lcd.setCursor(0, 1);
      lcd.print("Active.         ");
    } 
    else if (triggerEnabled == HIGH){
      lcd.setRGB(255, 0, 0);
      lcd.setCursor(0, 1);
      lcd.print("Triggered.      ");
    }
  }

  delay(1000);
}