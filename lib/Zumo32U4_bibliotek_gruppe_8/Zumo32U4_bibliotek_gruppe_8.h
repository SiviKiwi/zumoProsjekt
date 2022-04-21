/*

* Zumo32U4_bibliotek_gruppe_8.h - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar(Executive Cloud Computing and Database Wizard⭐),
 * Sivert(Lord Prime Minister and Supreme Archive Overseer) og Iver(Senior Integrated Circuit Developer God).
 */

#pragma once

#ifndef Zumo32U4_bibliotek_gruppe_8

#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
#include <EEPROM.h>

#define NUM_SENSORS 5



class Zumo32U4_bibliotek_gruppe_8{
  public:

    Zumo32U4_bibliotek_gruppe_8(
        Zumo32U4Encoders encoders,
        Zumo32U4Motors motors,
        Zumo32U4ButtonA buttonA,
        Zumo32U4ButtonB buttonB,
        Zumo32U4ButtonC buttonC,
        Zumo32U4Buzzer buzzer,
        Zumo32U4LCD display);

    int getTwoTenCounter();
    bool getTenAchieved();
    float getDistance();
    float getSpeed();
    void updateSpeedDist();
    float setCapacity(float speed, unsigned long ms);
    void vectorOverflow();
    void oneSecBatState();
    void speedometerEvery60(float speed);
    void findSekstiSekTid(float speed);
    void askForCharging();
    void actualCharging();
    void checkForBatteryStatus();
    void displayFunctions();
    void chargingDisplay();
    void runningDisplay();
    void everyTenSecondsDisplay();
    void batteryLevelWarning();
    void updateBatteryHealth();
    int getBatteryLevel();
    int getBatteryHealth();
    void setBatteryHealth(int batteryHealth);
    void batteryHealthAlgorithm();
    void batteryReplacement();
    void batteryService();

  private:

    Zumo32U4Encoders encoders;
    Zumo32U4Motors motors;
    Zumo32U4ButtonA buttonA;
    Zumo32U4ButtonB buttonB;
    Zumo32U4ButtonC buttonC;
    Zumo32U4Buzzer buzzer;
    Zumo32U4LCD display;

    float currentCapacity;
  
    int twoToTenCounter;
    bool tenAchieved;
    unsigned long lastTimeGetSpeed;
    unsigned long start_time_one_sec_display;

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
    int StateOfChargeBelow5;
    bool askForChargingState;
    float batteryLevel;
    bool continueChargingDisplay;
    bool absContinueChargingDisplay;
    bool everyTenSecondsDisplayState;
    bool batteryLevelState;
    unsigned long batteryLevelWarningLedTimer1;
    unsigned long batteryLevelWarningLedTimer2;
    unsigned long batteryLevelWarningTimer;
    bool batteryLevelWarningOne;
    int batteryHealth;


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
    int chargingCyclesPrev;
    float sekstiSekMaksHastighetPrev;
    float gjennomsnittsHastighetPrev;
    int StateOfChargeBelow5Prev;

    
};

#endif
