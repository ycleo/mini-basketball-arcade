// Include the Wire library for I2C communication and FastLED library for controlling LED light bar.
#include <Wire.h>
#include <FastLED.h>

#define LED_PIN 5
#define NUM_LEDS 100
#define BRIGHTNESS 64
#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100

// hold the color information for each LED.
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType currentBlending;

extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

// The time for triggering color changes.
unsigned long long int color_change_time = 1000;

void setup()
{
  Wire.begin(3);                // Set the I2C address to 3
  Wire.onReceive(receiveEvent); // Set up the I2C receive callback
  Serial.begin(9600);           // Initializes Serial communication

  delay(3000); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);

  // light bar style default setting
  currentPalette = RainbowColors_p; // rainbow style color
  currentBlending = LINEARBLEND;    // Linear blending: a smooth transition between colors
}

void loop()
{
  static uint8_t startIndex = 0;
  startIndex += 1; /* we can change the amount to control the lighting moving speed */

  FillLEDsFromPaletteColors(startIndex);

  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
  if (millis() - color_change_time > 1000)
  {
    currentPalette = RainbowColors_p;
    currentBlending = LINEARBLEND;
  }
}

// receive callback function
void receiveEvent(int numBytes)
{
  while (Wire.available()) // signal detecting
  {
    char c = Wire.read();
    Serial.print(c);
    SetupBlackAndWhiteStripedPalette();
    currentBlending = NOBLEND; // NOBLEND: abrupt transition from one color to another
    color_change_time = millis();
  }
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  uint8_t brightness = 255;

  for (int i = 0; i < NUM_LEDS; ++i)
  {
    leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
    colorIndex += 3;
  }
}

void SetupTotallyRandomPalette()
{
  for (int i = 0; i < 16; ++i)
  {
    currentPalette[i] = CHSV(random8(), 255, random8());
  }
}

void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid(currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;
}

const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
    {
        CRGB::Red,
        CRGB::Gray, // 'white' is too bright compared to red and blue
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Black,

        CRGB::Red,
        CRGB::Red,
        CRGB::Gray,
        CRGB::Gray,
        CRGB::Blue,
        CRGB::Blue,
        CRGB::Black,
        CRGB::Black};
