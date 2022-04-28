
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
using namespace std;




const char* ssid = "iPhone 13 Pro Max";
const char* password = "hallosivert";

const char* mqtt_server = "172.20.10.4";

WiFiClient espClient;
PubSubClient client(espClient);

long lastMsg = 0;
char msg[50];
int value = 0;
int i = 0;
unsigned long publishTimer = 0;


String getValue(String data, char separator, int index);

void setup_wifi();
void callback(String topic, byte* message, unsigned int length);
void reconnect();


String inputFromZumo = "";

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);


  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);


}

void loop()
{
  // put your main code here, to run repeatedly:

  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  
  if (millis() > publishTimer + 1000) {
    if (Serial.available() > 0)
    {
      inputFromZumo = Serial.readString();

      String Streng = String(getValue(inputFromZumo, ';', i));
      client.publish("speed", Streng.c_str()); // Speed
      Streng = String(getValue(inputFromZumo, ';', i));
      client.publish("distance", Streng.c_str()); // distance

      Streng = String(getValue(inputFromZumo, ';', i));
      client.publish("batteriNivaa", Streng.c_str()); // battery
      Streng = String(getValue(inputFromZumo, ';', i));
      client.publish("batteryHealth", Streng.c_str()); // batteryHealth

      //Streng = String(getValue(inputFromZumo, ';', i));
      //client.publish("sendSaldo", Streng.c_str()); sendSaldo
      //Streng = String(getValue(inputFromZumo, ';', i));
      //client.publish("sporOmSaldo", Streng.c_str()); sporOmSaldo

      if (getValue(inputFromZumo, ';', i) == "0")
      {
        Streng = String(getValue(inputFromZumo, ';', i));
        client.publish("bileierID", Streng.c_str()); // bileierID
        Streng = String(getValue(inputFromZumo, ';', i));
        client.publish("zumoAskForCharging", Streng.c_str()); // zumoAskForcharging x, y
      }


      publishTimer = millis();

    }   // Evt. lage en millis() timer slik at den kun sender data en gang i sekundet. NB!: det er gjort.
  }
}





void setup_wifi()
{
  delay(10);
  // We start by connecting to a WiFi network
  //Serial.println(String(topic) + "," + String(messageTemp));
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
  //Serial.println(String(topic) + "," + String(messageTemp));

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message


  if (topic == "idealTid")
  {
    int b = messageTemp.length();
    if (b > 0)
    {
      //idealTid = messageTemp.toInt();

      Serial.println(String(topic) + "," + String(messageTemp));
    }
  }

  else if (topic = "linjeFolger")
  {
    if (messageTemp == "Avslått" || messageTemp == "Normal" || messageTemp == "Idealtid" || messageTemp == "Bestetid")
    {
      //linjeFolger = messageTemp;

      Serial.println(String(topic) + "," + String(messageTemp));
    }
  }

  else if (topic == "sporOmSaldo2")
  {
    // motta hva nå enn saldoen er

    Serial.println(String(topic) + "," + String(messageTemp));
  }
  else if (topic == "Ny_runde")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  else if (topic == "idealTidsAvvik")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  else if (topic == "ladingStoppet")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  else if (topic == "batteriNivaaSend")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
  }
  else if (topic == "zumoStopConfirmed")
  {
    Serial.println(String(topic) + "," + String(messageTemp));
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
      // INN
      client.subscribe("Ny_runde");
      client.subscribe("idealTidsAvvik");
      client.subscribe("sporOmSaldo2");
      client.subscribe("linjeFolger");
      client.subscribe("ladingStoppet");
      client.subscribe("batteriNivaaSend");
      client.subscribe("zumoStopConfirmed");

      // evt. ladepris

      // UT
      client.subscribe("bileierID");
      client.subscribe("zumoAskForCharging");
      client.subscribe("batteriNivaa");
      client.subscribe("sporOmSaldo");
      client.subscribe("sendSaldo");
      client.subscribe("speed");
      client.subscribe("distance");


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








