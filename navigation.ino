const int num_buoys = 3;
//                        Near       Middle       Far
float buoy_lats[num_buoys] = { 42.359195,  42.359228,  42.359261}; // Mooring field
float buoy_lons[num_buoys] = {-71.073700, -71.073825, -71.073950};
//float buoy_lats[num_buoys] = { 42.360601,  42.360763,  42.360925};  // Towards bridge
//float buoy_lons[num_buoys] = {-71.073761, -71.074089, -71.074417};

const bool approach_first_buoy_from_left = true;
const int num_waypoints = 2*num_buoys + 3;
const float target_clearance_radius = .0002; // Should be about 60 feet in latitude, 45 feet in longitude (in Boston). Good enough.
const float waypoint_acquisition_clearance_squared = target_clearance_radius*target_clearance_radius/9.0;
const float dock_lat = 52.360389;
const float dock_lon = -71.073364;

float waypoint_lats[num_waypoints];
float waypoint_lons[num_waypoints];
int next_waypoint = 0;

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

void choose_target(float& target_lat, float& target_lon) {
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

float absolute_angle_to_target() {
  float angle;
  float target_lat;
  float target_lon;
  choose_target(target_lat, target_lon);
  #if GPS_EXISTS 
  readGPS();  //puts values in "start" and "relative" variable
  angle = find_absolute_angle(GPS.latitudeDegrees, GPS.longitudeDegrees, target_lat, target_lon);
#else
  angle = find_absolute_angle(dock_lat, dock_lon, target_lat, target_lon);
#endif
  return rad2deg(angle);
}

void resetProgress() {
  next_waypoint = 0;
}

