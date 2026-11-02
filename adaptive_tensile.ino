
#include <Stepper.h>

// Stepper motor
const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Connections
const int potPin = A6;
const int buttonPin = 3;
const int ultrasonicPin = A2;

// Motor settings
const int center = 512;
const int deadZone = 50;
const int maxSpeed = 7;

// Ultrasonic sensor
const float detectDistance = 8.0;
const float releaseDistance = 11.0;

// Ignore additional flag triggers for 20 seconds
const unsigned long gracePeriod = 20000;

// Button debounce
bool motorOn = false;
bool lastReading = LOW;
bool buttonState = LOW;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// Motor state
int motorDirection = 1;
int currentSpeed = 0;
int flagCount = 0;

// Sensor state
bool flagPresent = false;
bool sensorArmed = true;

unsigned long lastTriggerTime = 0;
unsigned long lastSensorCheck = 0;

// Read Grove single-pin ultrasonic sensor
float readDistance() {
  pinMode(ultrasonicPin, OUTPUT);

  digitalWrite(ultrasonicPin, LOW);
  delayMicroseconds(2);

  digitalWrite(ultrasonicPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicPin, LOW);

  pinMode(ultrasonicPin, INPUT);

  unsigned long duration =
    pulseIn(ultrasonicPin, HIGH, 15000);

  if (duration == 0) {
    return -1;
  }

  return duration / 58.0;
}

void setup() {
  Serial.begin(9600);

  pinMode(buttonPin, INPUT);

  myStepper.setSpeed(2);

  Serial.println("DOUBLE FLAG SYSTEM READY");
  Serial.println("Motor OFF");
}

void loop() {

  // -------------------------
  // BUTTON ON/OFF
  // -------------------------

  bool reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        motorOn = !motorOn;

        if (motorOn) {
          // Set initial direction from dial
          int pot = analogRead(potPin);
          motorDirection = pot >= center ? 1 : -1;

          // Reset flag detection
          sensorArmed = true;
          flagPresent = false;

          Serial.println("Motor ON");
        } else {
          currentSpeed = 0;
          Serial.println("Motor OFF");
        }
      }
    }
  }

  lastReading = reading;

  if (!motorOn) {
    delay(10);
    return;
  }

  // -------------------------
  // POTENTIOMETER SPEED
  // -------------------------

  int pot = analogRead(potPin);
  int distance = abs(pot - center);

  currentSpeed = 0;

  if (distance >= deadZone) {
    currentSpeed = map(
      distance,
      deadZone,
      450,
      1,
      maxSpeed
    );

    currentSpeed = constrain(
      currentSpeed,
      1,
      maxSpeed
    );
  }

  // -------------------------
  // ULTRASONIC FLAG DETECTION
  // -------------------------

  if (millis() - lastSensorCheck >= 40) {
    lastSensorCheck = millis();

    // Re-arm after grace period
    if (!sensorArmed &&
        millis() - lastTriggerTime >= gracePeriod) {
      sensorArmed = true;

      Serial.println("Sensor READY");
    }

    float distanceCM = readDistance();

    bool detected =
      distanceCM > 0 &&
      distanceCM <= detectDistance;

    // Flag must leave the sensor range
    // before it can trigger again
    if (distanceCM < 0 ||
        distanceCM >= releaseDistance) {
      flagPresent = false;
    }

    // Reverse when a new flag is detected
    if (sensorArmed &&
        !flagPresent &&
        detected &&
        currentSpeed > 0) {

      motorDirection *= -1;
      flagCount++;

      flagPresent = true;
      sensorArmed = false;
      lastTriggerTime = millis();

      Serial.println("--------------------");
      Serial.println("FLAG DETECTED");
      Serial.print("Flag count: ");
      Serial.println(flagCount);

      Serial.print("New direction: ");
      Serial.println(
        motorDirection == 1 ? "CW" : "CCW"
      );

      Serial.println("Grace period: 20 seconds");
      Serial.println("--------------------");
    }
  }

  // -------------------------
  // MOTOR MOVEMENT
  // -------------------------

  if (currentSpeed > 0) {
    myStepper.setSpeed(currentSpeed);

    myStepper.step(motorDirection * 5);
  }
}
