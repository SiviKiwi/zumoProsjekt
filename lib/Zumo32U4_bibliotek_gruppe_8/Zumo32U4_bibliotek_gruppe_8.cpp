/*
 * Zumo32U4_bibliotek_gruppe_8.cpp - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar(Executive Cloud Computing and Database Wizard⭐),
 * Sivert(Lord Prime Minister and Supreme Archive Overseer) og Iver(Senior Integrated Circuit Developer God).
 */

#include <Arduino.h>
#include <Wire.h>
#include <Zumo32U4.h>
#include <EEPROM.h>

Zumo32U4Encoders encoders;
Zumo32U4Motors motors;
Zumo32U4ButtonA buttonA;
Zumo32U4ButtonB buttonB;
Zumo32U4ButtonC buttonC;
Zumo32U4Buzzer buzzer;
Zumo32U4LCD display;




Zumo32U4_bibliotek_gruppe_8::Zumo32U4_bibliotek_gruppe_8(){

  this->currentCapacity = 1200.0 * 3600;

  this->twoToTenCounter = 0;
  this->tenAchieved = false;
  this->lastTimeGetSpeed = 0;

  this->SOSmode = false;
  this->SOSmodeOneTimeOnly = false;

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
  this->StateOfChargeBelow5 = 0;
  this->askForChargingState = false;
  this->batteryLevel = 100;
  this->continueChargingDisplay = false;
  this->absContinueChargingDisplay = false;
  this->everyTenSecondsDisplayState = false;
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

  this->chargingDisplayTimer = 0;
  this->continueChargingDisplayPrev = false;

  this->batteryHealth = 100;
  this->level_1 = false;
  this->level_0 = false;

  this->tid70DifferensialPrev = 0;
  this->chargingCyclesPrev = 0;
  this->sekstiSekMaksHastighetPrev = 0;
  this->gjennomsnittsHastighetPrev = 0;
  this->StateOfChargeBelow5Prev = 0;

}

float Zumo32U4_bibliotek_gruppe_8::getDistance()
{
  return dist;
}

float Zumo32U4_bibliotek_gruppe_8::getSpeed()
{
    return speed;
}

int Zumo32U4_bibliotek_gruppe_8::getBatteryHealth()
{
  return batteryHealth;
}

int Zumo32U4_bibliotek_gruppe_8::getBatteryLevel()
{
  return batteryLevel;
}

void Zumo32U4_bibliotek_gruppe_8::setBatteryHealth(int batteryHealth)
{
  this->batteryHealth = battteryHealth;
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
    StateOfChargeBelow5++;
  }

  return currentCapacity;
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
        //everyTenSecondsDisplay(); Ikke nødvendig siden den er flyttet til egen skjermvelger

        if (everyTenSecondsDisplayState == false)
        {
          unsigned long time_now = millis();
          everyTenSecondsDisplayState = true;
        }

        if (millis() - time_now> + 1000)
        {
          twoToTenCounter = 0;
          tenAchieved = false;
        }
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

void Zumo32U4_bibliotek_gruppe_8::speedometerEvery60(float speed)
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

void Zumo32U4_bibliotek_gruppe_8::updateSpeedDist()  // Denne funksjonen erstatter distanse koden.
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


void Zumo32U4_bibliotek_gruppe_8::askForCharging()
{
  if ((buttonA.isPressed() == true && buttonC.isPressed() == true) || askForChargingState == true)
  {
    // sett swith variabel til casen med funksjonen actualCharging() i.
    askForChargingState = true;
    absContinueChargingDisplay = true; // Denne gjør det mulig å endre hva som vises på displayet.
  }
}


void Zumo32U4_bibliotek_gruppe_8::actualCharging()
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


void Zumo32U4_bibliotek_gruppe_8::batteryLevelWarning()
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








