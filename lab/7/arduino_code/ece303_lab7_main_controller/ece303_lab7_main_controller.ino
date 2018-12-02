#include "HX711.h"		// https://github.com/bogde/HX711
#include "EEPROMex.h"    // for writing & reading to eeprom
#include <Wire.h>        // for I2C comms with accelerometer


// HX711 Load Cell I/O
#define DOUT   5
#define CLK    2
HX711 scale(DOUT, CLK);

#define INTERRUPT_PIN    3    // digital pin 3, reads in pulses from rotary encoder
//#define LOAD_CELL_PIN    5    // analog pin 5, input pin for load cell
#define OUTPUT_PIN       12   // digital pin 12, output pin for PWM signal (linearly dependent on load cell value)
#define START_STOP_PIN   45   // digital pin 45, start/stop pin from DAQ
#define YAC_ESTOP_PIN    49   // digital pin 49, receives signal from DAQ if an alarm goes off
#define RELAY_PIN        50   // digital pin 50, relay to control motor for emergency shutdown
#define STARTSTOP_MATLAB 11   // digital pin 11, turns on when MATLAB sends start, off when stop

int start_stop_flag;

volatile int rpm = 0;    // initialize pulse counter

int duty_cycle;
int input_load = 0;
//long calibration_factor = -6266660;
double calibration_factor;

int max_load = 100;      // maximum load-cell value
//long max_rpm = 100L;     // maximum rpm value
int max_rpm;

// Accelerometer
int accelerometer_flag;  // flag for excessive tilt from accelerometer (1 for tilted, 0 for ok)
const int MPU=0x68;
int16_t AcX,AcY,AcZ;

// eeprom
const int maxAllowedWrites = 80;
const int memBase = 350;
int addressDouble;
int addressInt;

// timer
unsigned long start_millis = 0;
unsigned long current_millis;

// =============================================================================

void setup() {
     Serial.begin(9600);
     Serial1.begin(9600);

     pinMode(INTERRUPT_PIN, INPUT_PULLUP);     // use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
     attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), count_pulses, FALLING); // trigger interrupt routine on the RISING edge of the signal

     pinMode(YAC_ESTOP_PIN,   INPUT);        // digital input, estop signal from DAQ
     pinMode(START_STOP_PIN,  INPUT);        // digital input, start/stop from DAQ
     pinMode(RELAY_PIN,       OUTPUT);       // digital output, relay
     pinMode(STARTSTOP_MATLAB,OUTPUT);       // digital output, testing

     // eeprom setup
     EEPROM.setMemPool(memBase, EEPROMSizeMega);
     EEPROM.setMaxAllowedWrites(maxAllowedWrites);

     addressDouble = EEPROM.getAddress(sizeof(double));
     addressInt = EEPROM.getAddress(sizeof(int));
     readEEPROM();
     
     scale.set_scale(calibration_factor);
     scale.tare();             //Reset the scale to 0

     // Initiate I2C comms (for accelerometer)
     Wire.begin();
     Wire.beginTransmission(MPU);
     Wire.write(0x6B);
     Wire.write(0);
     Wire.endTransmission(true);
}

// =============================================================================

void loop() {

     // accelerometer thresholds
     //           8000           -8000           8000           -8000           19000           3000
     if ( (AcX <= 8000 && AcX >= -8000 && AcY <= 8000 && AcY >= -8000 && AcZ <= 19000 && AcZ >= 3000)) {
          // not tilted
          accelerometer_flag = 1;
     }
     else {
          // tilted
          accelerometer_flag = 0;
     }

     // uncomment to debug accelerometer =======================================
     // Serial.print("Accelerometer: ");
     // Serial.print("X = "); Serial.print(AcX);
     // Serial.print(" | Y = "); Serial.print(AcY);
     // Serial.print(" | Z = "); Serial.println(AcZ);

// =============================================================================

     if (digitalRead(YAC_ESTOP_PIN) == HIGH && accelerometer_flag == 1) {
          // if not in e-stop
          // must apply voltage to be out of estop
          digitalWrite(RELAY_PIN, HIGH);      // close relay
     }
     else {}
          // if in e-stop
          digitalWrite(RELAY_PIN, LOW);      // open relay & cut power to motor
    }

    // ========================================================================

     current_millis = millis();

