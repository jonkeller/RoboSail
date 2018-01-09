#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define WIND_HIGH 1023   //nominal 1023
#else
#define WIND_HIGH 902
#endif

int readWind() {
  int windAngle = 0;
#if WIND_SENSOR_EXISTS
  int windPulseWidth;
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
  // Read values from the WindSensor
  windPulseWidth = pulseIn(WIND_PIN, HIGH);
#else
  windPulseWidth = analogRead(WIND_PIN);    // read the value from the sensor
#endif
  //Serial.print("Wind pulse width: "); Serial.println(windPulseWidth);
  // Convert the wind angle to degrees from PWM.  Range -180 to +180
  windAngle = map(windPulseWidth, 0, WIND_HIGH, 180, -180);
  windAngle = constrain(windAngle, -180, 180);
#endif
  return windAngle;
}

