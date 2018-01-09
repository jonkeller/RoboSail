#if GPS_EXISTS
#include <UsefulCalcs.h>
#include <Adafruit_GPS.h>
#endif

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

// Set Up GPS and wait for fix on position
void initGPS() {
#if GPS_EXISTS
  Serial.println("Setting up GPS");
  GPS.begin(9600);  //default baud rate for Adafruit MTK GPS's
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCONLY);  //setting for minimum recommended data
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);  //update rate is 1 Hz
  enableInterrupt(); // activate TIMER0 interrupt, goes off every 1 msec
  while (!readGPS()) {  //loop code will not start until GPS is ready
    ;
  }
#endif
}

#if GPS_EXISTS
// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  GPS.read(); // reads char (if available) into internal buffer in GPS object
}

// function to enable TIMER0 interrupt for GPS
void enableInterrupt() {
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);
}
#endif

// Gets GPS data, parses it, and returns (x,y) position in meters in array called pos[]
bool readGPS() {
  bool start_pos_found = false; // this will be false until GPS fix is found and starting position saved
#if GPS_EXISTS
  // once GPS fix is found, these variables will be updated
  float startPositionX = 0;
  float startPositionY = 0;
  float relPositionX = 0;
  float relPositionY = 0;
  float XYpos[2]; // this is xy position - it is in a 2 place array for x, y
  float angleFromStart = 0;
  if (GPS.newNMEAreceived())
  {
    char* LastNMEA; // declare pointer to GPS data
    LastNMEA = GPS.lastNMEA(); // read the string and set the newNMEAreceived() flag to false
    if (!GPS.parse(LastNMEA)) 
    {
      return; // failed to parse a sentence (was likely incomplete) so just wait for another
    }
    Serial.println("\nNew data from GPS");
    int GPSfix = GPS.fix;  //put parsed data in variables for printing
    int GPSqual = GPS.fixquality;
    int GPSsat = GPS.satellites;
    if (GPS.fix)
    {
      if (start_pos_found)
      {
        // take in lat/lon degree values and return (x,y) in meters in XYpos array
        calc.latLonToUTM(GPS.latitudeDegrees, GPS.longitudeDegrees, XYpos);
        
        // calculate the boat position relative to where it was started
        relPositionX = XYpos[0] - startPositionX;
        relPositionY = XYpos[1] - startPositionY;
        angleFromStart = atan2(relPositionY, relPositionX) * 180 / 3.14;
        angleFromStart = clamp_angle(angleFromStart);
      }
      else // starting position not yet found but there is a fix
      { 
        // take in lat/lon degree values and return (x,y) in meters in pos array
        calc.latLonToUTM(GPS.latitudeDegrees, GPS.longitudeDegrees, XYpos);
        startPositionX = XYpos[0];
        startPositionY = XYpos[1];
        
        Serial.println("Starting position found!");
        Serial.print("x = "); Serial.print(startPositionX);
        Serial.print("   y = "); Serial.println(startPositionY);
        Serial.println();
        
        start_pos_found = true;
      }
    }
  }
#endif
  return start_pos_found;
}

