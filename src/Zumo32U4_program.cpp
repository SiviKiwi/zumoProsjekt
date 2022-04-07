/*
  Dette er kjorefila til Zumobilen. Den benytter seg av biblioteket gruppen har laget.
  Laget av gruppe 8 BIELEKTRO V22. Alvar, Sivert og Iver.
*/

#include <Arduino.h>
#include "Zumo32U4_bibliotek_gruppe_8.h"
#include <Wire.h>
#include <Zumo32U4.h>
#include <cmath>
#include <EEPROM.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
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

  if (EEPROM.read(1) == 1)
  {
    batteryHealth = EEPROM.read(0);
  }
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
    speed = egendefinert.getSpeed();

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











 // -------------------------------------------------------------------------

  // Batteriadvarselsfunksjonen


bool batteryLevelState = false;
unsigned long batteryLevelWarningLedTimer1 = 0;
unsigned long batteryLevelWarningLedTimer2 = 0;
unsigned long batteryLevelWarningTimer = 0;
bool batteryLevelWarningOne = false;
void batteryLevelWarning()
{
  if ((batteryLevel <= 10) && (batteryLevel > 5) && batteryLevelWarningOne = false)
  {
      ledYellow(1);
      buzzer.playFrequency(200, 300, 11)
      batteryLevelWarningLedTimer1 = millis();
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

     ledYellow(1);
      buzzer.playFrequency(1000, 300, 14)
      batteryLevelWarningLedTimer2 = millis();
      if ((millis() - batteryLevelWarningLedTimer2) > 300)
      {
        ledYellow(0); 
      }

      batteryLevelWarningLedTimer2 = millis();
      if ((millis() - batteryLevelWarningLedTimer2) > 100)
      {
        
      }

      ledYellow(1);
      buzzer.playFrequency(1000, 300, 14)
      batteryLevelWarningLedTimer2 = millis();
      if ((millis() - batteryLevelWarningLedTimer2) > 300)
      {
        ledYellow(0);
        batteryLevelState = true;
      }

      batteryLevelWarningTimer = millis();



  }

  else
  {
      batteryLevelWarningOne = false;
      batteryLevelState = false;
  }
}






// -------------------------------------------------------------------------






// Hvaskalvisesidisplayet funksjon:


unsigned long chargingDisplayTimer = 0;
continueChargingDisplayPrev = false;

chargingDisplay() // den som kjører mens man lader og 15 sek etterpå
{
  int batteryLevel = ((int)currentCapacity / (1200.0 * 3600)) * 100;


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
    int batteryLevel = ((int)currentCapacity / (1200.0 * 3600)) * 100;
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








int batteryHealth = 100; // Hva variabelen skal starte med
bool level_1 = false;
bool level_0 = false;

unsigned long tid70DifferensialPrev = 0;
int SOC<5%Prev = 0;
int chargingCyclesPrev = 0;
float sekstiSekMaksHastighetPrev = 0;
float gjennomsnittsHastighetPrev = 0; 
//int last_EEPROM_batteryHealth = 0;

void batteryHealth()
{

  //if (batteryHealth != last_EEPROM_batteryHealth)
  //{
    unsigned long time_now_batteryHealth = millis();
    if ((millis() - time_now_batteryHealth) > 1000)
    {
        //EEPROM.write(0, batteryHealth)
        EEPROM.put(0, batteryHealth);
        EEPROM.put(1,1);
        //last_EEPROM_batteryHealth = batteryHealth;
    }
  //}
  // chargingCycles
  // SOC<5%
  // gjennomsnittsHastighet
  // sekstiSekMaksHastighet
  // tid70Differensial
  // random faktor
  
  // level_1 for service
  // level_0 for batteribytt
  // batteriservice forbedrer batteryHealth poengsummen
  // batteribytte starter batteryHealth med ny verdi.

  // jeg tenker at utladningshastighet, altså fart må ha mye å si
  // jeg tenker også at antall ladesykluser må ha mye og si.

 batteryHealthAlgorithm();

  if( (batteryHealth < 15) && batteryHealth > 3 )
  {
    level_1 = true; 
  }
  else if(batteryHealth <= 3)
  {
    level_0 = true;
  }

}






void batteryServive()
{
  if (level_1 = true);  // Disse kan vel settes rett i loopen?
  {
    if (buttonA.isPressed())
    {
      batteryHealth = batteryHealth + 30;
      level_1 = false;
      // Husk også kostnad for batteriservice
    }
  }
}

void batteryReplacement()
{
  if (level_0 = true)
  {
    if (buttonA.isPressed())
    {
      batteryHealth = 100;
      level_0 = false;
      // Husk også kostnad for batteribytte.
    }
  }
}




void batteryHealthAlgorithm()
{

  const int K1 = 1; 
  const int K2 = 1;
  const int K3 = 1;

  const int Ka = 1;
  const int Kb = 1;
  const int Kc = 1;
  const int Kd = 1;
  const int Ke = 1;

 

  randomFactor = random(1,10);

  if (randomFactor > 8)
  {
    randomFactorExecuted = 0.5;
  }
  else
  {
    randomFactorExecuted = 1;
  }




  if ((tid70Differensial != tid70DifferensialPrev) || (SOC<5% != SOC<5%Prev) || (chargingCycles != chargingCyclesPrev) || (sekstiSekMaksHastighet != sekstiSekMaksHastighetPrev) || (gjennomsnittsHastighet != gjennomsnittsHastighetPrev))
  {
    if(tid70Differensial == tid70DifferensialPrev)
    {
      Ka = 0;
    }
    if(SOC<5% == SOC<5%Prev)
    {
      ke = 0;
    }

    if(chargingCycles == chargingCyclesPrev)
    {
      Kb = 0;
    }

    if(sekstiSekMaksHastighet == sekstiSekMaksHastighetPrev)
    {
      Kc = 0;
    }

    if(gjennomsnittsHastighet == gjennomsnittsHastighetPrev) 
    {
      Kd = 0;
    }

    int batteryHealth = randomFactorExecuted * (batteryHealth - ( (Ka* (K1 * pow((tid70Differensial),2))) + (Ke * (pow((SOC<5%),2))) + ( Kb * (K2*(chargingCycles))) + (K3 * ( Kc * sekstiSekMaksHastighet - Kd * gjennomsnittsHastighet)) ));

unsigned long tid70DifferensialPrev = tid70Differensial;
int SOC<5%Prev = SOC<5%;
int chargingCyclesPrev = chargingCycles;
float sekstiSekMaksHastighetPrev = sekstiSekMaksHastighet;
float gjennomsnittsHastighetPrev = gjennomsnittsHastighet; 

// dette her er nok litt rotete. Hvis dere har en annen løsning, så kan dere godt endre på det.
// Poenget med å gjøre det sånn er at vi ikke endrer på variabelen unødvendig, fordi den er satt
// opp slik at for hver gang den blir kalkulert så vil batteryHealth synke.

  }

 // Når det gjelder utregningen av batteryhealthfunksjonen så må vi nesten bare tilpasse konstantene når det blir nødving.

}
