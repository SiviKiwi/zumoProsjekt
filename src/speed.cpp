#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD display;

unsigned long time;
float speed;
float prevDist = 0;

float getDistance(int16_t encoderCount) {
  float rotasjoner = (float) encoderCount / (75.81 * 12.0);
  float cm = rotasjoner * 12.2522;
  return cm;
}

float getSpeed(float distance, unsigned long ms) {
  float cmSecond = distance / ((float) ms / 1000);
  return cmSecond;
}


void setup() {

  display.init();

  time = millis();
}

void loop() {

  int16_t countLeft = encoders.getCountsLeft();
  int16_t countRight = encoders.getCountsRight();

  int16_t avgCount = (countLeft + countRight) / 2;

  float dist = getDistance(avgCount);

  unsigned long timeNow = millis();

  if (timeNow - time > 100) {
    speed = getSpeed(dist - prevDist, timeNow - time);
    display.clear();
    display.gotoXY(0,0);
    display.print(speed);

    time = millis();
    prevDist = dist;

  }
}


// Jeg heter iver. Haha. Hallo Sivert. Med stor s. n f