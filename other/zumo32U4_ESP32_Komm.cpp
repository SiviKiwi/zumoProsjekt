
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
using namespace std;




// Innloggingsinfo for nettverk
const char* ssid = "iPhone 13 Pro Max";
const char* password = "hallosivert";

// Mosquitoserver ip
const char* mqtt_server = "172.20.10.4";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
int i = 0;
unsigned long publishTimer = 0;

// Deklarerer funskjoner
String getValue(String data, char separator, int index);

void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();


String inputFromZumo = "";

void setup() {

  // Starter seriellkommunikasjon med Zumo
  Serial.begin(115200);


  // Setter opp wifi og Mosquitotjener
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


}

void loop()
{

  // Prøver å koble til wifi
  if (!client.connected()) {
    reconnect();
  }

  // Brukes for å oppdatere meldinger fra Mosquitto og opprettholde tilkobling til server
  client.loop();


  // Publiserer kun til MQTT hvert sekund
  if (millis() > publishTimer + 1000) {
    // Sjekker om ESP har data fra Zumo i seriellbuffer
    if (Serial.available() > 0)
    {
      // Leser streng fra seriellbuffer
      inputFromZumo = Serial.readString();

      // Henter ut verdier fra seriellstrengen og publiserer dem over MQTT
      String Streng = String(getValue(inputFromZumo, ';', 7));
      client.publish("speed", Streng.c_str()); // Speed
      Streng = String(getValue(inputFromZumo, ';', 9));
      client.publish("distance", Streng.c_str()); // distance

      Streng = String(getValue(inputFromZumo, ';', 5));
      client.publish("batteriNivaaSend", Streng.c_str()); // battery
      Streng = String(getValue(inputFromZumo, ';', 11));
      client.publish("batteryHealth", Streng.c_str()); // batteryHealth

      //Streng = String(getValue(inputFromZumo, ';', 15));
      //client.publish("sendSaldo", Streng.c_str()); sendSaldo
      //Streng = String(getValue(inputFromZumo, ';', 13));
      //client.publish("sporOmSaldo", Streng.c_str()); sporOmSaldo

      // Sjekker om zumo spør etter lading
      if (getValue(inputFromZumo, ';', 3) == "0")
      {
        // Sender rellevant info for lading
        Streng = String(getValue(inputFromZumo, ';', 1));
        client.publish("bileierID", Streng.c_str()); // bileierID
        Streng = String(getValue(inputFromZumo, ';', 3));
        client.publish("zumoAskForCharging", Streng.c_str()); // zumoAskForcharging x, y
      }     

      publishTimer = millis();

    }   // Evt. lage en millis() timer slik at den kun sender data en gang i sekundet. NB!: det er gjort.
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


  // Henter idealtid fra node red og sender det til Zumo
  if (topic == "idealTid")
  {
    int b = messageTemp.length();
    if (b > 0)
    {
      //idealTid = messageTemp.toInt();

      Serial.println(String(topic) + "," + String(messageTemp));
    }
  }

  // Henter linjefølgertype og sender det til Zumo
  else if (topic = "linjeFolger")
  {
    if (messageTemp == "Avslått" || messageTemp == "Normal" || messageTemp == "Idealtid" || messageTemp == "Bestetid")
    {
      //linjeFolger = messageTemp;

      Serial.println(String(topic) + "," + String(messageTemp));
    }
  }

  // Sjekker om NodeRed spør om saldo og sier i fra til Zumo
  else if (topic == "sporOmSaldo2")
  {
    // motta hva nå enn saldoen er

    Serial.println(String(topic) + "," + String(messageTemp));
  }
  // Sjekker om zumoen har passert målstrek og sier i fra til zumo
  else if (topic == "Ny_runde")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  // Henter avviket fra idealtid og sender det til Zumo
  else if (topic == "idealTidsAvvik")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  // Sier i fra til Zumo at lading har stoppet
  else if (topic == "ladingStoppet")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  // Sier i fra til zumo hva batterinivået er
  else if (topic == "batteriNivaa")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  //  Sier i fra at stopp er mottatt
  else if (topic == "zumoStopConfirmed")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
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
      // INN
      client.subscribe("Ny_runde");
      client.subscribe("idealTidsAvvik");
      client.subscribe("sporOmSaldo2");
      client.subscribe("linjeFolger");
      client.subscribe("ladingStoppet");
      client.subscribe("batteriNivaa");
      client.subscribe("zumoStopConfirmed");
      client.subscribe("idealTid");

      // evt. ladepris

      // UT
      client.subscribe("bileierID");
      client.subscribe("zumoAskForCharging");
      client.subscribe("batteriNivaaSend");
      client.subscribe("sporOmSaldo");
      client.subscribe("sendSaldo");
      client.subscribe("speed");
      client.subscribe("distance");


    }

    else {
    // Venter 5 sekund før den prøver å koble til igjen
      delay(5000);
    }
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
