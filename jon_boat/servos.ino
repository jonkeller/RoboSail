/************Functions to drive Sail and Rudder servos ****************/
 // This code takes in the desired postions for the servos in degrees (as 
 // defined in RoboSail) then calculates appropriate values for the servo commands, 
 // making sure not to send the servos to impossible positions, which could 
 // damage the servo motors.
 // The Rudder servo motor ranges from 0 to 180 with 90 deg in the center
 // The Sailwinch servo is at ~55 deg when full-in, which we think of as 0 deg,
 // and ~125 deg when full out, which we think of as 90 deg

void driveSailServo(int sailPos) {
#if SERVOS_EXIST
  if ((sailPos >= 0) && (sailPos <= 90)) {
    int sailServoOut = map(sailPos, 0, 90, 55, 125);
    sailServo.write(sailServoOut);
  } else {
    Serial.print("ERROR - sail position out of range: ");
    Serial.println(sailPos);
  }
#endif  
}

void driveRudderServo(int rudderPos) {
#if SERVOS_EXIST
  if ((rudderPos >= -60) && (rudderPos <= 60)) {
    int rudderServoOut = map(rudderPos, -90, 90, 0, 180);
    rudderServo.write(rudderServoOut);
  } else {
    Serial.print("ERROR - rudder position out of range: ");
    Serial.println(rudderPos);
  }
#endif
}

