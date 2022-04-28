#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define tmpPin A0
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

String c = "c";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

String unit = "C";

float readTemp() {

float readValue = analogRead(tmpPin);
// Regner om fra ADC-verdi til spenning
float voltage = (readValue*5)/1023;

 // Regner om fra spenning til celsius
float celsius = (voltage-0.5)*100;

 return celsius;
}

void displayText(String text){
  
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(text);
  display.display();
  
  }

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println("Iver er kul");
  display.display(); 

}

void loop() {
  // put your main code here, to run repeatedly:


if (Serial.available() > 0)
{
  c = Serial.readString();

}

String stringDisplay = "x";
displayText(c);
Serial.println(c);
//c = "c";

delay(10);



}
