#if NEOPIXELS_EXIST
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include "pins.h"

const int numLeds = 24;
const int offset = 10;

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(numLeds, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
#endif

void neopixelSetup() {
#if NEOPIXELS_EXIST
  // This is for Trinket 5V 16MHz, you can remove these three lines if you are not using a Trinket
  #if defined (__AVR_ATtiny85__)
    if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
  #endif
  // End of trinket special code


  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
#endif
}

void display(int windAngle, int sailPosition, int desiredBearing, int rudderPosition) {
#if NEOPIXELS_EXIST
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
  strip.setPixelColor(toLedIndex(windAngle > 0 ? -sailPosition : sailPosition), strip.Color(255, 255, 255)); // Sheet = White
  strip.setPixelColor(toLedIndex(windAngle), strip.Color(0, 0, 255));        // Wind = Blue
  strip.setPixelColor(toLedIndex(desiredBearing), strip.Color(0, 255, 0));  // Bearing = Green
  strip.setPixelColor(toLedIndex(180+rudderPosition), strip.Color(255, 0, 0));   // Rudder = Red
  // TODO: Blend colors if they are the same LED
  strip.show();
#endif
}

int toLedIndex(int angle) {
  int led = (float)clampAngle(-angle) + 180.0;
  led *= ((float)numLeds / 360.0);
  if (led < 0) {
    led = 0;
  }
  if (led >= numLeds) {
    led = numLeds - 1;
  }
  led += offset;
  led %= numLeds;
  return led;
}

