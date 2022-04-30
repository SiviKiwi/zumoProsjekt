String hus = "hallo";

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
unsigned long besteTid = 0;
unsigned long besteIdealTidsAvvik = 0;
unsigned long idealTidsAvvik = 0;
unsigned long idealTid = 0;
String linjeFolger = "";
String printVerdi = "";

float gamleSaldo = 0;
bool sporOmSaldoState = false;

long lastMsg = 0;
char msg[50];
int value = 0;
int i = 0;
float distance1 = 0;
int globalPremiePenger = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void nyRundeFunction();
void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();
void hendelserEtterNyRunde();
String displayFunctionsSSD1306();
void bankKomm(int premiePenger);



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
  Serial2.println(displayFunctionsSSD1306());
  millisPrintScreenState = false;
  hendelserEtterNyRunde();
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

  
  if (topic == "idealTid")
  {
    int b = messageTemp.length();
    if (b > 0)
    {
      idealTid = messageTemp.toInt();
    }
  }
  
  else if (topic == "linjeFolger")
  {
    if (messageTemp == "Avslått" || messageTemp == "Normal" || messageTemp == "Idealtid" || messageTemp == "Bestetid")
    {
      linjeFolger = messageTemp;
    }
  }
  
  else if (topic == "sporOmSaldo2")
  {
    
      gamleSaldo = messageTemp.toFloat();
      String nySaldo = String(gamleSaldo + globalPremiePenger);
      client.publish("sendSaldo", nySaldo.c_str());
      //Serial2.println("ubaduhba");
      //delay(1000);
      //sporOmSaldoState = true;
    
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
      client.subscribe("idealTid");
      client.subscribe("sporOmSaldo");
      client.subscribe("sendSaldo");
      client.subscribe("linjeFolger");
      client.subscribe("sporOmSaldo2");

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
      //bankKomm(1000);
      //TODO penger

    }
    if (idealTidsAvvik > besteIdealTidsAvvik)
    {
      besteIdealTidsAvvik = idealTidsAvvik;
      //bankKomm(500);
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
  globalPremiePenger = premiePenger;
  /*if (sporOmSaldoState == true)
  {
    // OBS OBS hvis den ikke fungerer prøv å put callback inn her!
    
  String nySaldo = String(gamleSaldo + premiePenger);
  client.publish("sendSaldo", nySaldo.c_str());
  sporOmSaldoState = false;
  }*/
}
