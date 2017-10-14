#define WIND_HIGH 1023   //nominal 1023
#define WIND_PIN 7

int readWind() {
  int windAngle = 0;
#if WIND_SENSOR_EXISTS
  // Read values from the WindSensor
  int windPulseWidth = pulseIn(WIND_PIN, HIGH);
  //Serial.print("Wind pulse width: "); Serial.println(windPulseWidth);
  // Convert the wind angle to degrees from PWM.  Range -180 to +180
  windAngle = map(windPulseWidth, 0, WIND_HIGH, 180, -180);
  windAngle constrain(windAngle, -180, 180);
#endif
  return windAngle;
}

