/*
 * Zumo32U4_bibliotek_gruppe_8.h - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
 */

#ifndef Zumo32U4_bibliotek_gruppe_8.h
#define Zumo32U4_bibliotek_gruppe_8.h

#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>

class Zumo32U4_bibliotek_gruppe_8{
  public:
    double getDistance(int16_t encoderCount);
    double getSpeed(float distance, unsigned long ms);
    double setCapacity(float speed, unsigned long ms);
};

#endif