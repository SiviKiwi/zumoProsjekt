/*
 * Zumo32U4_bibliotek_gruppe_8.cpp - Bibliotek for funksjoner laget spesielt for Zumo32U4.
 * Laget av gruppe 8 BIELEKTRO V22. Alvar(Executive Cloud Computing and Database Wizard⭐),
 * Sivert(Lord Prime Minister and Supreme Archive Overseer) og Iver(Senior Integrated Circuit Developer God).
 */

#include <Arduino.h>
#include <Zumo32U4_bibliotek_gruppe_8.h>
#include <Wire.h>
#include <Zumo32U4.h>
#include <EEPROM.h>






Zumo32U4_bibliotek_gruppe_8::Zumo32U4_bibliotek_gruppe_8(
      Zumo32U4Encoders encoders,
      Zumo32U4Motors motors,
      Zumo32U4ButtonA buttonA,
      Zumo32U4ButtonB buttonB,
      Zumo32U4ButtonC buttonC,
      Zumo32U4Buzzer buzzer,
      Zumo32U4LCD display){

  this->encoders = encoders;
  this->motors = motors;
  this->buttonA = buttonA;
  this->buttonB = buttonB;
  this->buttonC = buttonC;
  this->buzzer = buzzer;
  this->display = display;
  
  // funksjon - setCapacity
  this->currentCapacity = 1200.0 * 3600;
  
  // funksjon - 
  this->twoToTenCounter = 0;
  this->tenAchieved = false;
  this->lastTimeGetSpeed = 0;
  this->start_time_one_sec_display = 0;

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
  this->maksHastighet = 400; // NB: husk å endre denne verdien til faktisk makshastighet.

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
  this->batteryHealth = batteryHealth;
}

float Zumo32U4_bibliotek_gruppe_8::setCapacity(float speed, unsigned long ms)
{
  if (speed < 0)
  {
    chargingCycles++;
  }
  
//---------------------------------------------------------------
// SOS modus for 10x lading av batteri
  if((buttonB.isPressed()) && (SOSmodeOneTimeOnly == false))
    {
      SOSmode = true;
      SOSmodeOneTimeOnly = true;
    }

  int multiplier = 1;
  if ((SOSmode == true) && (currentCapacity < 864000))
  {
    if (speed > 0) {
      multiplier = 1;
    }
    else {
      multiplier = 10;
    }
  }

  if (currentCapacity > 864000)
  {
    SOSmode = false;
  }
  
  //---------------------------------------------------------------

  float currentUsage = 2.0 * multiplier * speed + 10.0;
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
        twoToTenCounter = 0;
    }
}

