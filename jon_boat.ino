/* BoatCodeStarterWind+GPS+Compass rev 7/31/2017
© 2014-2017 RoboSail
Find detailed description in Decription tab
*/
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define GPS_EXISTS 1
#define RECEIVER_EXISTS 1
#define WIND_SENSOR_EXISTS 1
#define COMPASS_ACCEL_EXISTS 1
#define SERVOS_EXIST 1
#else
#define GPS_EXISTS 0
#define RECEIVER_EXISTS 0
#define WIND_SENSOR_EXISTS 0
#define COMPASS_ACCEL_EXISTS 1
#define SERVOS_EXIST 0
#endif

#if SERVOS_EXIST
#include <Servo.h>
#endif

#include <SoftwareSerial.h>
#include <Wire.h>

#if GPS_EXISTS
#include <UsefulCalcs.h>
#include <Adafruit_GPS.h>
#endif

#if COMPASS_ACCEL_EXISTS
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#endif

#include "RoboSail.h"

boolean displayValues = false;  //true calls function for values to be printed to monitor

#if GPS_EXISTS
//Fill in min/max parameters for the RC Receiver and WindSensor in RoboSail.h tab
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
Adafruit_GPS GPS(&Serial1); // This will only work on a Mega!
#else
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);
#endif
// initialize utility that will convert lat/lon to (x,y) positions in meters
UsefulCalcs calc(false);
#endif

//                        Near       Middle       Far
float buoy_lats[3] = { 42.359195,  42.359228,  42.359261}; // Mooring field
float buoy_lons[3] = {-71.073700, -71.073825, -71.073950};
//float buoy_lats[3] = { 42.360601,  42.360763,  42.360925};  // Towards bridge
//float buoy_lons[3] = {-71.073761, -71.074089, -71.074417};
const int num_buoys = 3;
bool approach_first_buoy_from_left = true;
const int num_waypoints = 2*num_buoys + 3;
float waypoint_lats[num_waypoints];
float waypoint_lons[num_waypoints];
int next_waypoint = 0;
const float target_clearance_radius = .0002; // Should be about 60 feet in latitude, 45 feet in longitude (in Boston). Good enough.
float waypoint_acquisition_clearance_squared = target_clearance_radius*target_clearance_radius/9.0;
const float dock_lat = 42.360389;
const float dock_lon = -71.073364;

float target_lat;
float target_lon;
const int IRONS_DEG = 45;
const float rudder_multiplier = -.5; // -0.3333333;
boolean autosail;
boolean desired_bearing_would_be_irons;
float absolute_angle;
float desired_bearing;
float desired_bearing_relative_to_wind;
float current_bearing_relative_to_wind;
boolean currently_in_irons;

void setup() {
  Serial.begin(115200);
  Serial.println("In setup()");
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("\nRoboSail BoatCode - 0.02\n");  //write program name here
  // Set RC receiver and WindSensor on digital input pins
  declarePins();

#if GPS_EXISTS
  Serial.println("Setting up GPS");
  checkGPS();
#endif
#if COMPASS_ACCEL_EXISTS
  Serial.println("Setting up Compass");
  checkCompass();
#endif
  //digitalWrite(LED_BUILTIN, LOW);

  setup_waypoints();
  Serial.println("Ready to loop");
}

