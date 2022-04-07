/*

* Zumo32U4_bibliotek_gruppe_8.h - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
 */

#pragma once

#ifndef Zumo32U4_bibliotek_gruppe_8

#include <Wire.h>
#include <Zumo32U4.h>


class Zumo32U4_bibliotek_gruppe_8{
  public:

    Zumo32U4_bibliotek_gruppe_8();
    int getTwoTenCounter();
    bool getTenAchieved();
    double getDistance();
    double getSpeed();
    void updateSpeedDist();
    double setCapacity(float speed, unsigned long ms);
    void timer1OverflowCounter();
    void vectorOverflow();
    void oneSecBatState();
    void speedometerEvery60();
    void findSekstiSekTid();
    void askForCharging();
    

  private:

    int currentCapacity;
  
    int twoToTenCounter;
    bool tenAchieved;
    unsigned long lastTimeGetSpeed;

    bool SOSmode;
    bool SOSmodeOneTimeOnly;

    float prevDist;
    float dist;
    float speed;

    unsigned long sekstiSekTimerFor;
    unsigned long sekstiSekTimerEtter;
    unsigned long sekstiSekTimer;

    float sekstiSekMaksHastighet;
    float gjennomsnittsHastighet;
    float sekstiSekunderDist;
    float prevSekstiSekunderDist;

    unsigned long tid70;
    unsigned long tid70Etter;
    unsigned long tid70Differensial;
    int maksHastighet; // NB: husk å endre denne verdien til faktisk makshastighet.

    int chargingCycles;
    int SOC<5%;
    bool askForChargingState;
    int batteryLevel;
    bool continueChargingDisplay;
    bool absContinueChargingDisplay;
    bool batteryLevelState;
    unsigned long batteryLevelWarningLedTimer1;
    unsigned long batteryLevelWarningLedTimer2;
    unsigned long batteryLevelWarningTimer;
    bool batteryLevelWarningOne;

    bool ledYellow1State1;
    bool ledYellow1State2;
    bool ledYellow0State2;
    bool ledYellow1Nr2State2;
    bool ledYellow0Nr2State2;
    
    unsigned long chargingDisplayTimer;
    bool continueChargingDisplayPrev;

    bool level_1;
    bool level_0;

    unsigned long tid70DifferensialPrev;
    int SOC5%;
    int chargingCyclesPrev;
    float sekstiSekMaksHastighetPrev;
    float gjennomsnittsHastighetPrev;

    
};

#endif
