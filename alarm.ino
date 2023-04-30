#define PIR_MOTION_SENSOR_1 2
#define PIR_MOTION_SENSOR_2 3

const int soundPin = 4;   
const int buzzerPin = 5;  
const int buttonPin = 6;

int MotionState1;         // the state of the Motion SENSOR 1 (PIR)
int MotionState2;         // the state of the Motion SENSOR 2 (PIR)
int soundState;
int buzzerState = LOW;
int buttonState = LOW;

bool alarmEnabled = false;
int triggerEnabled = LOW;

void setup() {
  pinMode(PIR_MOTION_SENSOR_1, INPUT);
  pinMode(PIR_MOTION_SENSOR_2, INPUT);
  pinMode(soundPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, OUTPUT);
  digitalWrite(buzzerPin, buzzerState);
  digitalWrite(buttonPin, buttonState);
  Serial.begin(9600);
}

void loop() {
  // Reading Data from Sensors
  MotionState1 = digitalRead(PIR_MOTION_SENSOR_1);
  MotionState2 = digitalRead(PIR_MOTION_SENSOR_2);
  soundState = digitalRead(soundPin);

  if (Serial.available() > 0) {
    String msg = Serial.readString();

    if (msg == "Enabled: True"){
      alarmEnabled = true;
    } else if (msg == "Enabled: False") {
      alarmEnabled = false;
    }

    if (msg == "Triggered: True"){
      triggerEnabled = HIGH;
    } else if (msg == "Triggered: False") {
      triggerEnabled = LOW;
    }
  }

  // int triggerAlarm = LOW;

  if (alarmEnabled) {
    if (MotionState1 == 1){
      triggerEnabled = HIGH;
    }
    if (MotionState2 == 1){
      triggerEnabled = HIGH;
    }
    // if (soundState == HIGH){
    //   triggerAlarm = HIGH;
    //   Serial.println("Sound");
    // }
  }

  if (triggerEnabled == HIGH){
    // digitalWrite(buzzerPin, triggerAlarm);
    //delay(3000);
    Serial.println("Movement: True");
  } else {
    Serial.println("Movement: False");
  }
  // if (alarmEnabled) {
  //   Serial.println("Enabled: True");
  // } else {
  //   Serial.println("Enabled: False");
  // }

  // digitalWrite(buzzerPin, triggerAlarm);
  digitalWrite(buttonPin, triggerEnabled);
  delay(1000);
}