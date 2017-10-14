#if COMPASS_ACCEL_EXISTS
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>
#endif

// Compass (magnetometer and accelerometer variables
// These values will need to be adjusted based on your particular compass.
// Use compassCalibration (in the Orientation library) to determine the correct hard iron calibration.
// Jon's LSM303:
const float hardiron_x = 17.82;
const float hardiron_y = -14.263;
const float hardiron_z = 2.685;

// Source: http://www.ngdc.noaa.gov/geomag-web/#igrfwmm
const float declination = -14.6375;

#if COMPASS_ACCEL_EXISTS
/* Assign a ID to Accelerometer and Magnetometer and define event */
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(54321);
sensors_event_t event;
#endif

float readCompassAccel() {
#if COMPASS_ACCEL_EXISTS
  float ax, ay, az, mx, my, mz;
  float my_adj, mx_adj;
  float Pi = 3.1415926;
  float pitch = 0;  //value in degrees, converted from raw data
  float roll = 0;   //value in degrees, converted from raw data
  float yaw = 0;    //value in degrees, converted from raw data

   /* Get a new sensor event */
  sensors_event_t accel_event;
  accel.getEvent(&accel_event);
  sensors_event_t mag_event;
  mag.getEvent(&mag_event);

  /* Invert X so that when when X, Y, or Z is pointed down, it has a positive reading. */
  ax = -accel_event.acceleration.x;
  ay = accel_event.acceleration.y;
  az = accel_event.acceleration.z;
  
  /* Adjust for hard iron effects */
  mx = mag_event.magnetic.x - hardiron_x;
  my = mag_event.magnetic.y - hardiron_y;
  mz = mag_event.magnetic.z - hardiron_z;

  /* Invert Y and Z axis so that when X, Y, or Z is pointed towards Magnetic North they get a positive reading. */
  my = -my;
  mz = -mz;
  
  roll = atan2(ay,az);
  pitch = atan(-ax/sqrt(pow(ay,2)+pow(az,2)));
  
  my_adj = mz*sin(roll) - my*cos(roll);
  mx_adj = (mx*cos(pitch) + my*sin(pitch)*sin(roll) + mz*sin(pitch)*cos(roll));
  
  yaw = atan2(my_adj,mx_adj);
  
  roll = roll * 180/Pi;
  pitch =  pitch * 180/Pi;
  yaw = yaw * 180/Pi;

  //define roll for RoboSail as rolling to Port side is positive, rolling to Starboard is negative
  //float robosailRoll  = -1 * roll;

  float heading = yaw + declination;
  return clamp_angle(heading);
#endif
}

float getRobosailHeading() {
  float heading = readCompassAccel();
  // The heading is converted to a frame of reference for RoboSail:
  // East is 0 degrees, North is 90 deg, West is 180 deg, South is 270 deg. 
  float robosailHeading = (360 - heading) + 90;
  return clamp_angle(robosailHeading);
}

const int num_smooth_points = 15;
float headings[num_smooth_points];
int current_smooth_idx = 0;
float getSmoothedRobosailHeading() {
  headings[current_smooth_idx++] = getRobosailHeading();
  current_smooth_idx %= num_smooth_points;
  float smoothed_heading = 0.0;
  for (int i=0; i<num_smooth_points; ++i) {
    smoothed_heading += (headings[i]/num_smooth_points);
  }
  return smoothed_heading;
}

// Set up Compass and check that it is connected
void initCompass() {
#if COMPASS_ACCEL_EXISTS
  Serial.println("Setting up Compass");
  mag.enableAutoRange(true);
  if(!mag.begin() || !accel.begin()) {
    Serial.println("No LSM303 Compass detected ... Check your wiring!");
    while(1);
  }
#endif
  for (int i=0; i<num_smooth_points; ++i) {
    getSmoothedRobosailHeading();
  }
}

