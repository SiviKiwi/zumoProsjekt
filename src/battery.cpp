#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD display;

unsigned long time;
float speed;
float prevDist = 0;
float currentCapacity = 1200.0 * 3600;

float getDistance(int16_t encoderCount) {
  float rotasjoner = (float) encoderCount / (75.81 * 12.0);
  float cm = rotasjoner * 12.2522;
  return cm;
}

float getSpeed(float distance, unsigned long ms) {
  float cmSecond = distance / ((float) ms / 1000);
  return cmSecond;
}



float setCapacity(float speed, unsigned long ms) {

  float currentUsage = 2.0 * abs(speed) + 10.0;

  currentCapacity -= currentUsage * (float) ms / 1000.0;

  return currentCapacity;

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
    unsigned long elapsedTime = timeNow - time;
    speed = getSpeed(dist - prevDist, elapsedTime);

    setCapacity(speed, elapsedTime);

    display.clear();
    display.gotoXY(0,0);
    display.print((float) currentCapacity / 3600.0);

    time = millis();
    prevDist = dist;

  }
}
