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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.println("\nRoboSail BoatCode - 0.03\n");  //write program name here
  // Set RC receiver and WindSensor on digital input pins
  declarePins();

  initGPS();
  checkCompass();
  digitalWrite(LED_BUILTIN, LOW);

  setup_waypoints();
  Serial.println("Ready to loop");
}

void loop() {
  //*********** Read in data from the RC receiver and sensors *********
  int sailPosition;
  int rudderPosition;
  readReceiver(sailPosition, rudderPosition);
  sailPosition = constrain(sailPosition, 0, 90);
  int sailPct = map(sailPosition, 0, 90, 0, 100);

  int windAngle = readWind();
  Serial.print("\tWind Angle: "); Serial.print(windAngle);
  Serial.print(" Sail: "); Serial.print(sailPosition);
  Serial.print("\tSail %:"); Serial.print(sailPct);
  
  bool autosail;
  if (sailPct >= 90) { // Autopilot mode iff sail joystick is at/above 90% 
    autosail = true;
    Serial.print("*AUTO*");
    rudderPosition = set_rudder(windAngle);
    sailPosition = set_sail(windAngle);
  } else {
    autosail = false;
    resetProgress();
    Serial.print("MANUAL");
  }
  /********************* send commands to motors *************************/
#if SERVOS_EXIST
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
#endif
} //end of loop()

int set_rudder(int windAngle) {
  float heading = 0;
  float robosailHeading;
  readCompassAccel(heading, robosailHeading); //Read heading and tilt from the Compass

  float current_bearing_relative_to_wind = robosailHeading - windAngle;
  bool currently_in_irons = abs(current_bearing_relative_to_wind) <= IRONS_DEG;
  Serial.print("\tIrons now? "); Serial.print(currently_in_irons?"IRONS!":" safe ");

  // Point at the target, unless that would put us in irons,
  // in which case go at a IRONS_DEG-degree angle toward-ish it
  float absolute_angle = absolute_angle_to_target();
  float desired_bearing = clamp_angle(absolute_angle - robosailHeading);
  float desired_bearing_relative_to_wind = clamp_angle(desired_bearing - windAngle);

  // TODO: what if in irons now?

  bool desired_bearing_would_be_irons;
  float rudderPosition;
  if (abs(desired_bearing_relative_to_wind) <= IRONS_DEG) {
    desired_bearing_would_be_irons = true;
    rudderPosition = (desired_bearing_relative_to_wind > 0 ? IRONS_DEG : -IRONS_DEG) * rudder_multiplier;
    rudderPosition = constrain(rudderPosition, -60, 60);
  } else {
    desired_bearing_would_be_irons = false;
    rudderPosition = constrain(desired_bearing*rudder_multiplier, -60, 60);
  }

  Serial.print("\tAbs Heading to Tgt: "); Serial.print(absolute_angle);
  Serial.print("\tCurrent R_Heading: "); Serial.print(robosailHeading);
  Serial.print("\tDesired Bearing: "); Serial.print(desired_bearing);
  Serial.print("\tDes. Bearing->Wind: "); Serial.print(desired_bearing_relative_to_wind);
  Serial.print("\tDes. Irons? "); Serial.print(desired_bearing_would_be_irons?"IRONS!":" safe ");
  Serial.print("\tRudder Pos: "); Serial.println(rudderPosition);
  return rudderPosition;
}

int set_sail(int windAngle) {
  // abs(windAngle): IRONS_DEG...180 -> abs(sail) 0...90
  return map(constrain(abs(windAngle), IRONS_DEG, 180), IRONS_DEG, 180, 0, 90);
  // TODO Pull it in a little and then let it out when gybing
}

