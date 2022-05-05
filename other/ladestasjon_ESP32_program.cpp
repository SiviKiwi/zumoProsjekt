#include <Arduino.h>
// Bibliotek for display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Biliotek for tilkobling til Wifi
#include <WiFi.h>
// Bibliotek for kommunikasjon med Mosquito
#include <PubSubClient.h>
// Bibliotek for I2C kommunikasjon
#include <Wire.h>

#include <cstdlib>
#include <iostream>
// Bibliotek for konvertering av verdier
#include <string>
//#include <sstream>
using namespace std;

// Pins for Ultralydsensor
int trigger = 32;
int echo = 34;

unsigned long millisPrintScreen = 0;
bool millisPrintScreenState = false;
String unit = "cm";

// Innloggingsinfo for nettverk
const char* ssid = "iPhone 13 Pro Max";
const char* password = "hallosivert";

// Mosquitoserver ip
const char* mqtt_server = "172.20.10.4";

String nyRunde = "0";
unsigned long rundeTid = 0;
unsigned long rundeTidStart = 0;
unsigned long rundeTidSlutt = 0;

bool zumoWaitForLadeskjemaState = false;
bool bileierIDState = false;
bool ladetypeState = false;
bool batteriNivaaState = false;
bool prislisteLadingState = false;
bool ladeGrenseState = false;
bool zumoAskForChargingState = false;

int waitForLadeskjemaIsTrueCounter = 0;

float gamleSaldo = 0;
bool sporOmSaldoState = false;
float nySaldo = 0;

float totalLadekost = 0;
String bileierID = "";
int batteriNivaa = 0;

bool ladingTimerState = false;
unsigned long ladingTimer = 0;
float ladePris = 0;
int ladeHastighet = 0;
int ladeGrense = 100;
bool stoppLadingState = false;
String ladetype = "";
String prislisteLading = "";
long lastMsg = 0;
char msg[50];
int value = 0;
int i = 0;
float distance1 = 0;


WiFiClient espClient;
PubSubClient client(espClient);

void nyRundeFunction();
void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();
void zumoBelow5AndNyRunde();
String displayFunctionsSSD1306();
void waitForLadeskjema();
void lading();




void setup() {

  // Starter serielkommunikasjon med Arduino
  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  // Setter pins for UltralydSensor
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  //Serial2.println("hello");

  // Setter opp wifi og Mosquitotjener
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {

  // Prøver å koble til wifi
  if (!client.connected()) {
    reconnect();
  }

  // Brukes for å oppdatere meldinger fra Mosquitto og opprettholde tilkobling til server
  client.loop();

  // Sjekker om zumo har gjordt en ny runde
  nyRundeFunction();
  // Sjekker om zumo er klar for lading
  zumoBelow5AndNyRunde();


  //Serial2.println("hallo");
  //delay(1000);

  // Skrur trigger lav for å være sikker på at
  // høypulsen er "ren"
  digitalWrite(trigger, LOW);
  delayMicroseconds(2);
  // Skrur på triggerpin i 10µs, deretter av igjen
  digitalWrite(trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger, LOW);
  // Leser av echopinne, og returnerer lydbølgens reisetid
  // i mikrosekunder
  long duration = pulseIn(echo, HIGH);
  // Utregning av distanse. 0.0343 er lydens hastighet i cm/µs.
  // Deles på to da lyden skal både frem og tilbake igjen
  distance1 = duration * 0.0343 / 2;
  //Serial.print(distance);
  delay(100);

  // Hvis det er mer enn 1 sekund siden timeren startet oppdateres skjermen.
  if (millisPrintScreenState == false)
  {
    millisPrintScreen = millis();
    millisPrintScreenState = true;
  }

  if (millis() > millisPrintScreen + 1000)
  {
    // Venter på TODO
    waitForLadeskjema();
    //TODO
    lading();
    Serial2.println(displayFunctionsSSD1306());
    millisPrintScreenState = false;

  }

}


// Funksjon for å koble til wifi
void setup_wifi()
{
  delay(10);

  // Kobler til wifi
  WiFi.begin(ssid, password);

  // Venter til wifi er tilkoblet
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

}


// Kjører når en ny melding kommer over ett av topics-ene ESP-en abbonerer på
void callback(String topic, byte* message, unsigned int length) {
  /*Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");*/
  String messageTemp;

  // Setter beskjeden sammen til en streng
  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }


  // Henter saldo fra Node red
  if (topic == "sporOmSaldo2")
  {

    gamleSaldo = messageTemp.toFloat();


  }
  // Henter bileier ID
  else if (topic == "bileierID")
  {
    bileierID = messageTemp;
    bileierIDState = true;

  }
  // Henter hvilke ladetype som skal brukes
  else if (topic == "ladetype")
  {
    ladetype = messageTemp;
    ladetypeState = true;

  }
  // Lagrer batterinivå som oppdateres fra nodeRed under lading
  else if (topic == "batteriNivaa")
  {
    float x = messageTemp.toFloat();
    batteriNivaa = int(x);
    batteriNivaaState = true;

  }
  else if (topic == "prislisteLading")
  {
    prislisteLading = messageTemp;
    prislisteLadingState = true;

  }
  // Henter info om hvilken prosent zumo skal lade til
  else if (topic == "ladegrense")
  {
    ladeGrense = messageTemp.toFloat();
    ladeGrenseState = true;
    if (ladeGrense > 0)
    {

    }
    else
    {
      ladeGrense = 100;
    }
  }
  // Sjekker om bilen skal stoppe lading
  else if (topic == "stoppLading")
  {
    stoppLadingState = true;
  }
  // Sjekker om bilen skal lades
  else if (topic == "zumoAskForCharging")
  {
    zumoAskForChargingState = true;
  }
}


