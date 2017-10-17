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
  #define NEOPIXELS_EXIST 0
#else
  #define GPS_EXISTS 0
  #define RECEIVER_EXISTS 0
  #define WIND_SENSOR_EXISTS 1
  #define COMPASS_ACCEL_EXISTS 1
  #define SERVOS_EXIST 0
  #define NEOPIXELS_EXIST 1
  #if SERVOS_EXIST
  #include <SoftwareSerial.h>
  #endif
  // Arduino Nano pins:
  // 10DOF SDA=A4
  // 10DOF SCL=A5
#endif

const int IRONS_DEG = 45;
const float rudderMultiplier = 1; // 0.3333333;

void setup() {
  Serial.begin(115200);
  Serial.println("In setup()");
  declarePins();
  Serial.println("\nRoboSail BoatCode - 0.04\n");  //write program name here

  digitalWrite(LED_BUILTIN, HIGH);
  initGPS();
  initCompass();
  setupWaypoints();
  neopixelSetup();
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
  bool currentlyInIrons = abs(windAngle) <= IRONS_DEG;
  Serial.print("\tIrons now? "); Serial.print(currentlyInIrons?"IRONS!":" safe ");
  // TODO: what if in irons now?

  int desiredBearing = 0;
  bool autosail;
  if (sailPct >= 90) { // Autopilot mode iff sail joystick is at/above 90% 
    autosail = true;
    Serial.print("\t*AUTO*");
    rudderPosition = setRudder(windAngle, desiredBearing);
    sailPosition = setSail(windAngle);
  } else {
    autosail = false;
    resetProgress();
    Serial.print("\tMANUAL");
  }

  /********************* send commands to motors *************************/
  driveSailServo(sailPosition);
  driveRudderServo(rudderPosition);
  display(windAngle, sailPosition, desiredBearing, rudderPosition);
} //end of loop()

int prevRudderPosition = 0;
int setRudder(int windAngle, int& desiredBearing) {
  float robosailHeading = getSmoothedRobosailHeading();

  // Point at the target, unless that would put us in irons,
  // in which case go at a IRONS_DEG-degree angle toward-ish it
  float absoluteAngle = absoluteAngleToTarget();
  desiredBearing = clampAngle(absoluteAngle - robosailHeading); // How much we want to turn (positive = port)
  float desiredBearingRelativeToWind = clampAngle(desiredBearing - windAngle);

  bool desiredBearingWouldBeIrons;
  int rudderPosition;
  if (abs(desiredBearingRelativeToWind) <= IRONS_DEG) {
    desiredBearingWouldBeIrons = true;
    // TODO: Stabilize this so it doesn't jitter between 45 and -45 so often
    if (abs(prevRudderPosition) == IRONS_DEG && abs(desiredBearingRelativeToWind) < 5) {
      rudderPosition = prevRudderPosition;
    } else {
      rudderPosition = (desiredBearingRelativeToWind > 0 ? -IRONS_DEG : IRONS_DEG) * rudderMultiplier;
    }
    rudderPosition = constrain(rudderPosition, -60, 60);
  } else {
    desiredBearingWouldBeIrons = false;
    rudderPosition = constrain(desiredBearing*-1*rudderMultiplier, -60, 60);
  }

  // If desiredBearing is within 5 degrees of +/-180, and previous rudderPosition was +/-60, don't flip the sign
  if (abs(desiredBearing) >= 170 && abs(prevRudderPosition) == 60 && abs(rudderPosition) == 60) {
    rudderPosition = prevRudderPosition;
  }

  Serial.print("\tCur. RoboHeading: "); Serial.print(robosailHeading); Serial.print("  ");
  Serial.print("\tAbs Heading to Tgt: "); Serial.print(absoluteAngle); Serial.print("  ");
  Serial.print("\tDes. Bearing: "); Serial.print(desiredBearing);
  Serial.print(" \tDes. Bearing->Wind: "); Serial.print(desiredBearingRelativeToWind);
  Serial.print("\tDes. Irons? "); Serial.print(desiredBearingWouldBeIrons?"IRONS!":" safe ");
  Serial.print("\tRudder Pos: "); Serial.println(rudderPosition);
  prevRudderPosition = rudderPosition;
  return rudderPosition;
}

int setSail(int windAngle) {
  // abs(windAngle): IRONS_DEG...180 -> abs(sail) 0...90
  return map(constrain(abs(windAngle), IRONS_DEG, 180), IRONS_DEG, 180, 0, 90);
  // TODO Pull it in a little and then let it out when gybing
}

