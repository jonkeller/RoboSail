//input pins from receiver
#define RUDDER_RC_PIN 2
#define SAIL_RC_PIN 3

#define RUDDER_HIGH 1900  //nominal 2000
#define RUDDER_LOW 1130   //nominal 1000
#define SAIL_HIGH 1710   //nominal 2000
#define SAIL_LOW 1110    //nominal 1000

 // Takes in the PWM signals from the RC Receiver and translate
 // them to the servo positions in degrees.
 // Takes in the PWM signals from the WindSensor and translate 
 // it to the windvane position in degrees.
void readReceiver(int &sailPosition, int &rudderPosition) {
#if RECEIVER_EXISTS
  // Read the command pulse from the RC receiver
  int rudderPulseWidth = pulseIn(RUDDER_RC_PIN, HIGH);
  int sailPulseWidth = pulseIn(SAIL_RC_PIN, HIGH);
  // Calculate the servo position in degrees.
  rudderPosition = map(rudderPulseWidth, RUDDER_LOW, RUDDER_HIGH, -60, 60);
  sailPosition = map(sailPulseWidth, SAIL_LOW, SAIL_HIGH, 0, 90);
  Serial.print("SailPulse: "); Serial.print(sailPulseWidth);
#else
  sailPosition = 90;
  rudderPosition = 0;
#endif
}

