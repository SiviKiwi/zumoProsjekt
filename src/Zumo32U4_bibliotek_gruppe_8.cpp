/*
 * Zumo32U4_bibliotek_gruppe_8.cpp - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
 */

#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4LCD display;

Zumo32U4_bibliotek_gruppe_8::Zumo32U4_bibliotek_gruppe_8(){
  
}

float Zumo32U4_bibliotek_gruppe_8::getDistance(int16_t encoderCount)
{
  float rotasjoner = (float)encoderCount / (75.81 * 12.0);
  float cm = rotasjoner * 12.2522;
  return cm; 
}

float Zumo32U4_bibliotek_gruppe_8::getSpeed(float distance, unsigned long ms)
{
  float cmSecond = distance / ((float)ms / 1000);
  return cmSecond; 
}

float Zumo32U4_bibliotek_gruppe_8::setCapacity(float speed, unsigned long ms)
{
  if (speed < 0.1)
  {
    speed = 0;  
  }

  float currentUsage = 2.0 * speed + 10.0;
  currentCapacity -= currentUsage * (float)ms / 1000.0;
  return currentCapacity;
}
