#include "MQ135.h"
#include "DHT.h"
#include <TimeLib.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);

MQ135 gasSensor = MQ135(A0);

SimpleTimer timer;

// finger sensor button
const int buttonPin = 6;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(buttonPin, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Game");
  lcd.setCursor(0, 1);
  lcd.print("Ventilate Or DIY");  
  delay(3000);
  timer.setInterval(5000L, sendTemps);
  lcd.clear();
}

void sendTemps() {
  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int ppmc = gasSensor.getCorrectedPPM(t, h);
  int ppm = gasSensor.getPPM();
  int rzero = gasSensor.getRZero();
  int rzeroc = gasSensor.getCorrectedRZero(t, h);

  if (isnan(h) || isnan(t)) {
    return;
  }
  // main data from sensors
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("PPM: ");
  Serial.println(ppm);
  Serial.print("PPMCor: ");
  Serial.println(ppmc);
  Serial.println();
  
  Serial.print("RZeroc: ");
  Serial.println(rzeroc);
  Serial.print("AO: ");
  Serial.println(A0);
  Serial.print("RZero: ");
  Serial.println(rzero);
  Serial.println();

  if (ppmc > 200) {
    
    // the first way to turn off the light
    lcd.noBacklight();
      delay(2000);
      
    // the second way to turn off the light (if false)
    lcd.setBacklight(true);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("H: ");
    lcd.print(h);
    lcd.setCursor(11, 0);
    lcd.print("T: ");
    lcd.print(t);
    lcd.setCursor(3, 1);
    lcd.print("!PPM: ");
    lcd.print(ppmc);
    
  } else if (ppmc < 200) {

    if (ppmc < 170) {
      lcd.noBacklight();
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("H: ");
    lcd.print(h);
    lcd.setCursor(11, 0);
    lcd.print("T: ");
    lcd.print(t);
    lcd.setCursor(4, 1);
    lcd.print("PPM: ");
    lcd.print(ppmc);
  }
}

void loop() {
  // Turn on display light by sensor button :)
  if (digitalRead(buttonPin)) {
    lcd.backlight();
    delay(3000);
    lcd.noBacklight();
  }
  timer.run();
}
