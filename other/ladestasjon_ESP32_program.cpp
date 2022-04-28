#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

#include <cstdlib>
#include <iostream>
#include <string>
//#include <sstream>
using namespace std;

int trigger = 32;
int echo = 34;

unsigned long millisPrintScreen = 0;
bool millisPrintScreenState = false;
String unit = "cm";

const char* ssid = "iPhone 13 Pro Max";
const char* password = "hallosivert";

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
  // put your setup code here, to run once:

  Serial2.begin(115200, SERIAL_8N1, 16, 17);

  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial2.println("hello");

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  // put your main code here, to run repeatedly:

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  nyRundeFunction();
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

  if (millisPrintScreenState == false)
  {
    millisPrintScreen = millis();
    millisPrintScreenState = true;
  }

  if (millis() > millisPrintScreen + 1000)
  {
    waitForLadeskjema();
    lading();
    Serial2.println(displayFunctionsSSD1306());
    millisPrintScreenState = false;

  }

}


void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println();
  //Serial.print("Connecting to ");
  //Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.print(".");
  }

  /*Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());*/
}


void callback(String topic, byte* message, unsigned int length) {
  /*Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");*/
  String messageTemp;

  for (int i = 0; i < length; i++) {
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  //Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message


  if (topic == "sporOmSaldo2")
  {

    gamleSaldo = messageTemp.toFloat();


    //Serial2.println("ubaduhba");
    //delay(1000);
    //sporOmSaldoState = true;

  }
  else if (topic == "bileierID")
  {
    bileierID = messageTemp;
    bileierIDState = true;

  }
  else if (topic == "ladetype")
  {
    ladetype = messageTemp;
    ladetypeState = true;

  }
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
  else if (topic == "stoppLading")
  {
    stoppLadingState = true;
  }
  else if (topic == "zumoAskForCharging")
  {
    zumoAskForChargingState = true;
  }
}


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

    else {
      /*Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");*/
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void nyRundeFunction()
{
  if (distance1 < 15)
  {
    nyRunde = "1";
    //nyRunde må settes lav etter alle funksjonene som er avhengig av den har fått kjørt.
  }
}


void zumoBelow5AndNyRunde()
{

  if (nyRunde == "1" && zumoAskForChargingState == true)
  {

    client.publish("zumoStopConfirmed", "1");
    zumoWaitForLadeskjemaState = true;
    nyRunde = "0";


  }
}

void waitForLadeskjema()
{
  if ( (zumoWaitForLadeskjemaState == true) && (ladeGrenseState == true) && (ladetypeState == true) && (bileierIDState == true) && (prislisteLadingState == true) && (batteriNivaaState == true))
  {

    String prislisteLadingArray[3];
    for (int i = 0; i < 3; ++i)
    {
      prislisteLadingArray[i] = getValue(prislisteLading, ',', i);
    }


    if (ladetype == "Schukolading")
    {
      ladePris = prislisteLadingArray[0].toFloat();
      ladeHastighet = 1;
    }
    else if (ladetype == "Superlading")
    {
      ladePris = prislisteLadingArray[1].toFloat();
      ladeHastighet = 3;
    }
    else if (ladetype == "Superduperlading")
    {
      ladePris = prislisteLadingArray[2].toFloat();
      ladeHastighet = 5;
    }
  }

}



void lading()
{
  if (ladingTimerState == false)
  {
    ladingTimer = millis();
    ladingTimerState = true;

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
        

  if ((millis() > ladingTimer + 5000) && (stoppLadingState == false) && (zumoWaitForLadeskjemaState == true))
  {


    if (gamleSaldo > (((100 - batteriNivaa)) / 4) * (ladePris * 4 * 0.01))
    {

      batteriNivaa = batteriNivaa + (4 * ladeHastighet);
      float ladekost = 4 * ladePris * 0.01 * ladeHastighet;
      if (batteriNivaa >= ladeGrense)
      {
        int delta = batteriNivaa - ladeGrense;
        ladekost = ladekost - delta * ladePris * 0.01 * 4;
        batteriNivaa = ladeGrense;

        bileierIDState = false;
        ladetypeState = false;
        batteriNivaaState = false;
        prislisteLadingState = false;
        ladeGrenseState = false;

        zumoWaitForLadeskjemaState = false;
        zumoAskForChargingState = false;
        client.publish("ladingStoppet", "");

      }

      nySaldo = gamleSaldo - ladekost;
      totalLadekost = (totalLadekost + ladekost);


      int a = int(totalLadekost);
      String astring = String(a);
      client.publish("totalLadekost", astring.c_str());
      int b = int(batteriNivaa);
      String bstring = String(b);
      client.publish("batteriNivaaSend", bstring.c_str());
      int c = int(nySaldo);
      String cstring = String(c);
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