// Funksjon som kobler til MQTT og abbonerer på flere topics
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial2.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial2.println("connected");
      // Subscribe
      client.subscribe("sporOmSaldo");
      client.subscribe("sendSaldo");
      client.subscribe("sporOmSaldo2");
      client.subscribe("zumoStopConfirmed");
      client.subscribe("bileierID");
      client.subscribe("stoppLading");
      client.subscribe("ladingStoppet");
      client.subscribe("zumoAskForCharging");
      client.subscribe("batteriNivaaSend");
      client.subscribe("totalLadekost");
      client.subscribe("batteriNivaa");
      client.subscribe("prislisteLading");
      client.subscribe("ladegrense");
      client.subscribe("ladetype");


    }

    // Venter 5 sekund før den prøver å koble til igjen
    else {
      delay(5000);
    }
  }
}


// funksjon som kjører hvis zumo-en er forran ultrasonisk sensor
void nyRundeFunction()
{
  // Hvis noe er nermere enn 15cm
  if (distance1 < 15)
  {
    // Setter ny runde til 1, dette er en streng som skal sendes over seriell til Zumo
    nyRunde = "1";
    //nyRunde må settes lav etter alle funksjonene som er avhengig av den har fått kjørt.
  }
}


// Funksjon som kjører hvis bilen er forran ladestasjonen og den ber om å få lade
void zumoBelow5AndNyRunde()
{

  if (nyRunde == "1" && zumoAskForChargingState == true)
  {

    // Sier i fra om at zumo har stoppet foran ladestasjon
    client.publish("zumoStopConfirmed", "1");
    // Zumo skal spørre etter ladeskjema
    zumoWaitForLadeskjemaState = true;
    nyRunde = "0";


  }
}

// Sjekker at zumo har mottatt all info den trenger for å starte lading
void waitForLadeskjema()
{
  if ( (zumoWaitForLadeskjemaState == true) && (ladeGrenseState == true) && (ladetypeState == true) && (bileierIDState == true) && (prislisteLadingState == true) && (batteriNivaaState == true))
  {

    // Henter ut prisliste
    String prislisteLadingArray[3];
    for (int i = 0; i < 3; ++i)
    {
      prislisteLadingArray[i] = getValue(prislisteLading, ',', i);
    }


    // Ved sjukolading lades det sent, men billigere
    if (ladetype == "Schukolading")
    {
      ladePris = prislisteLadingArray[0].toFloat();
      ladeHastighet = 1;
    }
    // Superlading er 3 ganger så rask som scuko men litt dyrere
    else if (ladetype == "Superlading")
    {
      ladePris = prislisteLadingArray[1].toFloat();
      ladeHastighet = 3;
    }
    // Superduperlading er 5 ganger raskere enn schuko men dyrest
    else if (ladetype == "Superduperlading")
    {
      ladePris = prislisteLadingArray[2].toFloat();
      ladeHastighet = 5;
    }
  }

}