void setup_waypoints() {
  // Find bearing between buoy 0 and buoy 1
  float buoy_direction = find_absolute_angle(buoy_lats[0], buoy_lons[0], buoy_lats[1], buoy_lons[1]);
  Serial.print("Bearing from buoy 0 to buoy 1: "); Serial.println(buoy_direction, 6);

  // Find direction bearings (relative to "up" being buoy_direction)
  float left = buoy_direction + PI/2;
  if (left > PI) { left -= 2*PI; }
  //Serial.print("Left Bearing: "); Serial.println(left, 6);
  float right = buoy_direction - PI/2;
  if (right < -PI) { right += 2*PI; }
  //Serial.print("Right Bearing: "); Serial.println(right, 6);
  float down = buoy_direction + PI;
  if (down > PI) { down -= 2*PI; }
  //Serial.print("Down Bearing: "); Serial.println(down, 6);

  calculate_position_from_coords_bearing_and_distance(buoy_lats[0], buoy_lons[0], down, target_clearance_radius, &waypoint_lats[0], &waypoint_lons[0]);
  waypoint_lats[num_waypoints-1] = waypoint_lats[0];
  waypoint_lons[num_waypoints-1] = waypoint_lons[0];

  int next_buoy = 0;
  int dir = 1;
  bool approach_next_buoy_from_left = approach_first_buoy_from_left;
  for (int i=1; i<num_waypoints-1; ++i) {
    calculate_position_from_coords_bearing_and_distance(buoy_lats[next_buoy], buoy_lons[next_buoy], approach_next_buoy_from_left?left:right, target_clearance_radius, &waypoint_lats[i], &waypoint_lons[i]);
    approach_next_buoy_from_left = !approach_next_buoy_from_left;
    next_buoy += dir;
    if (next_buoy >= num_buoys) {
      // Turn around
      dir = -1;
      next_buoy += dir;
      ++i;
      approach_next_buoy_from_left = !approach_next_buoy_from_left;
      //Serial.println("Turning around");
      calculate_position_from_coords_bearing_and_distance(buoy_lats[next_buoy], buoy_lons[next_buoy], buoy_direction, target_clearance_radius, &waypoint_lats[i], &waypoint_lons[i]);
    }
  }
}

void calculate_position_from_coords_bearing_and_distance(float from_lat, float from_lon, float bearing, float distance, float* to_lat, float* to_lon) {
  *to_lat = from_lat + sin(bearing)*distance;
  *to_lon = from_lon + cos(bearing)*distance;
  //Serial.print(distance, 6); Serial.print("\t from ("); Serial.print(from_lat, 6); Serial.print(","); Serial.print(from_lon, 6); Serial.print(")\t toward "); Serial.print(bearing, 6); Serial.print("\t is ("); Serial.print(*to_lat, 6); Serial.print(","); Serial.print(*to_lon, 6); Serial.println(")");
}
  
void loop() {
  //Serial.println("In loop()");
  //*********** Read in data from the RC receiver and sensors *********
#if RECEIVER_EXISTS
  readReceiver();
#else
  rudderPosition = 0;
  sailPosition = 90;
#endif

#if WIND_SENSOR_EXISTS
  readWind();
#endif
#if COMPASS_ACCEL_EXISTS
  //readAccel();
  readCompassAccel(); //Read heading and tilt from the Compass
  //readCompass_Adafruit();
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
  sailPosition = constrain(sailPosition, 0, 90);
  int sailPct = map(sailPosition, 0, 90, 0, 100);
  
  if (sailPct >= 90) { // Autopilot mode iff sail joystick is at/above 90% 
    autosail = true;
  
    // Started at startPositionX, startPositionY iff start_pos_found is true
    choose_target();
    set_rudder();
    set_sail();
  } else {
    autosail = false;
  }
  /********************* send commands to motors *************************/
#if SERVOS_EXIST
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
#endif
  
  if (displayValues) {printToMonitor();}
  Serial.print("SailPulse: "); Serial.print(sailPulseWidth);
  Serial.print("\tSailpos: "); Serial.print(sailPosition);
  Serial.print("\tSailpos %:"); Serial.print(sailPct);
  Serial.print("\tMode: "); Serial.print(autosail?"*AUTO*":"MANUAL");
  Serial.print("\tAbs Heading to Tgt: "); Serial.print(absolute_angle);
  Serial.print("\tRobosailHeading: "); Serial.print(robosailHeading);
  Serial.print("\tDesired Bearing: "); Serial.print(desired_bearing);
  Serial.print("\tWind Angle: "); Serial.print(windAngle);
  Serial.print("\tIrons now? "); Serial.print(currently_in_irons?"IRONS!":" safe ");
  Serial.print("\tDes. Bearing->Wind: "); Serial.print(desired_bearing_relative_to_wind);
  Serial.print("\tDes. Bearing->Irons? "); Serial.print(desired_bearing_would_be_irons?"IRONS!":" safe ");
  Serial.print("\tRudder Pos: "); Serial.println(rudderPosition);
} //end of loop()

