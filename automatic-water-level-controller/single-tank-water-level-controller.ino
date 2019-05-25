// ---------------------------------------------------------------------------
// Program to measure water level with ultrasonic sensor
// and display it on 7 segment 4 digit display
// Author: Amandeep Singh <as.smartdevelopers@gmail.com>
// Created: 12-04-2016
// Modified: 14-01-2018 
// ---------------------------------------------------------------------------

#include <NewPing.h> // Library for Ultra Sonic Sensor
#include <TM1637Display.h> //Arduino library for TM1637 (LED Driver)

/*
 * Config for Ultrasonic sensor
 */
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
#define US_TRIG_PIN  8  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define US_ECHO_PIN  9  // Arduino pin tied to echo pin on the ultrasonic sensor.

NewPing sonar(US_TRIG_PIN, US_ECHO_PIN, MAX_DISTANCE);

/*
 * Configuration for 7segment Display
 */
// Display 
const int CLK = 2; //Set the CLK pin connection to the display
const int DIO = 3; //Set the DIO pin connection to the display

TM1637Display display(CLK, DIO);

/*
 * Config for Water Tank Level
 */
const int MAX_WATER_LEVEL = 20; // Height in cm at which water level is maximum
const int MIN_WATER_LEVEL = 200; // Height in cm at which water level is minimum 

void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  //pinMode(LEDPin, OUTPUT); // LED Pin setup
  display.setBrightness(0x0a); //set the diplay to maximum brightness  
}

void loop() {

  int usVal;
  int per;

  // Get ultrasonic sensor value
  usVal = usValueInCm();
  if(usVal != 0) {
  Serial.print(usVal);
  Serial.print("cm");
  Serial.print("\n");


  // Get percentage of water level
  per = waterLevelInPercent(usVal);

  // Serial Print
  Serial.print(per);
  Serial.print("%");
  Serial.print("\n");

  display.showNumberDec(per); //Display the numCounter value;
  }
  delay(1000); // Wait 50ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

}

/*
 * Function: usPingValueInCm
 * Description: To get the Ultrasonic sensor value using New ping
 * Return: Interger
 */
int usValueInCm(){
  int t = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  t = t / US_ROUNDTRIP_CM;
  return t;
}

/*
 * Function: waterLevelInPercent
 * Description: To get the water level in percent
 * Return: Interger
 */
int waterLevelInPercent(int usVal) {
  int cusVal = usVal;

  // Check if ultrasonic value is pass our condition 
  if (usVal <= MAX_WATER_LEVEL) {
    usVal = MAX_WATER_LEVEL;
  } else if (usVal >= MIN_WATER_LEVEL) {
    usVal = MIN_WATER_LEVEL;
  } else {
    usVal = usVal - MAX_WATER_LEVEL;
  }

  // Get water tank level length in centimeters
  int tankLength = MIN_WATER_LEVEL - MAX_WATER_LEVEL;

  // Get water level value in percentage
  float percentage = 0;

  if(( cusVal  > MAX_WATER_LEVEL)&&( cusVal < MIN_WATER_LEVEL ) ){
    percentage = (((float(tankLength) - float(usVal))/float(tankLength)) * 100);
  } else if (cusVal >= MIN_WATER_LEVEL){
    percentage = 0;
  } else {
    percentage = 100.00;
  }
  
  return (int) percentage;
}
