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

int twoToTenCounter = 0;
bool tenAchieved = false;


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

float Zumo32U4_bibliotek_gruppe_8::setCapacity(float speed, unsigned long ms, bool SOSmode, float currentCapacity)
{
  if (speed < 0.1)
  {
    speed = 0;  
  }

  if ((SOSmode == true) && (currentCapacity < 864000))
  {
    int multiplyer = 10;
  }
  else
  {
    multiplyer = 1;
  }
  
  if (currentCapacity > 864000)
  {
    SOSmode = false;
  }

  float currentUsage = 2.0 * multiplyer * speed + 10.0;
  currentCapacity -= currentUsage * (float)ms / 1000.0;
  return currentCapacity;
}




void Zumo32U4_bibliotek_gruppe_8::timer1OverflowCounter()
{
  unsigned long time_now = millis();
  while (millis() < time_now + 1000)
  {
      // Her går koden som skal vises på skjermen.
      // Trengs det å starte skjerm, så gjør det
      // før time_now variabelen.
  } 

  twoToTenCounter = 0;
  tenAchieved = false;
}

void Zumo32U4_bibliotek_gruppe_8::vectorOverflow()
{
    twoToTenCounter = twoToTenCounter + 1;
    if (twoToTenCounter >= 5)
    {
        tenAchieved = true;
    }
}

void Zumo32U4_bibliotek_gruppe_8::oneSecBatState()
{
    if (tenAchieved = true)
    {
        Zumo32U4_bibliotek_gruppe_8::timer1OverflowCounter();
    }
}


