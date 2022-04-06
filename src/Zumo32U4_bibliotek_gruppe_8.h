/*
 * Zumo32U4_bibliotek_gruppe_8.h - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
 */

 #pragma once

#ifndef Zumo32U4_bibliotek_gruppe_8.h
#define Zumo32U4_bibliotek_gruppe_8.h

#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>


class Zumo32U4_bibliotek_gruppe_8{
  public:


    int getTwoTenCounter();

    bool getTenAchieved();

    double getDistance(int16_t encoderCount);
    double getSpeed(float distance, unsigned long ms);
    double setCapacity(float speed, unsigned long ms);
    void timer1OverflowCounter();
    void vectorOverflow();
    void oneSecBatState();
    void speedometerEvery60();
    void findSekstiSekTid();
    void distance();

  private:
    int twoToTenCounter = 0;
    bool tenAchieved = false;

    bool SOSmode = false;
    bool SOSmodeOneTimeOnly = false;

    float prevDist = 0;
    float dist = 0;

    unsigned long sekstiSekTimerFor = 0;
    unsigned long sekstiSekTimerEtter = 0;
    unsigned long sekstiSekTimer = 0;

    float sekstiSekMaksHastighet = 0;
    float gjennomsnittsHastighet = 0;
    float sekstiSekunderDist = 0;
    float prevSekstiSekunderDist = 0;

    unsigned long tid70 = 0;
    unsigned long tid70Etter = 0;
    unsigned long tid70Differensial = 0;
    const int maksHastiget = 0; // NB: husk å endre denne verdien til faktisk makshastighet.
};

#endif
