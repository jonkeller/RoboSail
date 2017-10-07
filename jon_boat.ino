/* BoatCodeStarterWind+GPS+Compass rev 7/31/2017
© 2014-2017 RoboSail
Find detailed description in Decription tab
*/
#define GPS_EXISTS 1
#define RECEIVER_EXISTS 1
#define SENSORS_EXIST 1
#define SERVOS_EXIST 1

#if SERVOS_EXIST
#include <Servo.h>
#endif

#include <SoftwareSerial.h>
#include <Wire.h>

#if GPS_EXISTS
#include <UsefulCalcs.h>
#include <Adafruit_GPS.h>
#endif

#if SENSORS_EXIST
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#endif

#include "RoboSail.h"

boolean displayValues = true;  //true calls function for values to be printed to monitor

#if GPS_EXISTS
//Fill in min/max parameters for the RC Receiver and WindSensor in RoboSail.h tab
Adafruit_GPS GPS(&Serial);
// initialize utility that will convert lat/lon to (x,y) positions in meters
UsefulCalcs calc(false);
#endif

//                        Near       Middle       Far
float buoy_lats[3] = { 42.360601,  42.360763,  42.360925};
float buoy_lons[3] = {-71.073761, -71.074089, -71.074417};
const int num_buoys = 3;
int next_buoy = 0;
bool approach_on_left = true;
bool ascending_buoys = true; // ...e.g. true if we're going in the dirction of 0->1->2, false if we're coming back.
float target_clearance_radius = .00006; // Should be about 21 feet in latitude, 15 feet in longitude (in Boston). Good enough.
float dock_lat = 42.360389;
float dock_lon = -71.073364;

float target_lat;
float target_lon;
int IRONS_DEG = 45;

void setup() {
  Serial.begin(115200);
  Serial.println("In setup()");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
#if SENSORS_EXIST
  accel.begin();
  mag.begin();
#endif

  Serial.println("\nRoboSail BoatCode - 0.02\n");  //write program name here
  // Set RC receiver and WindSensor on digital input pins
  declarePins();

#if GPS_EXISTS
  Serial.println("Setting up GPS");
  checkGPS();
#endif
#if SENSORS_EXIST
  Serial.println("Setting up Compass");
  checkCompass();
#endif
  Serial.println("Ready to loop");
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  Serial.println("In loop()");
  //*********** Read in data from the RC receiver and sensors *********
#if RECEIVER_EXISTS
  readReceiver();
#else
  rudderPosition = 0;
  sailPosition = 90;
#endif

#if SENSORS_EXIST
  readWind();
  readAccel();
  readCompassAccel(); //Read heading and tilt from the Compass
#endif
#if GPS_EXISTS
  readGPS();  //puts values in "start" and "relative" variable
#endif

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
#if SERVOS_EXIST
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
#endif
  
  if (displayValues) {printToMonitor();}
  
} //end of loop()

void choose_target() {
  /* Option 1:
  if (passed_buoy) {
    approach_on_left = !approach_on_left
    if (ascending_buoys) {
      if (next_buoy < num_buoys-1) {
        ++next_buoy;
      } else {
        ascending_buoys = false;
        // Target should be top, then left/right
      }
    } else {
      if (next_buoy > 0) {
        --next_buoy;
      } else {
        ascending_buoys = true;
        // TODO: Handle if it's time to stop
        // Target should be bottom, then left/right
      }
    }
  }
  */
  /* Option 2:
   * Set up (num_buoys*2 + 1) waypoints, go through them in sequence
   */
  target_lat = buoy_lats[next_buoy];
  target_lon = buoy_lons[next_buoy];
}

void set_rudder() {
  // Point at the target, unless that would put us in irons,
  // in which case go at a IRONS_DEG-degree angle toward-ish it
  float absolute_angle = find_absolute_angle(
#if GPS_EXISTS 
    GPS.latitudeDegrees, GPS.longitudeDegrees,
#else
    dock_lat, dock_lon,
#endif
    target_lat, target_lon);
  Serial.print("Want to head at angle (degrees, East=0): "); Serial.println(absolute_angle);
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
  Serial.print("Finding angle from ("); Serial.print(from_lat, 6); Serial.print(","); Serial.print(from_lon, 6); Serial.print(") -> ("); Serial.print(to_lat, 6); Serial.print(","); Serial.print(to_lon, 6); Serial.println(")");
  float lat_delta = to_lat - from_lat;
  float lon_delta = to_lon - from_lon;
  Serial.print("lat_delta: "); Serial.print(lat_delta, 6); Serial.print("\tlon_delta: "); Serial.println(lon_delta, 6);
  float angle = atan2(lat_delta, lon_delta);    
  Serial.print("Radians: "); Serial.println(angle, 6);
  angle = angle * 57296 / 1000; // Radians to degrees
  Serial.print("Degrees: "); Serial.println(angle);
  return angle;
}

void set_sail() {
  // abs(windAngle): IRONS_DEG...180 -> abs(sail) 0...90
  sailPosition = map(constrain(abs(windAngle), IRONS_DEG, 180), IRONS_DEG, 180, 0, 90);
  Serial.print("Autopilot sail position:");
  Serial.println(sailPosition);
  // TODO Pull it in a little and then let it out when gybing
}

