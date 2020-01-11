/*
  Nixie tube clock arduino sketch
  Designed and created by Mistral
*/

#include <Wire.h>
#include "RTClib.h"

#define   SECOND     3
#define   LATCHPIN   5
#define   CLOCKPIN   6
#define   DATAPIN    4

RTC_DS1307 RTC;

// Update nixie?
boolean update = true;

// No of random gen numbers on each hour?
int randomCount = 300;

// Time storage as individual digits
int h1 = 0;
int h2 = 0;
int m1 = 0;
int m2 = 0;
int s1 = 0;
int s2 = 0;

// Temp Time Allocation
int hours = 0;
int minutes = 0;
int seconds = 0;

// Binary number representation in nibbles
int bn0[] = {0,0,0,0};
int bn1[] = {0,0,0,1};
int bn2[] = {0,0,1,0};
int bn3[] = {0,0,1,1};
int bn4[] = {0,1,0,0};
int bn5[] = {0,1,0,1};
int bn6[] = {0,1,1,0};
int bn7[] = {0,1,1,1};
int bn8[] = {1,0,0,0};
int bn9[] = {1,0,0,1};

void setup() {
  Serial.begin(115200);
  Wire.begin();
  RTC.begin();
  
  // Setup pins for 74HC595 Shift Registers
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);
  
  // Setup pin for 1Hz from RTC
  pinMode(SECOND, INPUT);
  
}

void loop() {
  DateTime now = RTC.now();

  // Update local stored time
  updateTime(String(now.hour()), String(now.minute()), String(now.second()));
  
  // If new second, update
  if (digitalRead(SECOND) == 1) {
    if (seconds != s2) {
      seconds = s2;
      update = true;
    }
  }
  
  // If new hour then cycle randomCount no of chars on nixies
  if (h2 == 0) {
    String randH1 = String(h1);
    String randH2 = String(h2);
    String randM1 = String(m1);
    String randM2 = String(m2);
    String randS1 = String(s1);
    String randS2 = String(s2);
    for (int i = 0; i < randomCount; i++) {
      h1 = random(10);
      h2 = random(10);
      m1 = random(10);
      m2 = random(10);
      s1 = random(10);
      s2 = random(10);
      
      if (i >= (randomCount/10)*8) {
        m1 = randM1.toInt();
        delay(15);
      }
      if (i >= (randomCount/10)*8.5) {
        h2 = randH2.toInt();
        delay(15);
      }
      if (i >= (randomCount/10)*9) {
        h1 = randH1.toInt();
        delay(15);
      }
      if (i >= (randomCount/10)*9.5) {
        m2 = randM2.toInt();
        delay(15);
      }
      
      updateTime(
        String(h1) + String(h2), 
        String(m1) + String(m2), 
        String(s1) + String(s2)
      );
      setNixie();
      printTime();
      delay(30);
    }
    updateTime(String(now.hour()), String(now.minute()), String(now.second()));
  }
  
  if (update) {
    setNixie();
    printTime();
    update = false;
  }
}

void updateTime(String hourss, String minutess, String secondss) {
  
  if (hourss.toInt() < 10) hourss = "0" + hourss;
  if (minutess.toInt() < 10) minutess = "0" + minutess;
  if (secondss.toInt() < 10) secondss = "0" + secondss;
  
  String tmph1 = hourss.substring(0,1);
  String tmph2 = hourss.substring(1);
  String tmpm1 = minutess.substring(0,1);
  String tmpm2 = minutess.substring(1);
  String tmps1 = secondss.substring(0,1);
  String tmps2 = secondss.substring(1);
  
  h1 = tmph1.toInt();
  h2 = tmph2.toInt();
  m1 = tmpm1.toInt();
  m2 = tmpm2.toInt();
  s1 = tmps1.toInt();
  s2 = tmps2.toInt();
  
}

void setNixie() {
  digitalWrite(LATCHPIN, LOW);
  //updateRegister(compare(s2));
  //updateRegister(compare(s1));
  updateRegister(compare(m2));
  updateRegister(compare(m1));
  updateRegister(compare(h2));
  updateRegister(compare(h1));
  digitalWrite(LATCHPIN, HIGH);
  Serial.println("");
}

void updateRegister(int* binaryNumber) {
  
  sendToShiftRegister(DATAPIN, CLOCKPIN, binaryNumber);
  
}

void tick() {
  // activate relay on second...
  // maybe not a relay...
  // too much clicking!
  // 
}

void printTime() {
  Serial.print(h1);
  Serial.print(h2);
  Serial.print(":");
  Serial.print(m1);
  Serial.print(m2);
  Serial.print(":");
  Serial.print(s1);
  Serial.println(s2);
}

int* compare(int i) {
  switch(i) {
    case 0:
      return bn0;
    case 1:
      return bn1;
    case 2:
      return bn2;
    case 3:
      return bn3;
      break;
    case 4:
      return bn4;
    case 5:
      return bn5;
    case 6:
      return bn6; 
    case 7:
      return bn7;
    case 8:
      return bn8;
    case 9:
      return bn9;
    default:
      return bn0;
  }
}

void sendToShiftRegister(uint8_t dataPin, uint8_t clockPin, int* bn) {
  for (int i = 0; i < 4; i++)  {                                                                                                                              
    digitalWrite(DATAPIN, bn[i]);
    digitalWrite(CLOCKPIN, HIGH);
    digitalWrite(CLOCKPIN, LOW);
  }
}
