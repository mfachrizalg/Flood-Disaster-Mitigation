#define BLYNK_TEMPLATE_ID "TMPL6Ju5JTqle"
#define BLYNK_TEMPLATE_NAME "Flood Disaster Mitigation Kelompok 2"
#define BLYNK_AUTH_TOKEN "p6m1q3p7XpfwKS69hWSQUGNZkJeaV7cm"

#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <Arduino.h>

char ssid[] = "Wokwi-GUEST";
char pass[] = "";

#define SERVO_PIN 21 
#define TRIG_PIN 18 // ESP32 pin GIOP23 connected to Ultrasonic Sensor's TRIG pin
#define ECHO_PIN 5 // ESP32 pin GIOP22 connected to Ultrasonic Sensor's ECHO pin
#define LED 2
#define LED2 4
#define LED3 13
#define buzzer 12
#define pos 0;

int16_t i;

float duration_us, distance_cm;
float waterHeight;
float volume;
float drainHeight = 400;
float drainWide = 150;

Servo servo;
BlynkTimer timer;
LiquidCrystal_I2C lcd(0x27,16,2);

const int sensorReadingInterval = 1000;
unsigned long lastSensorReadingTime = 0;

void myTimer() 
{
  Blynk.virtualWrite(V4, i);  
  Blynk.virtualWrite(V0, LED);
  Blynk.virtualWrite(V1, LED2);
  Blynk.virtualWrite(V2, LED3);
  Blynk.virtualWrite(V5, waterHeight);
}

void setup()
{
  Wire.begin(23, 22);
  Serial.begin(9600);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimer); 
  
  pinMode(TRIG_PIN, OUTPUT);// configure the echo pin to input mode
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(buzzer, OUTPUT);
  
  servo.attach(SERVO_PIN);
  servo.write(0);
  lcd.init();
  lcd.backlight();
}

void loop()
{
  Blynk.run();
  timer.run();

  unsigned long currentTime = millis();
  if ((currentTime - lastSensorReadingTime) < sensorReadingInterval){
    return;
  }
  lastSensorReadingTime - currentTime;
  
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration_us = pulseIn(ECHO_PIN, HIGH);
  
  distance_cm = 0.017 * duration_us;

  waterHeight = drainHeight - distance_cm;
  volume =  waterHeight * drainWide;

  i = analogRead(34);
  String msg = i < 2165 ? "VERY WET" : i > 3135 ? "DRY" : "NORMAL";

  if (distance_cm < 30) {
    digitalWrite(LED, HIGH);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    Blynk.virtualWrite(V0, 1);
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 0);
    servo.write(90);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Soil: ");
    lcd.print(msg);
    lcd.setCursor(0,1);
    lcd.print("KONDISI: BAHAYA");
    tone(buzzer, 500);
    delay(3000);
    noTone(buzzer);
  }

  else if (distance_cm > 31 && distance_cm < 75) {
    digitalWrite(LED, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, HIGH);
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 0);
    Blynk.virtualWrite(V2, 1);
    servo.write(0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Soil: ");
    lcd.print(msg);
    lcd.setCursor(0,1);
    lcd.print("KONDISI: ANCAMAN");
  }

  else if (distance_cm > 76){
    digitalWrite(LED, LOW);
    digitalWrite(LED2, HIGH);
    digitalWrite(LED3, LOW);
    Blynk.virtualWrite(V0, 0);
    Blynk.virtualWrite(V1, 1);
    Blynk.virtualWrite(V2, 0);
    servo.write(0);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Soil: ");
    lcd.print(msg);
    lcd.setCursor(0,1);
    lcd.print("KONDISI: AMAN");

  }

}

void Blink(int x){
  digitalWrite(x, HIGH);
  delay(10);
  digitalWrite(x, LOW);
}