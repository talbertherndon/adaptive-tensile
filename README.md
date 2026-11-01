# Adaptive Tensile

Arduino sketch for an adaptive tensile structure prototype. A 28BYJ-48 stepper motor tensions the structure, and a potentiometer controls its direction and speed.

## How it works

- **Button (pin 3):** toggles the motor on and off.
- **Potentiometer (A6):** centered = stopped (with a dead zone); turn one way to wind, the other way to unwind. The further from center, the faster it moves.
- Pot readings and speed are printed to the Serial Monitor at 9600 baud.

## Hardware

| Part | Connection |
|---|---|
| 28BYJ-48 stepper + ULN2003 driver | IN1–IN4 → pins 8, 9, 10, 11 |
| Potentiometer | A6 |
| Grove button | pin 3 (HIGH when pressed) |

## Usage

Open `adaptive_tensile.ino` in the Arduino IDE, select your board and port, and upload. Uses the built-in `Stepper` library.
