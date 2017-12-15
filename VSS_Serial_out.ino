// Matthew McMillan 
// @matthewmcmillan
// http://matthewcmcmillan.blogspot.com
//
// Digital speedometer
//
// VSS on car connects to pin 5
// CLK on display to Analog pin 5
// DAT on display to Analog pin 4
//
// Code adapted from Matthew McMilan by Markus Ippy . Removed 7 Segment LCD code and replaced with Serial Output


#include <SPI.h>
#include <Wire.h>


const int hardwareCounterPin = 5;
const int samplePeriod = 1000; //in milliseconds
const float pulsesPerMile = 4000; // this is pulses per mile for Toyota. Other cars are different.
const float convertMph = pulsesPerMile/3600;
unsigned int count;
float mph;
unsigned int imph;
int roundedMph;
int previousMph;
int prevCount;

const int numReadings = 30;     // the number of readings for average brightness
int readings[numReadings];      // the readings array for the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int average = 3; 



void setup(void) {
  Serial.begin(9600);
  Serial.println("Start");
  

  TCCR1A = 0; //Configure hardware counter 
  TCNT1 = 0;  // Reset hardware counter to zero
}

void loop() {

  /////////////////////////////////////////////////////////////
  // This uses the hardware pulse counter on the Arduino.
  // Currently it collects samples for one second.
  //
  bitSet(TCCR1B, CS12); // start counting pulses
  bitSet(TCCR1B, CS11); // Clock on rising edge
  delay(samplePeriod); // Allow pulse counter to collect for samplePeriod
  TCCR1B = 0; // stop counting
  count = TCNT1; // Store the hardware counter in a variable
  TCNT1 = 0;     // Reset hardware counter to zero
  mph = (count/convertMph)*10; // Convert pulse count into mph.
  imph = (unsigned int) mph; // Cast to integer. 10x allows retaining 10th of mph resolution.

  int x = imph / 10;
  int y = imph % 10;
  
  // Round to whole mile per hour
  if(y >= 5){
    roundedMph = x + 1;
  }else{
    roundedMph = x;
  }
  
  //If mph is less than 1mph just show 0mph.
  //Readings of 0.9mph or lower are some what erratic and can
  //occasionally be triggered by electrical noise.
  if(x == 0){
    roundedMph = 0;
  }
  
  // Don't display mph readings that are more than 50 mph higher than the 
  // previous reading because it is probably a spurious reading.
  // Accelerating 50mph in one second is rocketship fast so it is probably
  // not real.
  if((roundedMph - previousMph) > 50){
    Serial.println(previousMph);
  }else{
    Serial.println(roundedMph);
  }
  
  
  previousMph = roundedMph; // Set previousMph for use in next loop.
}
