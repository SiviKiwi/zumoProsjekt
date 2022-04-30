/*
 * Dette er kjorefila til Zumobilen. Den benytter seg av biblioteket gruppen har laget.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar(Executive Cloud Computing and Database Wizard⭐),
 * Sivert(Lord Prime Minister and Supreme Archive Overseer) og Iver(Senior Integrated Circuit Developer God).
 */

#include <Arduino.h>
#include "Zumo32U4_bibliotek_gruppe_8.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include <EEPROM.h>

Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4LCD display;
Zumo32U4_bibliotek_gruppe_8 egendefinert(lineSensors, encoders, motors, buttonA, buttonB, buttonC, buzzer, display);


unsigned long time;
float speed;
float dist;
int variabel;
unsigned long time_now = 0;

float currentCapacity = 1200.0 * 3600;

void setup()
{
  //display.init();
  Serial1.begin(115200);
  
  time = millis();

  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= 0b00000101;
  TIMSK1 |= 0b00000001;
  interrupts();

  if (EEPROM.read(1) == 1)
  {
    egendefinert.setBatteryHealth(EEPROM.read(0));
  }
    
  delay(500);

  lineSensors.initFiveSensors();
  time_now = millis();
  while (millis() < time_now + 5000)
  {
    lineSensors.calibrate();
    motors.setSpeeds(100, -100);
  }
  motors.setSpeeds(0, 0);
  delay(500);
}


void loop()
{

  egendefinert.getState();

  // Funksjonen askForCharging må kjøres for å sjekke om case skal byttes.
  // Det er også relevant å se på løsninger for å kunne lade over IoT,
  // foreløpig er det kun over zumoens knapper.

  egendefinert.askForCharging();

  if (egendefinert.getZumoStopConfirmed() == true)
  {
    variabel = 1;
  }
  else{
    variabel = 0;
  }

  switch(variabel)
  {
    case 0:

      egendefinert.linjefolgerFunctions();
      break;

    case 1:

      motors.setSpeeds(0, 0);
      egendefinert.actualCharging();
      break;

  }

  egendefinert.countRounds();

  egendefinert.oneSecBatState(); // Denne funksjonen sjekker om det har gått ti sekunder siden
  // den sist. gå til library c++ for å legge til statusfunksjoner.

  egendefinert.updateSpeedDist();

  unsigned long timeNow = millis();
  if (timeNow - time > 100)
  {
    unsigned long elapsedTime = timeNow - time;
    speed = egendefinert.getSpeed();
    dist = egendefinert.getDistance();
    egendefinert.setCapacity(speed, elapsedTime);

    egendefinert.updateSpeedDist();
    egendefinert.findSekstiSekTid(speed); // Denne funksjonen setter variabelen speedometerEvery60 trenger for agere.
    egendefinert.speedometerEvery60(speed); // Setter variablene som vil brukes i battery_health

    egendefinert.batteryLevelWarning();

    egendefinert.updateBatteryHealth();
    egendefinert.checkForBatteryStatus();

    egendefinert.batteryService();
    egendefinert.batteryReplacement();

//    display.clear();
//    display.gotoXY(0, 0);
//    display.print(dist);
//    display.print((float)currentCapacity / 3600.0);
//    display.print(dist);
//    display.print(speed);



    //egendefinert.displayFunctions(); // Denne skal erstatte alle med display ovenfor.

    time = millis();
  }

  /* Under er for sekstisekunders */


  egendefinert.sendSerial();
}

ISR(TIMER1_OVF_vect)
{
  TCNT1 = 31250;
  egendefinert.vectorOverflow(); // Denne funksjonen forteller oneSecBatState om den skal agere.
}
