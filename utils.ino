float rad2deg(float rad) {
  return rad * 57296 / 1000;
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

// Squared distance, in units of latitude/longitude degrees.
// Kind of meaningless since, unless we're at the equator, latitude degrees
// are longer than longitude degrees.
float distance_squared(float lat1, float lon1, float lat2, float lon2) {
  return (lat1-lat2)*(lat1-lat2) + (lon1-lon2)*(lon1-lon2);
}

void calculate_position_from_coords_bearing_and_distance(float from_lat, float from_lon, float bearing, float distance, float* to_lat, float* to_lon) {
  *to_lat = from_lat + sin(bearing)*distance;
  *to_lon = from_lon + cos(bearing)*distance;
  //Serial.print(distance, 6); Serial.print("\t from ("); Serial.print(from_lat, 6); Serial.print(","); Serial.print(from_lon, 6); Serial.print(")\t toward "); Serial.print(bearing, 6); Serial.print("\t is ("); Serial.print(*to_lat, 6); Serial.print(","); Serial.print(*to_lon, 6); Serial.println(")");
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

