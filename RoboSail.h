// ****** Find the values for the transmitter (using program RCReader) 
// ****** and WindSensor (using WindSensorTest)
// ****** and change the values below as needed
#define RUDDER_HIGH 1900  //nominal 2000
#define RUDDER_LOW 1100   //nominal 1000
#define SAIL_HIGH 2000   //nominal 2000
#define SAIL_LOW 1000    //nominal 1000
#define WIND_HIGH 1023   //nominal 1023

// Pin assignments
#define WIND_PIN 7
//input pins from receiver
#define RUDDER_RC_PIN 2
#define SAIL_RC_PIN 3
// Output pins to the servos
#define RUDDER_SERVO_PIN 8
#define SAIL_SERVO_PIN 9
// variables to hold input and output values
int rudderPulseWidth;
int rudderServoOut;
int sailPulseWidth;
int sailServoOut;
//variables for WindSensor
int windAngle = 0;
int windPulseWidth = 0;
int rudderPosition = 0;
int sailPosition = 45;

#if SERVOS_EXIST
//create servo objects
Servo rudderServo;
Servo sailServo;
#endif

// Compass (magnetometer and accelerometer variables
// These values will need to be adjusted based on your particular compass.
// Use compassCalibration (in the Orientation library) to determine the correct hard iron calibration.
float hardiron_x = -11.91;
float hardiron_y = 5.05;
float hardiron_z = -1.02;

float pitchAccel = 0; //raw accelerometer value
float rollAccel = 0;  //raw accelerometer value
float yawAccel = 0;   //raw accelerometer value
float robosailRollAccel = 0;  // converted so Port lean is positive
float pitch = 0;  //value in degrees, converted from raw data
float roll = 0;   //value in degrees, converted from raw data
float yaw = 0;    //value in degrees, converted from raw data

float heading;  //value in degrees
float robosailHeading;  //value in degrees, converted to RoboSail FrameofRef
float robosailRoll;   // converted so Port lean is positive

// Source: http://www.ngdc.noaa.gov/geomag-web/#igrfwmm
float declination = -14.6067;

#if SENSORS_EXIST
/* Assign a ID to Accelerometer and Magnetometer and define event */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
sensors_event_t event;
#endif

#if GPS_EXISTS
//variables for GPS
// this will be false until GPS fix is found and starting position saved
bool start_pos_found = false;
int GPSfix;
int GPSqual;
int GPSsat;
// once GPS fix is found, these variables will be updated
float startPositionX = 0;
float startPositionY = 0;
float relPositionX = 0;
float relPositionY = 0;
float XYpos[2]; // this is xy position - it is in a 2 place array for x, y
float angleFromStart = 0;
#endif

