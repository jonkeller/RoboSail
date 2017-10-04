/* BoatCodeStarterWindOnly rev 7/31/2017
© 2014-2017 RoboSail
Find detailed description in Decription tab
*/
#include <Servo.h>
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
#include <UsefulCalcs.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#include "RoboSail.h"
boolean displayValues = true;  //true calls function for values to be printed to monitor

//Fill in min/max parameters for the RC Receiver and WindSensor in RoboSail.h tab
Adafruit_GPS GPS(&Serial);
// initialize utility that will convert lat/lon to (x,y) positions in meters
UsefulCalcs calc(false);

void setup() {
  Serial.begin(115200);
  accel.begin();
  mag.begin();

  Serial.println("\nRoboSail BoatCode - XXXXX\n");  //write program name here
  // Set RC receiver and WindSensor on digital input pins
  declarePins();

  checkGPS();
  checkCompass();
}

void loop() {
  //*********** Read in data from the RC receiver and sensors *********
  readReceiver();
  readWind();
  readAccel();
  readGPS();  //puts values in "start" and "relative" variable

  //Read heading and tilt from the Compass
  readCompassAccel();

  // You now have values from the RC Receiver and Sensors in these variables: 
  // rudderPosition, sailPosition, and windAngle, 
  // pitchAccel, rollAccel, yawAccel, robosailRollAccel, heading,
  // robosailHeading, robosailRoll,
  // startPositionX, startPositionY, relPositionX, relPositionY, angleFromStart
  // Calculate new values for rudderPosition and sailPosition in degrees 
  // and set those variables to the new values.
  // If you do not set the values, it will use the values from the RC Receiver
  // For example, to make the rudder follow the wind angle you would have:
  // rudderPosition = windAngle;
  //**************** your code here ******************
  
  Serial.print("Sail position: ");
  Serial.println(sailPosition);
  int sailPct = map(constrain(sailPosition, SAIL_LOW, SAIL_HIGH), SAIL_LOW, SAIL_HIGH, 0, 100);
  Serial.print("Sail position %:");
  Serial.println(sailPct);
  
  if (sailPct >= 90) { // Autopilot mode iff sail joystick is at/above 90% 
    Serial.println("In autopilot mode");
    // abs(windAngle): 45...180 -> abs(sail) 0...90
    sailPosition = map(constrain(abs(windAngle), 45, 180), 45, 180, 0, 90);
    Serial.print("Autopilot sail position:");
    Serial.println(sailPosition);
  
    //rudderPosition = TODO: Use rudder to follow planned path
    // TODO: Goal: 42.360685, -71.073914
    // Started at startPositionX, startPositionY iff start_pos_found is true
  } else {
    Serial.println("In manual mode");
  }

  /********************* send commands to motors *************************/
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
  
  if (displayValues) {printToMonitor();}
  
} //end of loop()


