/*
  Dette er kjorefila til Zumobilen. Den benytter seg av biblioteket gruppen har laget.
  Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
*/

#include <arduino.h>
#include "Zumo32U4_bibliotek_gruppe_8.h"
#include <Wire.h>
#include <Zumo32U4.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
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

  egendefinert.updateSpeedDist(/* Må ta inn tiden siden programfilen startet.*/);

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
  egendefinert.findSekstiSekTid(speed); // Denne funksjonen setter variabelen speedometerEvery60 trenger for agere.
  egendefinert.speedometerEvery60(speed); // Setter variablene som vil brukes i battery_health

}

ISR(TIMER1_OVF_VECT)
{
  TCNT1 = 31250;
  egendefinert.vectorOverflow(); // Denne funksjonen forteller oneSecBatState om den skal agere.
}












int batteryLevel = ((int)currentCapacity / (1200.0 * 3600)) * 100;
int chargingCycles = 0;// når farten er negativ, når vi trykker på knapper, når bilen er i ladestasjon.



// ladefunksjon

bool askForChargingState = false;
void askForCharging()
{
  if ((buttonA == true && buttonC == true) || askForChargingState == true)
  {
    // sett swith variabel til casen med funksjonen actualCharging() i.
    askForChargingState = true;
    absContinueChargingDisplay = true;
  }
}


bool continueChargingDisplay = false;
bool absContinueChargingDisplay = false;
void actualCharging()
{
  // ------------------------------------------
  // Her skal displayet oppdatere seg i sanntid
  //-------------------------------------------
  if (buttonC == true)
  {
    batteryLevel = batteryLevel + 10;
    if (batteryLevel > 100)
    {
      batteryLevel = 100;
    }

    // husk å også trekke kostnad fra konto
    // hvis kontoen er tom, så må ladingen avsluttes(askForChargingState)

  }

  if ((buttonA == true) && (buttonC == true)) // Jeg har gjort det slik at
  {
    askForChargingState = false;
    continueChargingDisplay = true;
  }
}





bool batteryLevelState = false;
void batteryLevelWarning()
{
  if ((batteryLevel <= 10) && (batteryLevel > 5))
  {

  }

  else if (batteryLevel <= 5)
  {

  }
}


















Hvaskalvisesidisplayet funksjon:


unsigned long chargingDisplayTimer = 0;
continueChargingDisplayPrev = false;

chargingDisplay() // den som kjører mens man lader og 15 sek etterpå
{
  // Her må vi displaye de rette variablene.
  if ((continueChargingDisplay == true) && (continueChargingDisplayPrev == false))
  {
    chargingDisplayTimer = millis();
  }
  continueChargingDisplayPrev = continueChargingDisplay;
  if ((millis() - chargingDisplayTimer > 15000) && (continueChargingDisplay == true))
  {
    absContinueChargingDisplay = false;
    continueChargingDisplay = false;
  }
}

void runningDisplay()  // Den som skal kjøres til vanlig
{

}


void everyTenSecondsDisplay()  // Den som skal kjøres hvert tiende sekund på interrupt
{

}

void displayFunctions()
{
  if ()
  {
    runningDisplay();
  }
  else if ()
  {
    everyTenSecondsDisplay();
  }
  else if (absContinueChargingDisplay == true)
  {
    chargingDisplay();
  }
}
