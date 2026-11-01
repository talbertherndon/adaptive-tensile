
#include <Stepper.h>

const int stepsPerRevolution = 2048;
Stepper myStepper(stepsPerRevolution, 8, 10, 9, 11);

// Potentiometer
const int potPin = A6;
const int center = 512;
const int deadZone = 50;
const int maxSpeed = 10;

// Button
const int buttonPin = 3;
bool motorOn = false;
bool lastReading = LOW;
bool buttonState = LOW;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
  Serial.begin(9600);

  // Grove button: HIGH when pressed
  pinMode(buttonPin, INPUT);

  Serial.println("Motor OFF");
}

void loop() {
  // Read and debounce button
  bool reading = digitalRead(buttonPin);

  if (reading != lastReading) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        motorOn = !motorOn;
        Serial.println(motorOn ? "Motor ON" : "Motor OFF");
      }
    }
  }

  lastReading = reading;

  // Motor OFF
  if (!motorOn) {
    delay(10);
    return;
  }

  // Read potentiometer
  int pot = analogRead(potPin);
  int distance = abs(pot - center);

  if (distance < deadZone) {
    delay(10);
    return;
  }

  int speed = map(distance, deadZone, 512, 1, maxSpeed);
  speed = constrain(speed, 1, maxSpeed);

  myStepper.setSpeed(speed);

  if (pot > center) {
    myStepper.step(10);
  } else {
    myStepper.step(-10);
  }

  Serial.print("Pot: ");
  Serial.print(pot);
  Serial.print(" | Speed: ");
  Serial.println(speed);
}
