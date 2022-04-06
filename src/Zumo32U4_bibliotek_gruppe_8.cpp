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
            // TODO: Endre funksjonen slik at hvis farten er positiv skal den ikkke lades ut 
            // ti ganger raskere.
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

  Zumo32U4_bibliotek_gruppe_8::twoToTenCounter = 0;
  Zumo32U4_bibliotek_gruppe_8::tenAchieved = false;
}

void Zumo32U4_bibliotek_gruppe_8::vectorOverflow()
{
    Zumo32U4_bibliotek_gruppe_8::twoToTenCounter = Zumo32U4_bibliotek_gruppe_8::twoToTenCounter + 1;
    if (Zumo32U4_bibliotek_gruppe_8::twoToTenCounter >= 5)
    {
        Zumo32U4_bibliotek_gruppe_8::tenAchieved = true;
    }
}

void Zumo32U4_bibliotek_gruppe_8::oneSecBatState()
{
    if (Zumo32U4_bibliotek_gruppe_8::tenAchieved = true)
    {
        Zumo32U4_bibliotek_gruppe_8::timer1OverflowCounter();
    }
}

void Zumo32U4_bibliotek_gruppe_8::findSekstiSekTid(float speed)
{

    if (Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerFor == 0)
    {
        Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerEtter = 0;
    }

    else 
    {
        Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerEtter = millis();   
    }

    if (speed > 0.1)
    {
        Zumo32U4_bibliotek_gruppe_8::sekstiSekTimer = Zumo32U4_bibliotek_gruppe_8::sekstiSekTimer + (Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerEtter - Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerFor);
        Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerFor = millis();
    }

    else if (speed < 0.1)
    {
        Zumo32U4_bibliotek_gruppe_8::sekstiSekTimer = Zumo32U4_bibliotek_gruppe_8::sekstiSekTimer + (Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerEtter - Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerFor);
        Zumo32U4_bibliotek_gruppe_8::sekstiSekTimerFor = 0;
    }
}

void Zumo32U4_bibliotek_gruppe_8::speedometerEvery60(float speed)
{

if (Zumo32U4_bibliotek_gruppe_8::SekstiSekTimer > 60000)
    {
        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


        if (speed > Zumo32U4_bibliotek_gruppe_8::sekstiSekMaksHastighet)
        {                 // Her henter jeg inn makshastigheten som skal vises etter seksti sekunder.
            Zumo32U4_bibliotek_gruppe_8::sekstiSekMaksHastighet = speed; // VIKTIG
        }


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
        

        Zumo32U4_bibliotek_gruppe_8::GjennomsnittsHastighet = (sekstiSekunderDist - prevSekstiSekunderDist)/60; // VIKTIG Denne er ikke ferdig!!!
        prevSekstiSekunderDist = sekstiSekunderDist; // sekstiSekunderDist trenger en verdi.


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


        if (speed > Zumo32U4_bibliotek_gruppe_8::maksHastiget)
        {
            Zumo32U4_bibliotek_gruppe_8::tid70 = millis();
        }

        if (speed < Zumo32U4_bibliotek_gruppe_8::maksHastiget)
        {
            Zumo32U4_bibliotek_gruppe_8::tid70Etter = millis();
            Zumo32U4_bibliotek_gruppe_8::tid70Differensial = tid70Etter - tid70; // VIKTIG
        }


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    Zumo32U4_bibliotek_gruppe_8::sekstiSekTimer = 0;

    }
}

void distance()  // Denne funksjonen erstatter distanse koden. 
{
  int16_t countLeft = encoders.getCountsLeft();
  int16_t countRight = encoders.getCountsRight();

  int16_t avgCount = (countLeft + countRight) / 2;

  Zumo32U4_bibliotek_gruppe_8::prevDist = Zumo32U4_bibliotek_gruppe_8::dist;
  Zumo32U4_bibliotek_gruppe_8::dist = Zumo32U4_bibliotek_gruppe_8::getDistance(avgCount);
}

/* Disse funksjonene fungerer nok ikke pågrunn av mangel på constructors.*/

