#include "HX711.h"
#include "EEPROMex.h"
 
#define DOUT  5
#define CLK  2
 
HX711 scale(DOUT, CLK);

#define INTERRUPT_PIN  3  // digital pin 2, reads in pulses from rotary encoder
#define OUTPUT_PIN     12 // digital pin 12, output pin for PWM signal (linearly dependent on load cell value)
#define YAC_ESTOP_PIN  49 // digital pin 49, receives signal from DAQ if an alarm goes off
#define RELAY_PIN      50 // digital pin 50, relay to control motor for emergency shutdown
#define START_STOP_PIN 45
int start_stop_flag;

volatile int rpm = 0;  // initialize pulse counter

int duty_cycle;
int input_load;

double calibration_factor;
int max_rpm;
int max_load = 100;
const int maxAllowedWrites = 80;
const int memBase = 350;
int addressDouble;
int addressInt;

unsigned long start_millis = 0;
unsigned long current_millis;

void updateEEPROM() { 
  EEPROM.updateDouble(addressDouble, calibration_factor);
  scale.set_scale(calibration_factor);  //Calibration Factor obtained from first sketch
  scale.tare();             //Reset the scale to 0 NEED TO TEST IF WE NEED THIS LINE WITH LOAD CELL AND GUI
  EEPROM.updateInt(addressInt, max_rpm);   
}

void readEEPROM() { 
  if (EEPROM.readDouble(addressDouble) > 0) {
    calibration_factor = EEPROM.readDouble(addressDouble);
  }
  else {
    calibration_factor = -6266660;
  }

  if (EEPROM.readInt(addressInt) > 0) {
    max_rpm = EEPROM.readInt(addressInt);
  }
  else {
    max_rpm = 200; // IS THIS RIGHT?
  }
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  EEPROM.setMemPool(memBase, EEPROMSizeMega);
  EEPROM.setMaxAllowedWrites(maxAllowedWrites);

  addressDouble = EEPROM.getAddress(sizeof(double));
  addressInt = EEPROM.getAddress(sizeof(int));
  readEEPROM();

  scale.set_scale(calibration_factor);
  scale.tare();             //Reset the scale to 0
    
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);     // use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), count_pulses, RISING); // trigger interrupt routine on the RISING edge of the signal

  pinMode(RELAY_PIN, OUTPUT);               // digital output pin for relay
  pinMode(YAC_ESTOP_PIN, INPUT);        // digital input pin for estop signal from DAQ

  pinMode(START_STOP_PIN, INPUT);
  pinMode(11, OUTPUT);

}

void loop() {

  if (digitalRead(YAC_ESTOP_PIN) == LOW) {
    digitalWrite(RELAY_PIN, HIGH);      // if out of e-stop, close relay
  }
  else if (digitalRead(YAC_ESTOP_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW);      // open relay & cut power to motor
  }
  
  current_millis = millis();

  input_load = scale.get_units()*1000;  //Up to 3 decimal points
  // Adjust load cell value so that it does not exceed range of 0-100g
  if (input_load > max_load) {
    input_load = max_load;
  }
  else if (input_load < 0) {
    input_load = 0;
  }
  
  duty_cycle = map(input_load, 0, 100, 0, 255);    // map the input load (from 0 to 100) to the duty cycle output (from 0 to 255)
  
  if (digitalRead(START_STOP_PIN) == HIGH)
  {
    analogWrite(OUTPUT_PIN, duty_cycle);                // write the load cell dependent duty cycle to the analog output pin
    start_stop_flag = 1;
  }
  else
  {
    analogWrite(OUTPUT_PIN, 0);
//    digitalWrite(13, LOW);
    start_stop_flag = 0;
  }

  if (start_stop_flag == 1)
    digitalWrite(11, HIGH);
  else if (start_stop_flag == 0)
    digitalWrite(11, LOW);
    
  if (current_millis - start_millis >= 1000)
  {
    Serial.println(rpm);                              // print pulse counter every second
    Serial1.write(rpm % 256);
    Serial1.write(input_load); //% 256);
    rpm = 0;                                         // reset the pulse counter to 0
    start_millis = current_millis;
  }

}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
  // increase pulse counter by one
  rpm++;
}