void Zumo32U4_bibliotek_gruppe_8::chargingDisplay() // den som kjører mens man lader og 15 sek etterpå
{
  // Her må vi displaye de rette variablene.

  display.scrollDisplayLeft();

  display.gotoXY(0, 0);
  display.print("Bat: ");
  display.print(getBatteryLevel();); //TODO trenger vi getBatteryLevel sivert?
  display.print("%");

  display.gotoXY(0, 1);
  display.print("Char_$: ");
  display.print();  // TODO legge til IoT for ladekostnader.
  display.print();

  display.gotoXY(6, 1);
  display.print("acc_bal: ");
  display.print();  //TODO legge til IoT for kontobalanse.
  display.print();



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


void Zumo32U4_bibliotek_gruppe_8::runningDisplay()  // Den som skal kjøres til vanlig
{

display.clear();

display.print("Spd:");
display.print(getSpeed(););
display.print("m/s");

display.gotoXY(0, 1);
display.print("Dist:");
display.print(getDistance(););
display.print("cm");

}


void Zumo32U4_bibliotek_gruppe_8::everyTenSecondsDisplay()  // Den som skal kjøres hvert tiende sekund på interrupt
{

  display.scrollDisplayLeft();

  display.gotoXY(0,0);
  display.print("Bat:");
  display.print("100");
  display.print("%");

  display.gotoXY(0,1);
  display.print("cc:");
  display.print("10");

  display.gotoXY(6,1);
  display.print("bh:");
  display.print("100");
  display.print("p");

}


void Zumo32U4_bibliotek_gruppe_8::displayFunctions()
{
  if () //TODO
  {
    runningDisplay();
  }
  else if (tenAchieved == true) //TODO Trenger vi getTenAchieved Sivert?
  {
    everyTenSecondsDisplay();
  }
  else if (absContinueChargingDisplay == true)
  {
    chargingDisplay();
  }

// Hvis det trengs flere displaytyper, legg gjerne til.

}










void Zumo32U4_bibliotek_gruppe_8::updateBatteryHealth()
{

  batteryHealthAlgorithm();

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
  // StateOfChargeBelow5
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

 void Zumo32U4_bibliotek_gruppe_8::checkForBatteryStatus()
 {

  if( (batteryHealth < 15) && batteryHealth > 3 )
  {
    level_1 = true; 
  }
  else if(batteryHealth <= 3)
  {
    level_0 = true;
  }

}


void Zumo32U4_bibliotek_gruppe_8::batteryServive()
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


void Zumo32U4_bibliotek_gruppe_8::batteryReplacement()
{
  if (level_0 = true)
  {
    if (buttonA.isPressed())
    {
      batteryHealth = 100;
      level_0 = false;
      // Husk også kostnad for batteribytte. Jeg tror akkurat denne kan inn i switch-casen.
      // Den forstyrrer jo tross alt ikke noe av det andre. Hvilke biler er det som 
      // skifter batteri i fart, eller mens noen av de andre funksjonene fungerer samtidig?
    }
  }
}


void Zumo32U4_bibliotek_gruppe_8::batteryHealthAlgorithm()
{

  const int K1 = 1; 
  const int K2 = 1;
  const int K3 = 1;

  // her kan vi forandre på koeffisientene slik at vi får et passende resultat.
  
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




  if ((tid70Differensial != tid70DifferensialPrev) || (StateOfChargeBelow5 != StateOfChargeBelow5) || (chargingCycles != chargingCyclesPrev) || (sekstiSekMaksHastighet != sekstiSekMaksHastighetPrev) || (gjennomsnittsHastighet != gjennomsnittsHastighetPrev))
  {
    if(tid70Differensial == tid70DifferensialPrev)
    {
      Ka = 0;
    }
    if(StateOfChargeBelow5 == StateOfChargeBelow5Prev)
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

    int batteryHealth = randomFactorExecuted * (batteryHealth - ( (Ka* (K1 * pow((tid70Differensial),2))) + (Ke * (pow((StateOfChargeBelow5),2))) + ( Kb * (K2*(chargingCycles))) + (K3 * ( Kc * sekstiSekMaksHastighet - Kd * gjennomsnittsHastighet)) ));

unsigned long tid70DifferensialPrev = tid70Differensial;
int StateOfChargeBelow5Prev = StateOfChargeBelow5;
int chargingCyclesPrev = chargingCycles;
float sekstiSekMaksHastighetPrev = sekstiSekMaksHastighet;
float gjennomsnittsHastighetPrev = gjennomsnittsHastighet; 

// dette her er nok litt rotete. Hvis dere har en annen løsning, så kan dere godt endre på det.
// Poenget med å gjøre det sånn er at vi ikke endrer på variabelen unødvendig, fordi den er satt
// opp slik at for hver gang den blir kalkulert så vil batteryHealth synke.

  }

 // Når det gjelder utregningen av batteryhealthfunksjonen så må vi nesten bare tilpasse konstantene når det blir nødving.

}
