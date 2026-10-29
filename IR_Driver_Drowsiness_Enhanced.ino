/*
  IR-Based Driver Drowsiness Detection and Alarm System
  Enhanced Arduino example

  Planned pins:
    IR sensor OUT -> D2
    Active buzzer -> D8
    Red LED       -> D9
    Yellow LED    -> D10
    Green LED     -> D11

  Assumption: sensor reads LOW when the target eye-closure condition is detected.
  Verify this polarity with your actual sensor module before use.

  Safety note: This is an educational prototype, not a certified driver-safety device.
*/

const byte IR_SENSOR = 2;
const byte BUZZER = 8;
const byte RED_LED = 9;
const byte YELLOW_LED = 10;
const byte GREEN_LED = 11;

const unsigned long DROWSINESS_TIME_MS = 2000UL;
const unsigned long BUZZER_INTERVAL_MS = 300UL;

unsigned long eyeClosedStart = 0;
unsigned long lastBuzzerToggle = 0;
unsigned long drowsinessCount = 0;

bool eyeClosed = false;
bool drowsinessDetected = false;
bool buzzerState = false;

void setIndicators(bool red, bool yellow, bool green) {
  digitalWrite(RED_LED, red ? HIGH : LOW);
  digitalWrite(YELLOW_LED, yellow ? HIGH : LOW);
  digitalWrite(GREEN_LED, green ? HIGH : LOW);
}

void stopAlarm() {
  digitalWrite(BUZZER, LOW);
  buzzerState = false;
}

void setup() {
  pinMode(IR_SENSOR, INPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  stopAlarm();
  setIndicators(false, false, true);

  Serial.begin(9600);
  Serial.println(F("IR Driver Drowsiness Detection System"));
  Serial.println(F("System started. Verify sensor polarity before testing."));
}

void loop() {
  const int sensorValue = digitalRead(IR_SENSOR);
  const unsigned long now = millis();

  // Assumes LOW means the target eye-closure condition is detected.
  if (sensorValue == LOW) {
    if (!eyeClosed) {
      eyeClosed = true;
      eyeClosedStart = now;
      Serial.println(F("Sensor indicates eye closure."));
    }

    const unsigned long closedDuration = now - eyeClosedStart;

    if (closedDuration >= DROWSINESS_TIME_MS) {
      if (!drowsinessDetected) {
        drowsinessDetected = true;
        ++drowsinessCount;

        Serial.println(F("WARNING: prolonged eye-closure condition detected."));
        Serial.print(F("Drowsiness events this session: "));
        Serial.println(drowsinessCount);
      }

      setIndicators(true, false, false);

      if (now - lastBuzzerToggle >= BUZZER_INTERVAL_MS) {
        lastBuzzerToggle = now;
        buzzerState = !buzzerState;
        digitalWrite(BUZZER, buzzerState ? HIGH : LOW);
      }
    } else {
      // Monitoring state: threshold has not yet been reached.
      setIndicators(false, true, false);
      stopAlarm();
    }
  } else {
    if (eyeClosed) {
      Serial.println(F("Sensor returned to normal state."));
      if (drowsinessDetected) {
        Serial.println(F("Alert cleared."));
      }
    }

    eyeClosed = false;
    drowsinessDetected = false;
    stopAlarm();
    setIndicators(false, false, true);
  }
}
