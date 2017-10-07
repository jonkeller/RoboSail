/* BoatCodeStarterWind+GPS+Compass rev 7/31/2017
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

//                       Middle      Dock
float target_lats[3] = { 42.360601,  42.360389};
float target_lons[3] = {-71.073761, -71.073364};

float target_lat;
float target_lon;
int IRONS_DEG = 45;

void setup() {
  Serial.begin(115200);
  Serial.println("In setup()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  accel.begin();
  mag.begin();

  Serial.println("\nRoboSail BoatCode - 0.02\n");  //write program name here
  // Set RC receiver and WindSensor on digital input pins
  declarePins();

  Serial.println("Setting up GPS");
  checkGPS();
  Serial.println("Setting up Compass");
  checkCompass();
  Serial.println("Ready to loop");
  digitalWrite(LED_BUILTIN, LOW);
  test_find_absolute_angle(); // TODO Remove
}

void loop() {
  return; // TODO Remove
  Serial.println("I am in loop");
  //*********** Read in data from the RC receiver and sensors *********
  readReceiver();
  readWind();
  //readAccel();
  //readGPS();  //puts values in "start" and "relative" variable

  //Read heading and tilt from the Compass
  //readCompassAccel();

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
  int sailPct = map(constrain(sailPosition, 0, 90), 0, 90, 0, 100);
  Serial.print("Sail position %:");
  Serial.println(sailPct);
  
  if (sailPct >= 90) { // Autopilot mode iff sail joystick is at/above 90% 
    Serial.println("In autopilot mode");
  
    // Started at startPositionX, startPositionY iff start_pos_found is true
    choose_target();
    set_rudder();
    set_sail();
  } else {
    Serial.println("In manual mode");
  }

  /********************* send commands to motors *************************/
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
  
  if (displayValues) {printToMonitor();}
  
} //end of loop()

void choose_target() {
  // TODO: Keep track of which target is next, whether to approach it on the left or right, and by how wide a margin
  target_lat = target_lats[0];
  target_lon = target_lons[0];
}

void set_rudder() {
  // Point at the target, unless that would put us in irons,
  // in which case go at a IRONS_DEG-degree angle toward-ish it
  float absolute_angle = find_absolute_angle(GPS.latitudeDegrees, GPS.longitudeDegrees, target_lat, target_lon);
  Serial.print("Want to head at angle: "); Serial.println(absolute_angle);
  Serial.print("Current heading: "); Serial.println(robosailHeading);

  float bearing = absolute_angle - robosailHeading;
  Serial.print("Desired bearing: "); Serial.println(bearing);

  float bearing_relative_to_wind = windAngle - bearing;
  Serial.print("Wind angle: "); Serial.println(windAngle);
  Serial.print("Bearing relative to wind: "); Serial.println(bearing_relative_to_wind);
  if (abs(bearing_relative_to_wind) <= IRONS_DEG) {
    rudderPosition = bearing_relative_to_wind > 0 ? bearing_relative_to_wind+IRONS_DEG : bearing_relative_to_wind-IRONS_DEG;
    rudderPosition = constrain(rudderPosition, -60, 60);
    Serial.print("Rudder position, to avoid irons: "); Serial.println(rudderPosition);
  } else {
    rudderPosition = constrain(bearing, -60, 60);
    Serial.print("Rudder position: "); Serial.println(rudderPosition);
  }
}

// Note: does not do great-circle distance, so not suitable for
// distances >= hundreds of km. But works fine for things on the
// order of tens of meters.
// This gives the graph-paper angle, i.e. 0 = to the right, 90 = up
float find_absolute_angle(float from_lat, float from_lon, float to_lat, float to_lon) {
  float lat_delta = to_lat - from_lat;
  float lon_delta = to_lon - from_lon;
  float angle;
  if (abs(lon_delta) <= 0.0001) {
    angle = lon_delta > 0 ? 0 : 180;
  } else {
    angle = atan2(lat_delta, lon_delta);    
  }
  angle = angle * 57296 / 1000; // Radians to degrees
  return angle;
}

void set_sail() {
  // abs(windAngle): IRONS_DEG...180 -> abs(sail) 0...90
  sailPosition = map(constrain(abs(windAngle), IRONS_DEG, 180), IRONS_DEG, 180, 0, 90);
  Serial.print("Autopilot sail position:");
  Serial.println(sailPosition);
  // TODO Pull it in a little and then let it out when gybing
}

void test_find_absolute_angle() {
  float angle;
  angle = find_absolute_angle(0, 0, 100, 0);
  Serial.print("North: ");
  Serial.println(angle);

  angle = find_absolute_angle(0, 0, 100, 100);
  Serial.print("Northwest: ");
  Serial.println(angle);

  angle = find_absolute_angle(10, 10, 10, 20);
  Serial.print("West: ");
  Serial.println(angle);

  angle = find_absolute_angle(10, 10, 0, 20);
  Serial.print("Southwest: ");
  Serial.println(angle);

  angle = find_absolute_angle(39, -86, 18, -86);
  Serial.print("South: ");
  Serial.println(angle);

  angle = find_absolute_angle(39, -86, 19, -66);
  Serial.print("Southeast: ");
  Serial.println(angle);

  angle = find_absolute_angle(39, -86, 39, 5);
  Serial.print("East: ");
  Serial.println(angle);

  angle = find_absolute_angle(39, -86, 40, -85);
  Serial.print("Northeast: ");
  Serial.println(angle);  
}

