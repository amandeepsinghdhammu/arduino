// ---------------------------------------------------------------------------
// Program to measure water level with ultrasonic sensor
// and display it on 7 segment 4 digit display
// ---------------------------------------------------------------------------

#include <NewPing.h> // Library for Ultra Sonic Sensor
#include <TM1637Display.h> //Arduino library for TM1637 (LED Driver)

/*
 * Config for Ultrasonic sensor
 */
#define US_PING_PIN  11  // Arduino pin tied to both trigger and echo pins on the ultrasonic sensor.
#define MAX_DISTANCE 300 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(US_PING_PIN, US_PING_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

/*
 * Configuration for 7segment
 */
const int CLK = 3; //Set the CLK pin connection to the display
const int DIO = 2; //Set the DIO pin connection to the display

TM1637Display display(CLK, DIO); //set up the 4-Digit Display. 

/*
 * LED to show the moter status
 */

const int LEDPin = 4;
 
/*
 * Config for Water Tank Level
 */
#define MAX_WATER_LEVEL 1 // Height in cm at which water level is maximum
#define MIN_WATER_LEVEL 17 // Height in cm at which water level is minimum



void setup() {
  Serial.begin(115200); // Open serial monitor at 115200 baud to see ping results.
  pinMode(LEDPin, OUTPUT); // LED Pin setup
  display.setBrightness(0x0a); //set the diplay to maximum brightness
}

void loop() {
  int usVal;
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
