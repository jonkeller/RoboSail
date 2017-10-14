#include "pins.h"

void declarePins() {
#if WIND_SENSOR_EXISTS
  pinMode(WIND_PIN, INPUT);
#endif

#if SERVOS_EXIST
  pinMode(RUDDER_RC_PIN, INPUT);
  pinMode(SAIL_RC_PIN, INPUT);

  // attach the servos to the proper pins
  rudderServo.attach(RUDDER_SERVO_PIN);
  sailServo.attach(SAIL_SERVO_PIN);
#endif

  pinMode(LED_BUILTIN, OUTPUT);
}

