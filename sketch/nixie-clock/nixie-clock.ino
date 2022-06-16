/**
 * Nixie Tube Clock using DS1307 RTC 
 * module and 74HC595 shift registers
 * 
 * Author: Jazer Barclay
 */

#include <Wire.h>
#include <RTClib.h>

#define   DATAPIN    4
#define   LATCHPIN   5
#define   CLOCKPIN   6

RTC_DS1307 rtc;

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

// Store the last second value read
int ls = 0; 

void setup () {
  
  // Wait for serial to be ready
  while (!Serial);

  // Start serial
  Serial.begin(57600);
   
  // Setup pins for shift registers
  pinMode(LATCHPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);

  // If rtc module missing then halt
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  
}


void loop () {
  
  // Get time from rtc
  DateTime now = rtc.now();

  // If second updates then update display 
  if (ls != now.second()) {

    // Store new second value for next check
    ls = now.second();
    
    // Every hour shuffle numbers
    if (now.second() == 0 && now.minute() == 0)
      shuffleDisplay(now.hour(), now.minute(), 400);

    // Set display to current time
    updateDisplay(now.hour(), now.minute());

    // Wait a bit before next check
    delay(500);
  }
  
}

/**
 * Shuffles the numbers on the display randomly
 * with each digit locking in after some time
 * 
 * int hour - The actual hour
 * int min  - The actual minute
 * int shuffleCount - Number of times to randomise
 */
void shuffleDisplay(int hour, int min, int shuffleCount) {

  // For each until shuffle count
  for (int i = 1; i <= shuffleCount; i++) {

    // Update display with random values (lock in based on ternary if)
    updateDisplay( 
      ((i > ((shuffleCount/4)*2)) ? (hour/10)*10 : random(10)*10) + 
      ((i > ((shuffleCount/4)*4)) ?  hour%10     : random(10)), 
      ((i > ((shuffleCount/4)*1)) ? (min/10)*10  : random(10)*10) + 
      ((i > ((shuffleCount/4)*3)) ?  min%10      : random(10))
    );

    // Wait 15ms and as iteration increases, increase delay
    delay(15);
    if (i > shuffleCount / 4) delay(5);
    if (i > shuffleCount / 3) delay(10);
    if (i > shuffleCount / 2) delay(15);
  }
  
}

/**
 * Writes the hour and minute to shift registers
 */
void updateDisplay(int hour, int min) {
  digitalWrite(LATCHPIN, LOW);
  sendToShiftRegister(compare(min%10));
  sendToShiftRegister(compare(min/10));
  sendToShiftRegister(compare(hour%10));
  sendToShiftRegister(compare(hour/10));
  digitalWrite(LATCHPIN, HIGH);
}

/**
 * Send array of bits to shift register
 */
void sendToShiftRegister(int* bn) {
  for (int i = 0; i < 4; i++) {
    digitalWrite(DATAPIN, bn[i]);
    digitalWrite(CLOCKPIN, HIGH);
    digitalWrite(CLOCKPIN, LOW);
  }
}

/**
 * Returns the nibble associated with 
 * the given decimal value
 */
int* compare(int i) {
  switch(i) {
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
    case 0:
    default:
      return bn0;
  }
}