//     scale.set_scale(calibration_factor);  //Calibration Factor obtained from first sketch
     input_load = scale.get_units()*1000;  //Up to 3 decimal points
     // Adjust load cell value so that it does not exceed range of 0-100g
     if (input_load > max_load) {
          input_load = max_load;
     }
     else if (input_load < 0) {
          input_load = 0;
     }

     // uncomment to debug load cell ===========================================
     // Serial.print("input load: ");
     // Serial.println(input_load);

     duty_cycle = map(input_load, 0, max_load, 0, 255);    // map the input voltage (from 0 to 1023) to the duty cycle output (from 0 to 255)

     // ========================================================================

     if (digitalRead(START_STOP_PIN) == HIGH) {
          // if in "start" state
     	analogWrite(OUTPUT_PIN, duty_cycle);   // write the load cell dependent duty cycle to the analog output pin
     	start_stop_flag = 1;
     }
     else {
          // if in "stop" state
     	analogWrite(OUTPUT_PIN, 0);  // no output signal to H-bridge
     	start_stop_flag = 0;
     }

     // ========================================================================

     // testing purposes
     if (start_stop_flag == 1) {
     	digitalWrite(STARTSTOP_MATLAB, HIGH);
//          analogWrite(OUTPUT_PIN, duty_cycle);   // write the load cell dependent duty cycle to the analog output pin}
     }
     else if (start_stop_flag == 0) {
     	digitalWrite(STARTSTOP_MATLAB, LOW);
//          analogWrite(OUTPUT_PIN, 0);  // no output signal to H-bridge
     }

     // ========================================================================

     // read in accelerometer data from eeprom
     Wire.beginTransmission(MPU);
     Wire.write(0x3B);
     Wire.endTransmission(false);
     Wire.requestFrom(MPU,12,true);

     AcX=Wire.read()<<8|Wire.read();
     AcY=Wire.read()<<8|Wire.read();
     AcZ=Wire.read()<<8|Wire.read();

     // ========================================================================

     if (current_millis - start_millis >= 1000) {
          Serial.print("RPM: ");                    // print pulse counter every second
          Serial.println(rpm);
          Serial.print("Input load: ");
          Serial.println(input_load);
//          Serial.println();
//          Serial.println();
          Serial1.flush();
          Serial1.write(rpm % 256);
          Serial1.write(input_load);              //% 256);
          rpm = 0;                                // reset the pulse counter to 0
          start_millis = current_millis;

          // read in max load cell & rpm from DAQ
          int b1 = Serial1.read();      // (calibration_matlab_int / 256) / 256
          int b2 = Serial1.read();      // calibration_matlab_int / 256) % 256
          int b3 = Serial1.read();      // calibration_matlab_int % 256
          int b4 = Serial1.read();      // rpm_matlab_int / 256
          int b5 = Serial1.read();      // rpm_matlab_int % 256

          // convert individual b's received to full number
          long calibration_matlab_b = (long)(b1)*256*256  + (long)(b2)*256 + (long)(b3);
          calibration_matlab_b *= -1;   // since received number will be postiive
          long max_rpm_matlab_b = (long)(b4)*256 + (long)(b5);

          if (calibration_matlab_b != 65793) {
               calibration_factor = (double)(calibration_matlab_b);
          }

          // uncomment to debug calibration_factor =============================
          // Serial.print("calibration_factor: ");
          // Serial.println(calibration_factor);

          if (max_rpm_matlab_b != -257) {
               max_rpm = (int)(max_rpm_matlab_b);
          }
          
          updateEEPROM();

          // uncomment to debug max_rpm ========================================
          // Serial.print("max_rpm: ");
          // Serial.println(max_rpm);

          Serial.println();
          Serial.println();

     }

}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
     // increase pulse counter by one
     rpm++;
}


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
