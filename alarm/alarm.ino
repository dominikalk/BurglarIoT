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
    int delimiter = msg.indexOf("_");
    String msg1 = msg.substring(0, delimiter);
    String msg2 = msg.substring(delimiter + 1);

    if (msg1 == "Enabled: True" || msg2 == "Enabled: True"){
      alarmEnabled = true;
    } else if (msg1 == "Enabled: False" || msg2 == "Enabled: False") {
      alarmEnabled = false;
    }

    if (msg1 == "Triggered: True" || msg2 == "Triggered: True"){
      triggerEnabled = HIGH;
    } else if (msg1 == "Triggered: False" || msg2 == "Triggered: False") {
      triggerEnabled = LOW;
    }
  }

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
    Serial.println("Triggered: True");
  } else {
    Serial.println("Triggered: False");
  }

  // digitalWrite(buzzerPin, triggerEnabled);
  digitalWrite(buttonPin, triggerEnabled);
  delay(1000);
}