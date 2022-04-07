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

  this->currentCapacity = 1200.0 * 3600;

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
  this->SOC<5% = 0;
  this->askForChargingState = false;
  this->batteryLevel = 100;
  this->continueChargingDisplay = false;
  this->absContinueChargingDisplay = false;
  this->batteryLevelState = false;
  this->batteryLevelWarningLedTimer1 = 0;
  this->batteryLevelWarningLedTimer2 = 0;
  this->batteryLevelWarningTimer = 0;
  this->batteryLevelWarningOne = false;

  this->ledYellow1State1 = false;
  this->ledYellow1State2 = false;
  this->ledYellow0State2 = false;
  this->ledYellow1Nr2State2 = false;
  this->ledYellow0Nr2State2 = false;



  

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

  batteryLevel = (currentCapacity / (3600 * 1200)) * 100;

  if (currentCapacity < 216000)
  {
    SOC<5%++;
  }

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


void askForCharging()
{
  if ((buttonA.isPressed() == true && buttonC.isPressed() == true) || askForChargingState == true)
  {
    // sett swith variabel til casen med funksjonen actualCharging() i.
    askForChargingState = true;
    absContinueChargingDisplay = true; // Denne gjør det mulig å endre hva som vises på displayet.
  }
}


void actualCharging()
{
 
  if (buttonC.isPressed() == true)
  {
    batteryLevel = batteryLevel + 10;
    if (batteryLevel > 100)
    {
      batteryLevel = 100;
    }

    // husk å også trekke kostnad fra konto
    // hvis kontoen er tom, så må ladingen avsluttes(askForChargingState)

  }

  if ((buttonA.isPressed() == true) && (buttonC.isPressed() == true)) // Jeg har gjort det slik at
  {
    askForChargingState = false;
    continueChargingDisplay = true;
  }
}






bool ledYellow1State1 = false;
bool ledYellow1State2 = false;
bool ledYellow0State2 = false;
bool ledYellow1Nr2State2 = false;
bool ledYellow0Nr2State2 = false;




void batteryLevelWarning()
{
  if ((batteryLevel <= 10) && (batteryLevel > 5) && batteryLevelWarningOne = false)
  {
//-----------
      if (ledYellow1State1 == false)
      {
        ledYellow(1);
        buzzer.playFrequency(200, 300, 11)
        batteryLevelWarningLedTimer1 = millis();
        ledYellow1State1 = true;
      }
//-----------
      if ((millis() - batteryLevelWarningLedTimer1) > 300)
      {
        ledYellow(0);
        batteryLevelWarningOne = true;
      }


  }

  else if (batteryLevel <= 5)
  {

    if ((batteryLevelState == false) || ((millis() - batteryLevelWarningTimer) > 15000)) 

      motors.setSpeeds(0,0);


//-----------
      if (ledYellow1State2 == false)
      {
        ledYellow(1);
        buzzer.playFrequency(1000, 300, 14)
        batteryLevelWarningLedTimer2 = millis();
        ledYellow1State2 = true;
      }
//----------    
      if (ledYellow0State2 == false)
      {
        if ((millis() - batteryLevelWarningLedTimer2) > 300)
        {
          ledYellow(0); 
          batteryLevelWarningLedTimer2 = millis();
          ledYellow0State2 = true;
        }
      }

//----------   
      if (ledYellow1Nr2State2 == false)
      {
        if ((millis() - batteryLevelWarningLedTimer2) > 100)
        {
          ledYellow(1);
          buzzer.playFrequency(1000, 300, 14);
          batteryLevelWarningLedTimer2 = millis();
          ledYellow1Nr2State2 = true;
        }
      }
    
//----------  
      if (ledYellow0Nr2State2 == false)
      {
        if ((millis() - batteryLevelWarningLedTimer2) > 300)
        {
          ledYellow(0);
          batteryLevelState = true;
          batteryLevelWarningTimer = millis();
          ledYellow0Nr2State2 = true;
        }
      }

      if(( ledYellow1State1 == true) && (ledYellow1State2 == true) && (ledYellow0State2 == true) && (ledYellow1Nr2State2 == true) && (ledYellow0Nr2State2 == true))
      {
        ledYellow1State1 = false;
        ledYellow1State2 = false;
        ledYellow0State2 = false;
        ledYellow1Nr2State2 = false;
        ledYellow0Nr2State2 = false;
      }
  }

  else
  {
      batteryLevelWarningOne = false;
      batteryLevelState = false;
  }
}