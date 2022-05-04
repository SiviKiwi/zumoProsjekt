/*
 * Dette er kjorefila til Zumobilen. Den benytter seg av biblioteket gruppen har laget.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar(Executive Cloud Computing and Database Wizard⭐),
 * Sivert(Lord Prime Minister and Supreme Archive Overseer) og Iver(Senior Integrated Circuit Developer God).
 */

#include <Arduino.h>
// Importer vårt lokale bibliotek
#include "Zumo32U4_bibliotek_gruppe_8.h"
// Importer bibliotekene som kreves av Zumo
#include <Wire.h>
#include "Zumo32U4.h"
// Importer EEPROM for lagring av verdier når Zumo er slått av
#include <EEPROM.h>

// Oppretter objekter av Zumo-klasser
Zumo32U4Encoders encoders;
Zumo32U4LineSensors lineSensors;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4LCD display;
// Oppretter et objekt av vårt lokale bibliotek og gir den Zumo-objektene som parameter
Zumo32U4_bibliotek_gruppe_8 egendefinert(lineSensors, encoders, motors, buttonA, buttonB, buttonC, buzzer, display);


// definerer globale variabler
unsigned long time;
float speed;
float dist;
int variabel;
unsigned long time_now = 0;

float currentCapacity = 1200.0 * 3600;

void setup()
{
  display.init();
  // Starter Serial1 for kommunikasjon med ESP32
  Serial1.begin(115200);
  
  time = millis();

  // TODO: få iver til å forklare hva dette er
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCCR1B |= 0b00000101;
  TIMSK1 |= 0b00000001;
  interrupts();

  // Leser batterihelse fra EEPROM
  if (EEPROM.read(1) == 1)
  {
    // Setter batterihelsen
    egendefinert.setBatteryHealth(EEPROM.read(0));
  }
    
  delay(1000);

  // kalibrerer linjesensorer
  egendefinert.initFiveSensors();
  time_now = millis();
  while (millis() < time_now + 5000)
  {
    egendefinert.calibrate();
    motors.setSpeeds(50, -50);
  }
  // Stanser Zumo-bil og venter 1 sekund
  motors.setSpeeds(0, 0);
  delay(1000);
}


void loop()
{

  // Setter state variabelen
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
      display.println("OK!!!!");
      egendefinert.linjefolgerFunctions();
      break;

    case 1:

      motors.setSpeeds(0, 0);
      egendefinert.actualCharging();
      break;

  }

  // Funksjon som teller antall runder Zumoen har kjørt
  egendefinert.countRounds();

  egendefinert.oneSecBatState(); // Denne funksjonen sjekker om det har gått ti sekunder siden
  // den sist. gå til library c++ for å legge til statusfunksjoner.

  egendefinert.updateSpeedDist();

  // Koden her kjøres en gang hvert 100ms
  unsigned long timeNow = millis();
  if (timeNow - time > 100)
  {
    unsigned long elapsedTime = timeNow - time;
    // Henter ut bilens hastighet
    speed = egendefinert.getSpeed();
    // Henter ut distansen bilen har kjørt
    dist = egendefinert.getDistance();
    // Regner ut ny batteri kapasitet
    egendefinert.setCapacity(speed, elapsedTime);

    // Kjører funksjon for oppdatering av hastighet og distanse
    egendefinert.updateSpeedDist();
    // Denne funksjonen setter variabelen speedometerEvery60 trenger for agere.
    egendefinert.findSekstiSekTid(speed);
    // Setter variablene som vil brukes i battery_health
    egendefinert.speedometerEvery60(speed);
    // Advarer om batterinivået er under 10 eller 5 prosent
    egendefinert.batteryLevelWarning();

    // Oppdateret helsen til batteriet
    egendefinert.updateBatteryHealth();
    // Sjekker batteriets status og om batteriet må på service eller byttes
    egendefinert.checkForBatteryStatus();

    // Hvis bilen må på service starter denne muligheten til å 'fikse' batteriet
    egendefinert.batteryService();
    // Hvis batteriet må byttes på starter denne muligheten til å 'bytte' batteriet
    egendefinert.batteryReplacement();

    // Funksjon som printer verdier til displayet
    // denne er kommentert ut siden displayet ikke kan brukes sammen med ESP32
//egendefinert.displayFunctions(); // Denne skal erstatte alle med display ovenfor.

    time = millis();
  }

  /*
    Sender følgene verdier til ESP32 over serielkommunikasjon:
    BileierID
    ZumoAskForCharging
    CurrentCapacity
    Speed
    Distance
    batteryHealth
  */
  egendefinert.sendSerial();
}

ISR(TIMER1_OVF_vect)
{
  TCNT1 = 31250;
  egendefinert.vectorOverflow(); // Denne funksjonen forteller oneSecBatState om den skal agere.
}
