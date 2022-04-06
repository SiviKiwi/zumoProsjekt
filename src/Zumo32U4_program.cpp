/*
* Dette er kjorefila til Zumobilen. Den benytter seg av biblioteket gruppen har laget.
* Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
*/

#include <arduino.h>
#include "Zumo32U4_bibliotek_gruppe_8.h"
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4LCD display;
Zumo32U4_bibliotek_gruppe_8 egendefinert;

unsigned long time;
float speed;

float currentCapacity = 1200.0 * 3600;

void setup()
{
  display.init();
  time = millis();

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= 0b00000101;
  TIMSK1 |= 0b00000001;
  interrupts();
}


void loop()
{

  egendefinert.oneSecBatState(); // Denne funksjonen sjekker om det har gått ti sekunder siden
                                 // den sist. gå til library c++ for å legge til statusfunksjoner.

 egendefinert.distance();

  unsigned long timeNow = millis(); 

  if (timeNow - time > 100)
  {
    unsigned long elapsedTime = timeNow - time;
    speed = egendefinert.getSpeed(egendefinert.dist - egendefinert.prevDist, elapsedTime);

    egendefinert.setCapacity(speed, elapsedTime, currentCapacity);

    display.clear();
    display.gotoXY(0, 0);
    display.print((float)currentCapacity / 3600.0);
    display.print(dist);
    display.print(speed);

    time = millis();
  }
    
    /* Under er for sekstisekunders */
    egendefinert.findSekstiSekTid(speed);
    egendefinert.speedometerEvery60(speed);
    
}

ISR(TIMER1_OVF_VECT)
{  
    TCNT1 = 31250;
    egendefinert.vectorOverflow(); 
}
