#include "MQ135.h"
#include "DHT.h"
#include <TimeLib.h>
#include <SimpleTimer.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);

// Need to calibrate at ones.
// Humidity 30%, Temperature 20 C.
MQ135 gasSensor = MQ135(A0);

SimpleTimer timer;

// finger sensor button
const int buttonPin = 6;
// Led
const int led = 3;

/**
 * Relay for manage display light.
 * On the back side of I2C there is
 * jumper for turning in the light.
 * It is changed on relay that connect
 * to arduino digital port.
 * HiGH signal - light ON
 *//
const int displayLight = 5;

/**
 * If i2c board using:
 * set SDA pin to A4
 * set SCL pin to A5
**/

// Display port might be 0x27, 0x3f, 0x63.
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(led, OUTPUT);
  pinMode(displayLight, OUTPUT);
  pinMode(buttonPin, INPUT);

  lcd.init();
  lcd.backlight();
  digitalWrite(displayLight, HIGH);

  // 1st param = number of symbol to start string
  // 2nd param = number of line (0 or 1)
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Game");
  lcd.setCursor(0, 1);
  lcd.print("Ventilate Or Die");
  delay(5000);
  lcd.clear();
  timer.setInterval(5000L, sendTemps);
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
  // main data from sensors to console
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("PPMCor: ");
  Serial.println(ppmc);
  Serial.println();

  Serial.print("RZeroc: ");
  Serial.println(rzeroc);
  Serial.print("PPM: ");
  Serial.println(ppm);
  Serial.print("AO: ");
  Serial.println(A0);
  Serial.print("RZero: ");
  Serial.println(rzero);
  Serial.println();

  if (ppmc > 200) {
    digitalWrite(led, HIGH);
    digitalWrite(displayLight, HIGH);
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Need to");
    lcd.setCursor(0, 1);
    lcd.print("Ventilate");
    lcd.setCursor(12, 0);
    lcd.print("!");
    lcd.print(ppmc);
  } else {
    
    if (ppmc < 170) {
      // Turn off display light
      digitalWrite(displayLight, LOW);
    }
    // Display tempereture, humidity & PPM.
    digitalWrite(led, LOW);
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
    digitalWrite(displayLight, HIGH);
    delay(3000);
    digitalWrite(displayLight, LOW);
  }
  timer.run();
}