// Funksjon for lading av zumo
void lading()
{
  // starter timer
  if (ladingTimerState == false)
  {
    ladingTimer = millis();
    ladingTimerState = true;

    // spør banken om saldo på konto
    client.publish("sporOmSaldo", "WX79");
  }

        // Her kan vi legge inn batterihelse skift batteri og service

/*
   if(batterHealth > x && y > batteryHealth)
   {
    
   }
   else if(battery helath < x)
   {
    
   }

*/
        

  // Hver 5. sekund "lades" zumo-en
  if ((millis() > ladingTimer + 5000) && (stoppLadingState == false) && (zumoWaitForLadeskjemaState == true))
  {


    /// Sjekker at det er nok penger på konto
    if (gamleSaldo > (((100 - batteriNivaa)) / 4) * (ladePris * 4 * 0.01))
    {

      // øker batterinivå
      batteriNivaa = batteriNivaa + (4 * ladeHastighet);
      // regner ut pris
      float ladekost = 4 * ladePris * 0.01 * ladeHastighet;
      // Sjekker om batteriet er over ladegrensen
      if (batteriNivaa >= ladeGrense)
      {
        // trekker fra det som er ladet over og gir tilbake de pengene som ikke skulle vært trekkt
        int delta = batteriNivaa - ladeGrense;
        ladekost = ladekost - delta * ladePris * 0.01 * 4;
        batteriNivaa = ladeGrense;

        // resetter variabler med info for lading
        bileierIDState = false;
        ladetypeState = false;
        batteriNivaaState = false;
        prislisteLadingState = false;
        ladeGrenseState = false;

        zumoWaitForLadeskjemaState = false;
        zumoAskForChargingState = false;
        // sier i fra om at lading er ferdig
        client.publish("ladingStoppet", "");

      }

      // regner ut ny saldo
      nySaldo = gamleSaldo - ladekost;
      // regner ut total ladekostnad
      totalLadekost = (totalLadekost + ladekost);


      int a = int(totalLadekost);
      String astring = String(a);
      // Sier i fra hvor mye bilen ladet for
      client.publish("totalLadekost", astring.c_str());
      int b = int(batteriNivaa);
      String bstring = String(b);
      // Sier i fra hva det nye batterinivået er
      client.publish("batteriNivaaSend", bstring.c_str());
      int c = int(nySaldo);
      String cstring = String(c);
      // Sier i fra hva den nye saldoen er
      client.publish("sendSaldo", cstring.c_str());

      if (batteriNivaa >= ladeGrense) {
        totalLadekost = 0;
      }
    }
    else
    {
      bileierIDState = false;
      ladetypeState = false;
      batteriNivaaState = false;
      prislisteLadingState = false;
      ladeGrenseState = false;

      zumoWaitForLadeskjemaState = false;
      zumoAskForChargingState = false;
      client.publish("ladingStoppet", "");
    }

    ladingTimerState = false;

  }
  else if (stoppLadingState == true)
  {

    bileierIDState = false;
    ladetypeState = false;
    batteriNivaaState = false;
    prislisteLadingState = false;
    ladeGrenseState = false;

    totalLadekost = 0;

    zumoWaitForLadeskjemaState = false;
    zumoAskForChargingState = false;
    int c = int(nySaldo);
    String cstring = String(c);
    client.publish("sendSaldo", cstring.c_str());
    client.publish("ladingStoppet", "1");
    stoppLadingState = false;

  }
}


// Returnerer hva som skal vises på displayet
String displayFunctionsSSD1306()
{

  if (zumoWaitForLadeskjemaState == true)
  {

    if (waitForLadeskjemaIsTrueCounter < 2)
    {
      waitForLadeskjemaIsTrueCounter = waitForLadeskjemaIsTrueCounter + 1;
      return String(bileierID);
    }
    else if (waitForLadeskjemaIsTrueCounter < 4)
    {

      waitForLadeskjemaIsTrueCounter = waitForLadeskjemaIsTrueCounter + 1;
      return (String(batteriNivaa) + "%");
    }
    else {

      waitForLadeskjemaIsTrueCounter = 0;
      return (String(totalLadekost) + "kr");

    }
  }
  else
  {
    return getValue(prislisteLading, ',', 0);
    //Serial2.println(getValue(prislisteLading,',',0));
    //delay(500);
  }
}


// Funksjon som henter ut verdier fra en streng separert av et gitt tegn
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strengIndex[] = {0, -1};
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strengIndex[0] = strengIndex[1] + 1;
      strengIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strengIndex[0], strengIndex[1]) : "";
}
