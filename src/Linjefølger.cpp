

#include <Wire.h>
#include <Zumo32U4.h>

#define NUM_SENSORS 5
// Dette er en veldig morsom kode siden vi tar
Zumo32U4Motors motors;    // i bruk PD-regulering.
Zumo32U4ButtonA buttonA;
Zumo32U4LineSensors lineSensors;  // Vi bruker sensorer som kan
Zumo32U4Buzzer buzzer;            // oppfatte teipen.
Zumo32U4LCD display;

unsigned long time_now = millis();
unsigned int lineSensorValues[NUM_SENSORS]; // Her ser vi at sensorene
// blir satt inn i en
int forrigeAvvik = 0;                       // variabel.

void setup() {
  // put your setup code here, to run once:

  display.clear();
  display.print("Zumo klar for å kalibreres. Trykk knapp A");
  display.clear();

  buttonA.waitForButton();

  lineSensors.initFiveSensors();

  while (millis() < time_now + 5000) {

    lineSensors.calibrate();
    motors.setSpeeds(200, -200);

  }

  motors.setSpeeds(0, 0);

  display.print("zumo ferdig med kalibrering. Trykk knapp A for starte linjefølgeren");
  buttonA.waitForButton();
}

void loop() {
  // put your main code here, to run repeatedly:
  display.clear();

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

  int venstrePaadrag = 400 + PD;
  int hoyrePaadrag = 400 - PD;

  venstrePaadrag = constrain(venstrePaadrag, 0, 400);
  hoyrePaadrag = constrain(hoyrePaadrag, 0, 400);   // Vi ønsker ikke
  // mer enn 400 til
  // motorene.

  motors.setSpeeds(venstrePaadrag, hoyrePaadrag);
  display.print(position);
}