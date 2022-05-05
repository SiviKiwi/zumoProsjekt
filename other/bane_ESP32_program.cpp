// Bibliotek for display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Biliotek for tilkobling til Wifi
#include <WiFi.h>
// Bibliotek for kommunikasjon med Mosquito
#include <PubSubClient.h>
// Bibliotek for I2C kommunikasjon
#include <Wire.h>


#include <iostream>
// Bibliotek for konvertering av verdier
#include <string>
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

// Mosquitotjener ip
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

  // Hvis ingen timer er startet startes en timer.
  if (millisPrintScreenState == false)
  {
    millisPrintScreen = millis();
    millisPrintScreenState = true;
  }

  // Hvis det er mer enn 1 sekund siden timeren startet oppdateres skjermen.
  if (millis() > millisPrintScreen + 1000)
  {
  Serial2.println(displayFunctionsSSD1306());
  millisPrintScreenState = false;
  // Sjekker om det er en ny runde, og TODO:
  hendelserEtterNyRunde();
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
  String messageTemp;

  // Setter beskjeden sammen til en streng
  for (int i = 0; i < length; i++) {
    messageTemp += (char)message[i];
  }


  // konverterer idealtiden til en int og lagrer den i en variabel vi kan bruke i koden
  if (topic == "idealTid")
  {
    int b = messageTemp.length();
    if (b > 0)
    {
      idealTid = messageTemp.toInt();
    }
  }

  // Motar hva slags linjefølger zumoen skal bruke og lagrer det i en variabel som skal sendes med seriel
  // til zumoen
  else if (topic == "linjeFolger")
  {
    if (messageTemp == "Avslått" || messageTemp == "Normal" || messageTemp == "Idealtid" || messageTemp == "Bestetid")
    {
      linjeFolger = messageTemp;
    }
  }

  // Regner ut ny saldo fra gammelsaldo og sende tilbake den nye saldoen
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


// Funksjon som kobler til MQTT og abbonerer på flere topics
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
      // Venter 5 sekund før den prøver å koble til igjen
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
  }
}


void hendelserEtterNyRunde()
{

  // Hvis det er en ny runde
  if (nyRunde == "1")
  {
    // Send det over MQTT slik at nodered vet det
    client.publish("Ny_runde", nyRunde.c_str());
    bankKomm(200);
    //TODO sende penger fordi fullført runde som i monopol


    // Regner ut rundetid
    rundeTidSlutt = millis();
    rundeTid = rundeTidSlutt - rundeTidStart; // TODO Sende rundetid til Node-red evt. bool nyRunde.
    rundeTidStart = millis();

    // Regner ut avvik fra ideel rundetid
    idealTidsAvvik = rundeTid - idealTid;

    String idealTidsAvvikStreng = String(idealTidsAvvik);


    // publiserer avviket til MQTT
    client.publish("idealTidsAvvik", idealTidsAvvikStreng.c_str());


    // Sjekker om rundetiden er raskere enn den beste tiden til banen
    if (rundeTid < besteTid)
    {
      besteTid = rundeTid;
      //bankKomm(1000);
      //TODO penger

    }
    // Sjekker om avviket er mindre enn det beste avviket fra idealtid
    if (idealTidsAvvik < besteIdealTidsAvvik)
    {
      besteIdealTidsAvvik = idealTidsAvvik;
      //bankKomm(500);
      //TODO penger

    }

    // Si i fra om at det ikke lenger er en ny runde
    nyRunde = "0";
  }
}


// Returnerer hva som skal vises på displayet
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


// Regner ut pengepremie
// Dette er ikke ferdig :(
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
