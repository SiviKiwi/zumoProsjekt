#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>

//#include <cstdlib>
#include <iostream>
#include <string>
using namespace std;


int trigger = 32;
int echo = 34;

const char* ssid = "iPhone 13 Pro Max";
const char* password = "hallosivert";

const char* mqtt_server = "172.20.10.4";

String nyRunde = "0";
unsigned long rundeTid = 0;
unsigned long rundeTidStart = 0;
unsigned long rundeTidSlutt = 0;
unsigned long besteTid = 0;
unsigned long besteIdealTidsAvvik = 0;
unsigned long idealTidsAvvik = 0;
unsigned long idealTid = 0;
String unit = "cm";
String linjeFolger = "";
String printVerdi = "";

unsigned long millisPrintScreen = 0;
bool millisPrintScreenState = false;

float gamleSaldo = 0;
bool sporOmSaldoState = false;

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
int i = 0;
float distance1 = 0;


void nyRundeFunction();
void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();
void hendelserEtterNyRunde();
String displayFunctionsSSD1306();
void bankKomm(int premiePenger);



void setup() {
  // put your setup code here, to run once:

  //Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  pinMode(trigger, OUTPUT);
  pinMode(echo, INPUT);
  Serial2.println("hello");
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


}

void loop()
{
  // put your main code here, to run repeatedly:
  
  
  nyRundeFunction();
  Serial2.println("hello");
  if (!client.connected()) {
    reconnect();
  }

  client.loop();



  //-----------------------------------------------------------------------
  if (millisPrintScreenState == false)
  {
    millisPrintScreen = millis();
    millisPrintScreenState = true;
  }

  if (millis() > millisPrintScreen + 1000)
  {
    Serial2.println(displayFunctionsSSD1306());
    //Serial2.println(distance1);
    millisPrintScreenState = false;
    hendelserEtterNyRunde();
  }
  //-----------------------------------------------------------------------


//cSerial2.println("Hallo");



  //-----------------------------------------------------------------------
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
  //Serial.println(distance1);
  delay(100);
  //-----------------------------------------------------------------------

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
  
  if (topic == "testTopic2") {
    //Serial.print("Changing output to ");
    if (messageTemp == "on") {
      //Serial.println("on");
      digitalWrite(2, HIGH);
    }
    
    else if (messageTemp == "off") {
      //Serial.println("off");
      digitalWrite(2, LOW);
    }
  }
  
  else if (topic == "idealTid")
  {
    int b = messageTemp.length();
    if (b > 0)
    {
      idealTid = messageTemp.toInt();
    }
  }
  
  else if (topic = "linjeFolger")
  {
    if (messageTemp == "Avslått" || messageTemp == "Normal" || messageTemp == "Idealtid" || messageTemp == "Bestetid")
    {
      linjeFolger = messageTemp;
    }
  }
  
  else if (topic == "sporOmSaldo")
  {
    if (messageTemp != "WX79")
    {
      gamleSaldo = messageTemp.toFloat();
      sporOmSaldoState = true;
    }
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
      client.subscribe("Ny_runde");
      client.subscribe("idealTidsAvvik");
      client.subscribe("sporOmSaldo");
      client.subscribe("sendSaldo");
      client.subscribe("linjeFolger");
      client.subscribe("idealTid");

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
  }
}


void hendelserEtterNyRunde()
{
  
  if (nyRunde == "1")
  {
    client.publish("Ny_runde", nyRunde.c_str());
    bankKomm(200);
    //TODO sende penger fordi fullført runde som i monopol


    rundeTidSlutt = millis();
    rundeTid = rundeTidSlutt - rundeTidStart; // TODO Sende rundetid til Node-red evt. bool nyRunde.
    rundeTidStart = millis();

    idealTidsAvvik = rundeTid - idealTid;

    String idealTidsAvvikStreng = String(idealTidsAvvik);


    client.publish("idealTidsAvvik", idealTidsAvvikStreng.c_str());

      

    if (rundeTid < besteTid)
    {
      besteTid = rundeTid;
      bankKomm(1000);
      //TODO penger

    }
    if (idealTidsAvvik > besteIdealTidsAvvik)
    {
      besteIdealTidsAvvik = idealTidsAvvik;
      bankKomm(500);
      //TODO penger

    }

    nyRunde = "0";
  }
}


String displayFunctionsSSD1306()
{

  if (linjeFolger == "Bestetid")
  {
    return String(besteTid);
  }
  else if (linjeFolger == "Idealtid")
  {
    return String(besteIdealTidsAvvik);
  }
  else
  {

    return (String(distance1) + unit);
  }
}


void bankKomm(int premiePenger)
{
  client.publish("sporOmSaldo", "WX79");
  if (sporOmSaldoState != false)
  {
    // OBS OBS hvis den ikke fungerer prøv å put callback inn her!
    
  String nySaldo = String(gamleSaldo + premiePenger);
  client.publish("sendSaldo", nySaldo.c_str());
  sporOmSaldoState = false;
  }
}
