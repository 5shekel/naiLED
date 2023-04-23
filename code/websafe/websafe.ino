#include <Arduino.h>
#include "webSafeColors.h"

const int redPin = 9;
const int greenPin = 10;
const int bluePin = 11;
const int buttonRecordPin = 2;
const int buttonPlaybackPin = 3;


uint8_t recordedColors[50];
uint8_t recordedColorsCount = 0;

uint8_t getClosestWebSafeColorIndex(uint8_t r, uint8_t g, uint8_t b) {
  uint8_t closestColorIndex = 0;
  uint32_t minDistance = UINT32_MAX;

  for (uint16_t i = 0; i < 216; i++) {
    uint8_t wsR = pgm_read_byte(&(webSafeColors[i].r));
    uint8_t wsG = pgm_read_byte(&(webSafeColors[i].g));
    uint8_t wsB = pgm_read_byte(&(webSafeColors[i].b));

    uint32_t distance = sq(r - wsR) + sq(g - wsG) + sq(b - wsB);

    if (distance < minDistance) {
      minDistance = distance;
      closestColorIndex = i;
    }
  }

  return closestColorIndex;
}


void setRGBLEDColor(const RGBColor &color) {
  analogWrite(redPin, color.r);
  analogWrite(greenPin, color.g);
  analogWrite(bluePin, color.b);
}

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  pinMode(buttonRecordPin, INPUT_PULLUP);
  pinMode(buttonPlaybackPin, INPUT_PULLUP);

  Serial.begin(9600);
}
void loop() {
  static uint32_t lastMillis = 0;
  if (millis() - lastMillis >= 100) {
    lastMillis = millis();

    RGBColor randomColor = {random(256), random(256), random(256)};
    uint8_t webSafeColorIndex = getClosestWebSafeColorIndex(randomColor.r, randomColor.g, randomColor.b);
    RGBColor webSafeColor = webSafeColors[webSafeColorIndex];
    setRGBLEDColor(webSafeColor);

    char inputColorStr[16];
    sprintf(inputColorStr, "(%d, %d, %d)", randomColor.r, randomColor.g, randomColor.b);
    char outputColorStr[16];
    sprintf(outputColorStr, "(%d, %d, %d)", webSafeColor.r, webSafeColor.g, webSafeColor.b);
    Serial.print("Input Color: ");
    Serial.print(inputColorStr);
    Serial.print("\tOutput Color: ");
    Serial.println(outputColorStr);

    if (digitalRead(buttonRecordPin) == LOW) {
      if (recordedColorsCount < 50) {
        recordedColors[recordedColorsCount++] = webSafeColorIndex;
      }
    }

    if (digitalRead(buttonPlaybackPin) == LOW) {
      for (uint8_t i = 0; i < recordedColorsCount; i++) {
        setRGBLEDColor(webSafeColors[recordedColors[i]]);
        delay(500);
      }
    }
  }
}
