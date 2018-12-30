// ---------------------------------------------------------------------------
// Program to measure water level with ultrasonic sensor
// and display it on 7 segment 4 digit display
// Author: Amandeep Singh <as.smartdevelopers@gmail.com>both
// Created: 12-04-2016
// Modified: 14-01-2018 
// ---------------------------------------------------------------------------

#include <NewPing.h> // Library for Ultra Sonic Sensor
#include <NewTone.h>
#include <TM1637Display.h> //Arduino library for TM1637 (LED Driver)
#include "pitches.h"

/*
 * Config for Ultrasonic sensor
 */
#define SONAR_NUM     2 // Number or sensors.
#define PING_INTERVAL 500 // Milliseconds between pings.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

unsigned long pingTimer[SONAR_NUM]; // When each pings.
unsigned int cm[SONAR_NUM]; // Store ping distances.
unsigned int per[SONAR_NUM]; // Store distances percentage.
uint8_t currentSensor = 0; // Which sensor is active.
 
#define US1_TRIG_PIN  7  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define US1_ECHO_PIN  6  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define US2_TRIG_PIN  9  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define US2_ECHO_PIN  8  // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing sonar[SONAR_NUM] = { // Sensor object array.
  NewPing(US1_TRIG_PIN, US1_ECHO_PIN, MAX_DISTANCE), // NewPing UC1 setup of pin and maximum distance.
  NewPing(US2_TRIG_PIN, US2_ECHO_PIN, MAX_DISTANCE) // NewPing UC2 setup of pin and maximum distance.
};

/*
 * Configuration for 7segment Display
 */
// Display 1 
const int CLK1 = 3; //Set the CLK pin connection to the display1
const int DIO1 = 2; //Set the DIO pin connection to the display1

// Display 2
const int CLK2 = 5; //Set the CLK pin connection to the display2
const int DIO2 = 4; //Set the DIO pin connection to the display2

TM1637Display display[SONAR_NUM] = {
  TM1637Display(CLK1, DIO1),
  TM1637Display(CLK2, DIO2)
};
//TM1637Display display(CLK1, DIO1); //set up the 4-Digit Display1.
//TM1637Display display(CLK2, DIO2); //set up the 4-Digit Display2. 

/*
 * LED to show the moter status
 */
//const int LEDPin = 10;

/*
 * Buzzer and melody configuration
 */
const int buzzerPin = 13; 

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

int alarmButton = 12;
boolean alarmSwitchStatus = false;

/*
 * Config for Water Tank Level
 */

typedef struct {
   int MAX;
   int MIN;
} Tank_Water_Level;

Tank_Water_Level TWL[SONAR_NUM] = {
   { 20, 102 },
   { 20, 112 }
};
 
/*// Tank 1 
#define TANK1_MAX_WATER_LEVEL 20 // Height in cm at which water level is maximum
#define TANK1_MIN_WATER_LEVEL 102 // Height in cm at which water level is minimum

// Tank 2
#define TANK2_MAX_WATER_LEVEL 20 // Height in cm at which water level is maximum
#define TANK2_MIN_WATER_LEVEL 112 // Height in cm at which water level is minimum
*/

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  //pinMode(LEDPin, OUTPUT); // LED Pin setup
  pinMode(alarmButton, INPUT_PULLUP); // set the internal pull up resistor, unpressed button is HIGH 
  display[0].setBrightness(0x0a); //set the diplay to maximum brightness
  display[1].setBrightness(0x0a); //set the diplay to maximum brightness

  pingTimer[0] = millis() + 75; // First ping start in ms.

  for (uint8_t i = 1; i < SONAR_NUM; i++)
    pingTimer[i] = pingTimer[i - 1] + PING_INTERVAL;
    
}

void loop() {

  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    if (millis() >= pingTimer[i]) {
      pingTimer[i] += PING_INTERVAL * SONAR_NUM;
      if (i == 0 && currentSensor == SONAR_NUM - 1)
        oneSensorCycle(); // Do something with results.
      sonar[currentSensor].timer_stop();
      currentSensor = i;
      cm[currentSensor] = 0;
      sonar[currentSensor].ping_timer(echoCheck);
    }
  }

  if ( digitalRead(alarmButton) == 0) {
    // button was pressed
    //do whatever the action is
    alarmSwitchStatus = true;
  } else {
    alarmSwitchStatus = false;
  }
    
  /*int usVal;
  int per;

  // Get ultrasonic sensor value
  usVal = usValueInCm();
  Serial.print(usVal);
  Serial.print("cm");
  Serial.print("\n");


  // Get percentage of water level
  per = waterLevelInPercent(usVal);

  // Serial Print
  Serial.print(per);
  Serial.print("%");
  Serial.print("\n");

  int val = 0;
  val = digitalRead(LEDPin);
    
  if(per < 30) {
    int val = 0;
    val = digitalRead(LEDPin);
    if (val != 1) {
      Serial.print("Yahoo");
      digitalWrite(LEDPin, HIGH);
    }    
  }

  if(per > 90) {
    digitalWrite(LEDPin, LOW);
  }
  display.showNumberDec(per); //Display the numCounter value;

  delay(1000); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

  */
}

void echoCheck() { // If ping echo, set distance to array.
  if (sonar[currentSensor].check_timer())
    cm[currentSensor] = sonar[currentSensor].ping_result / US_ROUNDTRIP_CM;
}

void oneSensorCycle() { // Do something with the results.
  for (uint8_t i = 0; i < SONAR_NUM; i++) {
    Serial.print(i);
    Serial.print("=");
    Serial.print(cm[i]);
    Serial.print("cm ");
    Serial.print("\n");
    
    // Get percentage of water level
    per[i] = waterLevelInPercent(cm[i], i);

    // Serial Print
    Serial.print(per[i]);
    Serial.print("%");
    Serial.print("\n");

    display[i].showNumberDec(per[i]); //Display the numCounter value;

    if(i == 0 && per[i] >= 90 && alarmSwitchStatus) {
      playAlarm();
    }
  }

  
}

/*
 * Function: waterLevelInPercent
 * Description: To get the water level in percent
 * Return: Interger
 */
int waterLevelInPercent(int usVal, int i) {
  int cusVal = usVal;

  // Check if ultrasonic value is pass our condition 
  if (usVal <= TWL[i].MAX) {
    usVal = TWL[i].MAX;
  } else if (usVal >= TWL[i].MIN) {
    usVal = TWL[i].MIN;
  } else {
    usVal = usVal - TWL[i].MAX;
  }

  // Get water tank level length in centimeters
  int tankLength = TWL[i].MIN - TWL[i].MAX;

  // Get water level value in percentage
  float percentage = 0;

  if(( cusVal  > TWL[i].MAX)&&( cusVal < TWL[i].MIN ) ){
    percentage = (((float(tankLength) - float(usVal))/float(tankLength)) * 100);
  } else if (cusVal >= TWL[i].MIN){
    percentage = 0;
  } else {
    percentage = 100.00;
  }
  
  return (int) percentage;
}

void playAlarm() {
  // iterate over the notes of the melody:
  //while(true) {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
  
      // to calculate the note duration, take one second
      // divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      NewTone(buzzerPin, melody[thisNote], noteDuration);
  
      // to distinguish the notes, set a minimum time between them.
      // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      // stop the tone playing:
      noNewTone(buzzerPin);
    }
  //}    
}