void choose_target() {
  if (distance_squared(
    waypoint_lats[next_waypoint], waypoint_lons[next_waypoint],
#if GPS_EXISTS 
    GPS.latitudeDegrees, GPS.longitudeDegrees
#else
    dock_lat, dock_lon
#endif
    ) < waypoint_acquisition_clearance_squared) {
    ++next_waypoint;
  }
  if (next_waypoint < num_waypoints) {
    target_lat = waypoint_lats[next_waypoint];
    target_lon = waypoint_lons[next_waypoint];
  } else {
    target_lat = dock_lat;
    target_lon = dock_lon;
  }
}

// Squared distance, in units of latitude/longitude degrees.
// Kind of meaningless since, unless we're at the equator, latitude degrees
// are longer than longitude degrees.
float distance_squared(float lat1, float lon1, float lat2, float lon2) {
  return (lat1-lat2)*(lat1-lat2) + (lon1-lon2)*(lon1-lon2);
}

void set_rudder() {
  // Point at the target, unless that would put us in irons,
  // in which case go at a IRONS_DEG-degree angle toward-ish it
  absolute_angle = find_absolute_angle(
#if GPS_EXISTS 
    GPS.latitudeDegrees, GPS.longitudeDegrees,
#else
    dock_lat, dock_lon,
#endif
    target_lat, target_lon);
  absolute_angle = rad2deg(absolute_angle);

  desired_bearing = clamp_angle(absolute_angle - robosailHeading);

  desired_bearing_relative_to_wind = clamp_angle(desired_bearing - windAngle);
  // TODO: what if in irons now?
  current_bearing_relative_to_wind = robosailHeading - windAngle;
  currently_in_irons = abs(current_bearing_relative_to_wind) <= IRONS_DEG;
  if (abs(desired_bearing_relative_to_wind) <= IRONS_DEG) {
    desired_bearing_would_be_irons = true;
    rudderPosition = (desired_bearing_relative_to_wind > 0 ? IRONS_DEG : -IRONS_DEG) * rudder_multiplier;
    //rudderPosition = constrain(rudderPosition, -60, 60);
  } else {
    desired_bearing_would_be_irons = false;
    rudderPosition = constrain(desired_bearing*rudder_multiplier, -60, 60);
  }
}

float clamp_angle(float angle) {
  while (angle > 180.0) {
    angle -= 360.0;
  }
  while (angle < -180.0) {
    angle += 360.0;
  }
  return angle;
}

// Note: does not do great-circle distance, so not suitable for
// distances >= hundreds of km. But works fine for things on the
// order of tens of meters.
// This gives the graph-paper angle, i.e. 0 = to the right, PI/2 = up
float find_absolute_angle(float from_lat, float from_lon, float to_lat, float to_lon) {
  //Serial.print("Finding angle from ("); Serial.print(from_lat, 6); Serial.print(","); Serial.print(from_lon, 6); Serial.print(") -> ("); Serial.print(to_lat, 6); Serial.print(","); Serial.print(to_lon, 6); Serial.println(")");
  float lat_delta = to_lat - from_lat;
  float lon_delta = to_lon - from_lon;
  //Serial.print("lat_delta: "); Serial.print(lat_delta, 6); Serial.print("\tlon_delta: "); Serial.println(lon_delta, 6);
  float angle = atan2(lat_delta, lon_delta);    
  //Serial.print("Radians: "); Serial.println(angle, 6);
  return angle;
}

void set_sail() {
  // abs(windAngle): IRONS_DEG...180 -> abs(sail) 0...90
  sailPosition = map(constrain(abs(windAngle), IRONS_DEG, 180), IRONS_DEG, 180, 0, 90);
  // TODO Pull it in a little and then let it out when gybing
}

float rad2deg(float rad) {
  return rad * 57296 / 1000;
}

/*
float deg2rad(float deg) {
  return deg * 1000 / 57296;
}
*/


