#include <Arduino.h>
// Bibliotek for i2c kommunikasjon
#include <Wire.h>

// Bibliotek for display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define tmpPin A0
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

String c = "c";

// Setter opp display med bredde og høyde
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


// Viser teks på disply
void displayText(String text){
  
  display.clearDisplay();
  display.setCursor(0, 10);
  display.println(text);
  display.display();
  
  }

void setup() {

  // Starter seriellkommunikasjon med ESP
  Serial.begin(115200);
  

  // Sjekker om Arduino klarer å koble til display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  // Venter 2 sek
  delay(2000);
  // setter opp display med tekstfarge og størrelse
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  // Printer en testmelding under oppstart
  display.println("Iver er kul");
  display.display(); 

}

void loop() {
  // put your main code here, to run repeatedly:


  // Sjekker om det er kommet data inn i Arduinoens buffer
if (Serial.available() > 0)
{
  //henter ut strengen fra seriellbufferen
    c = Serial.readString();
}

// Viser teksten på skjermen
displayText(c);
Serial.println(c);
//c = "c";

delay(10);

}
