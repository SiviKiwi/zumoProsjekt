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
Zumo32U4ButtonB buttonB;
Zumo32U4LCD display;


Zumo32U4_bibliotek_gruppe_8::Zumo32U4_bibliotek_gruppe_8(){

  this->twoToTenCounter = 0;
  this->tenAchieved;
  this->lastTimeGetSpeed = 0;

  this->SOSmode;
  this->SOSmodeOneTimeOnly;

  this->prevDist = 0;
  this->dist = 0;
  this->speed = 0;

  this->sekstiSekTimerFor = 0;
  this->sekstiSekTimerEtter = 0;
  this->sekstiSekTimer = 0;

  this->sekstiSekMaksHastighet = 0;
  this->gjennomsnittsHastighet = 0;
  this->sekstiSekunderDist = 0;
  this->prevSekstiSekunderDist = 0;

  this->tid70 = 0;
  this->tid70Etter = 0;
  this->tid70Differensial = 0;
  this->maksHastiget = 400; // NB: husk å endre denne verdien til faktisk makshastighet.

  this->chargingCycles = 0;

}

float Zumo32U4_bibliotek_gruppe_8::getDistance()
{
  return dist;
}

float Zumo32U4_bibliotek_gruppe_8::getSpeed()
{
    return speed;
}

float Zumo32U4_bibliotek_gruppe_8::setCapacity(float speed, unsigned long ms, float currentCapacity)
{
  if (speed < 0)
  {
    charcgingCycles++;
  }
//---------------------------------------------------------------

  if((buttonB == true) && (SOSmodeOneTimeOnly == false))
    {
      SOSmode = true;
      SOSmodeOneTimeOnly = true;
    }

//---------------------------------------------------------------
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

void Zumo32U4_bibliotek_gruppe_8::findSekstiSekTid(float speed)
{

    if (sekstiSekTimerFor == 0)
    {
        sekstiSekTimerEtter = 0;
    }

    else
    {
        sekstiSekTimerEtter = millis();
    }

    if (speed > 0.1)
    {
        sekstiSekTimer = sekstiSekTimer + (sekstiSekTimerEtter - sekstiSekTimerFor);
        sekstiSekTimerFor = millis();
    }

    else if (speed < 0.1)
    {
        sekstiSekTimer = sekstiSekTimer + (sekstiSekTimerEtter - sekstiSekTimerFor);
        sekstiSekTimerFor = 0;
    }
}

void speedometerEvery60(float speed)
{

if (sekstiSekTimer > 60000)
    {
        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


        if (speed > sekstiSekMaksHastighet)
        {                 // Her henter jeg inn makshastigheten som skal vises etter seksti sekunder.
            sekstiSekMaksHastighet = speed; // VIKTIG
        }


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


        gjennomsnittsHastighet = (sekstiSekunderDist - prevSekstiSekunderDist)/60; // VIKTIG Denne er ikke ferdig!!!
        prevSekstiSekunderDist = sekstiSekunderDist; // sekstiSekunderDist trenger en verdi.


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */


        if (speed > sekstiSekMaksHastighet)
        {
            tid70 = millis();
        }

        if (speed < sekstiSekMaksHastighet)
        {
            tid70Etter = millis();
            tid70Differensial = tid70Etter - tid70; // VIKTIG
        }


        /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    sekstiSekTimer = 0;

    }
}

void updateSpeedDist()  // Denne funksjonen erstatter distanse koden.
{
  int16_t countLeft = encoders.getCountsLeft();
  int16_t countRight = encoders.getCountsRight();

  int16_t avgCount = (countLeft + countRight) / 2;

  float rotasjoner = (float)avgCount / (75.81 * 12.0);
  dist = rotasjoner * 12.2522;

  if( lastTimeGetSpeed > 100){
    float cmSecond =  / ((float)ms / 1000);

    prevDist = dist;
    lastTimeGetSpeed = millis();
    }
}

/* Disse funksjonene fungerer nok ikke pågrunn av mangel på constructors.*/
