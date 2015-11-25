
#include "FastLED.h"

#define NUM_LEDS 114 //numero de leds 
#define LED_PIN 6 // arduino pin de salida
#define GROUND_PIN 10
#define BRIGHTNESS 255 // maximum brightness
#define SPEED 115200 

CRGB leds[NUM_LEDS];
uint8_t * ledsRaw = (uint8_t *)leds;

static const uint8_t magic[] = {
  'A','d','a'};
#define MAGICSIZE  sizeof(magic)
#define HEADERSIZE (MAGICSIZE + 3)

#define MODE_HEADER 0
#define MODE_DATA   2

static const unsigned long serialTimeout = 150000; // 150 seconds

void setup()
{
  pinMode(GROUND_PIN, OUTPUT); 
  digitalWrite(GROUND_PIN, LOW);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  uint8_t
    buffer[256],
  indexIn       = 0,
  indexOut      = 0,
  mode          = MODE_HEADER,
  hi, lo, chk, i, spiFlag;
  int16_t
    bytesBuffered = 0,
  hold          = 0,
  c;
  int32_t
    bytesRemaining;
  unsigned long
    startTime,
  lastByteTime,
  lastAckTime,
  t;
  int32_t outPos = 0;

  Serial.begin(SPEED); 

  Serial.print("Ada\n"); 

    startTime    = micros();
  lastByteTime = lastAckTime = millis();

  for(;;) {

    t = millis();
    if((bytesBuffered < 256) && ((c = Serial.read()) >= 0)) {
      buffer[indexIn++] = c;
      bytesBuffered++;
      lastByteTime = lastAckTime = t; 
    } 
    else {
      if((t - lastAckTime) > 1000) {
        Serial.print("Ada\n"); 
        lastAckTime = t; 
      }
      if((t - lastByteTime) > serialTimeout) {
        memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB)); 
        FastLED.show();
        lastByteTime = t; 
      }
    }

    switch(mode) {

    case MODE_HEADER:

      if(bytesBuffered >= HEADERSIZE) {
        for(i=0; (i<MAGICSIZE) && (buffer[indexOut++] == magic[i++]););
        if(i == MAGICSIZE) {
          hi  = buffer[indexOut++];
          lo  = buffer[indexOut++];
          chk = buffer[indexOut++];
          if(chk == (hi ^ lo ^ 0x55)) {
            bytesRemaining = 3L * (256L * (long)hi + (long)lo + 1L);
            bytesBuffered -= 3;
            outPos = 0;
            memset(leds, 0,  NUM_LEDS * sizeof(struct CRGB));
            mode           = MODE_DATA;
          } 
          else {

            indexOut  -= 3; // Rewind
          }
        }
        bytesBuffered -= i;
      }
      break;

    case MODE_DATA:

      if(bytesRemaining > 0) {
        if(bytesBuffered > 0) {
          if (outPos < sizeof(leds))
            ledsRaw[outPos++] = buffer[indexOut++]; 
          bytesBuffered--;
          bytesRemaining--;
        }
      } 
      else {
        startTime  = micros();
        mode       = MODE_HEADER; 
        FastLED.show();
      }
    } // end switch
  } // end for(;;)
}

void loop()
{
}
