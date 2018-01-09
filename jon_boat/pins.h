#ifndef PINS_H
#define PINS_H

//input pins from receiver
#define RUDDER_RC_PIN 2
#define SAIL_RC_PIN 3

// Output pins to the servos
#define RUDDER_SERVO_PIN 8
#define SAIL_SERVO_PIN 9

#define WIND_PIN 7

#if SERVOS_EXIST
#include <Servo.h>
#endif

#if SERVOS_EXIST
//create servo objects
Servo rudderServo;
Servo sailServo;
#endif

#if NEOPIXELS_EXIST
#define NEOPIXEL_PIN 6
#endif

#endif