void Zumo32U4_bibliotek_gruppe_8::oneSecBatState()
{
    if (tenAchieved == true)
    {
        //everyTenSecondsDisplay(); Ikke nødvendig siden den er flyttet til egen skjermvelger

        if (everyTenSecondsDisplayState == false)
        {
          start_time_one_sec_display = millis();
          everyTenSecondsDisplayState = true;
        }

        if (millis() - start_time_one_sec_display > 1000)
        {
          twoToTenCounter = 0;
          tenAchieved = false;
          everyTenSecondsDisplayState = false;
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

  if(millis() - lastTimeGetSpeed > 100){
    speed = (dist - prevDist) / ((float)(millis() - lastTimeGetSpeed) / 1000);

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
  if ((batteryLevel <= 10) && (batteryLevel > 5) && batteryLevelWarningOne == false)
  {
//-----------
      if (ledYellow1State1 == false)
      {
        ledYellow(1);
        buzzer.playFrequency(200, 300, 11);
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
        buzzer.playFrequency(1000, 300, 14);
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
  display.print(getBatteryLevel()); //TODO trenger vi getBatteryLevel sivert?
  display.print("%");

  display.gotoXY(0, 1);
  display.print("Char_$: ");
  display.print("\n");  // TODO legge til IoT for ladekostnader.
  display.print("\n");

  display.gotoXY(6, 1);
  display.print("acc_bal: ");
  display.print("\n");  // TODO legge til IoT for ladekostnader.
  display.print("\n");



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
display.print(getSpeed());
display.print("m/s");

display.gotoXY(0, 1);
display.print("Dist:");
display.print(getDistance());
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
  if (tenAchieved == true) //TODO Trenger vi getTenAchieved Sivert?
  {
    everyTenSecondsDisplay();
  }
  else if (absContinueChargingDisplay == true)
  {
    chargingDisplay();
  } else {
    runningDisplay();
  }

// Hvis det trengs flere displaytyper, legg gjerne til.

}



void Zumo32U4_bibliotek_gruppe_8::batteryHealthAlgorithm()
{

  int K1 = 1;
  int K2 = 1;
  int K3 = 1;

  // her kan vi forandre på koeffisientene slik at vi får et passende resultat.

  int Ka = 1;
  int Kb = 1;
  int Kc = 1;
  int Kd = 1;
  int Ke = 1;



  int randomFactor = random(1,10);
  int randomFactorExecuted = 0;

  if (randomFactor > 8)
  {
    randomFactorExecuted = 0.5;
  }
  else
  {
    randomFactorExecuted = 1;
  }




  if ((tid70Differensial != tid70DifferensialPrev) || (StateOfChargeBelow5 != StateOfChargeBelow5Prev) || (chargingCycles != chargingCyclesPrev) || (sekstiSekMaksHastighet != sekstiSekMaksHastighetPrev) || (gjennomsnittsHastighet != gjennomsnittsHastighetPrev))
  {
    if(tid70Differensial == tid70DifferensialPrev)
    {
      Ka = 0;
    }
    if(StateOfChargeBelow5 == StateOfChargeBelow5Prev)
    {
      Ke = 0;
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

    tid70DifferensialPrev = tid70Differensial;
    StateOfChargeBelow5Prev = StateOfChargeBelow5;
    chargingCyclesPrev = chargingCycles;
    sekstiSekMaksHastighetPrev = sekstiSekMaksHastighet;
    gjennomsnittsHastighetPrev = gjennomsnittsHastighet;

// dette her er nok litt rotete. Hvis dere har en annen løsning, så kan dere godt endre på det.
// Poenget med å gjøre det sånn er at vi ikke endrer på variabelen unødvendig, fordi den er satt
// opp slik at for hver gang den blir kalkulert så vil batteryHealth synke.

  }

 // Når det gjelder utregningen av batteryhealthfunksjonen så må vi nesten bare tilpasse konstantene når det blir nødving.

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
}
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


void Zumo32U4_bibliotek_gruppe_8::batteryService()
{
  if (level_1 == true)  // Disse kan vel settes rett i loopen?
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
  if (level_0 == true)
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





/*


void normalLinjefolger();
{

  int position = lineSensors.readLine(lineSensorValues);
  // Her ser man at man leser av disse sensorene.

      // TODO Vi må legge inn funkjsonen som gjør at den skal kunne returnere
      // fra en blindvei.

  int avvik = position - 2000;
  // Her ser man avviket. Avviket forteller oss om målet.
  // Når avviket er null, kreves det ingen regulering.
  int konstantP = 1;  // Disse er konstantene til PD-reguleringen.
  int konstantD = 10; // De er viktige for dens egenskaper.

  int PD = konstantP * avvik + konstantD * (avvik - forrigeAvvik);
  // Her får vi pådraget. Det er selve reguleringen.
  // Den bestemmer hvilke forandringer vi skal
  // gjøre på hastigheten til motorene.

  forrigeAvvik = avvik;

  int venstrePaadrag = 400 + PD;
  int hoyrePaadrag = 400 - PD;

  venstrePaadrag = constrain(venstrePaadrag, 0, 400);
  hoyrePaadrag = constrain(hoyrePaadrag, 0, 400);   // Vi ønsker ikke
  // mer enn 400 til
  // motorene.

  motors.setSpeeds(venstrePaadrag, hoyrePaadrag); 

}

void avslaattLinjefolger();
{

  // TODO Her må vi bestemme om denne funksjonen skal være litt
  // komplisert ved at den skal kunne være avslått på ulike
  // plasser.

  motors.setSpeeds(0,0);

}

unsigned long rundetid = 0;
unsigned long rundetidStart = 0;
unsigned long rundetidSlutt = 0;
unsigned long best_rundetid = 0;
int antall_runder_counter = 0;
void besteTidLinjefolger();
{
  // TODO Jeg vet ikke helt hvordan vi skal løse den annet enn å bare si at den beste runden
  // gir poeng eller noe sånt. å begynne å påvirke reguleringen for å få enda bedre tid
  // er svært vanskelig og jeg tenker at vi ikke har tid til det. Skal vi begynne å legge 
  // til integralledd liksom. denne funksjonen kan egt være en poenggivende funksjon.

  if (nyRunde == true)
  {
    int antall_runder_counter++;
    unsigned long rundetidSlutt = millis();
    unsigned long rundetid = rundetidSlutt - rundetidStart;
    unsigned long rundetidStart = millis();

    nyRunde = false;

  }

  if(rundetid > best_rundetid)
  {
    best_rundetid = rundetid;

    // TODO her må det gis poeng siden man har klart å slå forrige rundetid.
  }
}

void idealTidLinjefolger();
{

  int idealtid;
  bool nyRunde
  unsigned long besteRundetid;

  if(nyRunde == true)
  {

    // Her skal den endre på pådragene slik at man kan oppnå en tid nærmere idealtid

    unsigned long rundetidSlutt = millis();
    unsigned long rundetid = rundetidSlutt - rundetidStart;
    unsigned long rundetidStart = millis();
    
    int idealtidAvvik = rundetid - idealtid;

    int idealtidKonstantP = 1;  // Disse er konstantene til PD-reguleringen.
    int idealtidKonstantD = 10; // De er viktige for dens egenskaper.

    int idealtidPD = idealtidKonstantP * idealtidAvvik + idealtidKonstantD * (idealtidAvvik - idealtidForrigeAvvik);


    if(abs(idealtidAvvik) > abs(idealtidForrigeAvvik))
    {
      besteRundetid = rundetid;

      // TODO her må det gis poeng siden det har blitt
      //  oppnådd det minste avviket fra idealtiden.

    }

    idealtidForrigeAvvik = idealtidAvvik;

    nyRunde = false;
  }

  int position = lineSensors.readLine(lineSensorValues);
  // Her ser man at man leser av disse sensorene.
  int avvik = position - 2000;
  // Her ser man avviket. Avviket forteller oss om målet.
  // Når avviket er null, kreves det ingen regulering.
  int konstantP = 1;  // Disse er konstantene til PD-reguleringen.
  int konstantD = 10; // De er viktige for dens egenskaper.

  int PD = konstantP * avvik + konstantD * (avvik - forrigeAvvik);
  // Her får vi pådraget. Det er selve reguleringen.
  // Den bestemmer hvilke forandringer vi skal
  // gjøre på hastigheten til motorene.

  forrigeAvvik = avvik;

  int venstrePaadrag = 400 + PD + idealtidPD;
  int hoyrePaadrag = 400 - PD - idealtidPD;

  venstrePaadrag = constrain(venstrePaadrag, 0, 400);
  hoyrePaadrag = constrain(hoyrePaadrag, 0, 400);   // Vi ønsker ikke
  // mer enn 400 til
  // motorene.

  motors.setSpeeds(venstrePaadrag, hoyrePaadrag);

}


void linjefolgerFunctions();
{                     // Det som bestemmer denne valgfunksjonen kan være
                      // IoT avhengig
 
  if()
  {
    avslaattLinjefolger();
  }

  else if()
  {
    besteTidLinjefolger();
  }

  else if()
  {
    idealTidLinjefolger();
  }

  else
  {
    normalLinjefolger();
  }
}





bool nyRunde = false;
void nyRunde();
{
  // TODO Dette er en funksjon som skal gjøre en boolsk variabel sann når 
  //      zumoen har kjørt en runde. Dette skal brukes i 
  //      kunkurranselinjefølgerfunksjonene. Sensorer er bestilt og på vei.
  //
}
*/
