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
  #define WIND_SENSOR_EXISTS 1
  #define COMPASS_ACCEL_EXISTS 1
  #define SERVOS_EXIST 0
  #include <SoftwareSerial.h>
  // Arduino Nano pins:
  // 10DOF SDA=A4
  // 10DOF SCL=A5
#endif

const int IRONS_DEG = 45;
const float rudder_multiplier = -.5; // -0.3333333;

void setup() {
  Serial.begin(115200);
  Serial.println("In setup()");
  declarePins();
  Serial.println("\nRoboSail BoatCode - 0.04\n");  //write program name here

  digitalWrite(LED_BUILTIN, HIGH);
  initGPS();
  initCompass();
  setup_waypoints();
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Setup complete");
}

void loop() {
  //*********** Read in data from the RC receiver and sensors *********
  int sailPosition;
  int rudderPosition;
  readReceiver(sailPosition, rudderPosition);
  sailPosition = constrain(sailPosition, 0, 90);
  int sailPct = map(sailPosition, 0, 90, 0, 100);
  Serial.print("Sail: "); Serial.print(sailPosition);
  Serial.print("\tSail %:"); Serial.print(sailPct);

  int windAngle = readWind();
  Serial.print("\tWind <: "); Serial.print(windAngle);
  bool currently_in_irons = abs(windAngle) <= IRONS_DEG;
  Serial.print("\tIrons now? "); Serial.print(currently_in_irons?"IRONS!":" safe ");
  // TODO: what if in irons now?
  
  bool autosail;
  if (sailPct >= 90) { // Autopilot mode iff sail joystick is at/above 90% 
    autosail = true;
    Serial.print("\t*AUTO*");
    rudderPosition = set_rudder(windAngle);
    sailPosition = set_sail(windAngle);
  } else {
    autosail = false;
    resetProgress();
    Serial.print("\tMANUAL");
  }
  /********************* send commands to motors *************************/
#if SERVOS_EXIST
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
#endif
} //end of loop()

int prevRudderPosition = 0;
int set_rudder(int windAngle) {
  float robosailHeading = getSmoothedRobosailHeading();

  // Point at the target, unless that would put us in irons,
  // in which case go at a IRONS_DEG-degree angle toward-ish it
  float absolute_angle = absolute_angle_to_target();
  float desired_bearing = clamp_angle(absolute_angle - robosailHeading); // How much we want to turn (positive = port)
  float desired_bearing_relative_to_wind = clamp_angle(desired_bearing - windAngle);

  bool desired_bearing_would_be_irons;
  int rudderPosition;
  if (abs(desired_bearing_relative_to_wind) <= IRONS_DEG) {
    desired_bearing_would_be_irons = true;
    rudderPosition = (desired_bearing_relative_to_wind > 0 ? IRONS_DEG : -IRONS_DEG) * rudder_multiplier;
    rudderPosition = constrain(rudderPosition, -60, 60);
  } else {
    desired_bearing_would_be_irons = false;
    rudderPosition = constrain(desired_bearing*rudder_multiplier, -60, 60);
  }

  // If desired_bearing is within 5 degrees of +/-180, and previous rudderPosition was +/-60, don't flip the sign
  if (abs(desired_bearing) >= 170 && abs(prevRudderPosition) == 60 && abs(rudderPosition) == 60) {
    rudderPosition = prevRudderPosition;
  }

  Serial.print("\tCur. RoboHeading: "); Serial.print(robosailHeading); Serial.print("  ");
  Serial.print("\tAbs Heading to Tgt: "); Serial.print(absolute_angle); Serial.print("  ");
  Serial.print("\tDes. Bearing: "); Serial.print(desired_bearing);
  Serial.print("\tDes. Bearing->Wind: "); Serial.print(desired_bearing_relative_to_wind);
  Serial.print("\tDes. Irons? "); Serial.print(desired_bearing_would_be_irons?"IRONS!":" safe ");
  Serial.print("\tRudder Pos: "); Serial.println(rudderPosition);
  prevRudderPosition = rudderPosition;
  return rudderPosition;
}

int set_sail(int windAngle) {
  // abs(windAngle): IRONS_DEG...180 -> abs(sail) 0...90
  return map(constrain(abs(windAngle), IRONS_DEG, 180), IRONS_DEG, 180, 0, 90);
  // TODO Pull it in a little and then let it out when gybing
}

