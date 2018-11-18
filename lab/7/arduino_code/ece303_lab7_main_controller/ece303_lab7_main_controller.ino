#include "HX711.h"		// https://github.com/bogde/HX711

// HX711 Load Cell I/O
#define DOUT   5
#define CLK    2
HX711 scale(DOUT, CLK);

#define INTERRUPT_PIN    3    // digital pin 3, reads in pulses from rotary encoder
#define LOAD_CELL_PIN    5    // analog pin 5, input pin for load cell
#define OUTPUT_PIN       12   // digital pin 12, output pin for PWM signal (linearly dependent on load cell value)
#define START_STOP_PIN   45   // digital pin 45, start/stop pin from DAQ
#define YAC_ESTOP_PIN    49   // digital pin 49, receives signal from DAQ if an alarm goes off
#define RELAY_PIN        50   // digital pin 50, relay to control motor for emergency shutdown

int start_stop_flag;

volatile int rpm = 0;    // initialize pulse counter

int duty_cycle;
int input_load;
float calibration_factor = -6266660;

int max_load = 100;      // maximum load-cell value

unsigned long start_millis = 0;
unsigned long current_millis;

// =============================================================================

void setup() {
     Serial.begin(9600);
     Serial1.begin(9600);

     scale.set_scale();
     scale.tare();             //Reset the scale to 0

     pinMode(INTERRUPT_PIN, INPUT_PULLUP);     // use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
     attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), count_pulses, RISING); // trigger interrupt routine on the RISING edge of the signal

     pinMode(YAC_ESTOP_PIN,   INPUT);        // digital input, estop signal from DAQ
     pinMode(START_STOP_PIN,  INPUT);        // digital input, start/stop from DAQ
     pinMode(RELAY_PIN,       OUTPUT);       // digital output, relay
     pinMode(11,              OUTPUT);       // digital output, testing
}

// =============================================================================

void loop() {

     if (digitalRead(YAC_ESTOP_PIN) == LOW) {
          // if in e-stop
          digitalWrite(RELAY_PIN, HIGH);      // close relay
     }
     else if (digitalRead(YAC_ESTOP_PIN) == HIGH) {
          // if out of e-stop
          digitalWrite(RELAY_PIN, LOW);      // open relay & cut power to motor
     }

     current_millis = millis();

     scale.set_scale(calibration_factor);  //Calibration Factor obtained from first sketch
     input_load = scale.get_units()*1000;  //Up to 3 decimal points
     // Adjust load cell value so that it does not exceed range of 0-100g
     if (input_load > max_load)
          input_load = max_load;
     else if (input_load < 0)
          input_load = 0;

     duty_cycle = map(input_load, 0, 1023, 0, 255);    // map the input voltage (from 0 to 1023) to the duty cycle output (from 0 to 255)

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

     // testing purposes
     if (start_stop_flag == 1)
     	digitalWrite(11, HIGH);
     else if (start_stop_flag == 0)
     	digitalWrite(11, LOW);


     if (current_millis - start_millis >= 1000) {
     Serial.println(rpm);                    // print pulse counter every second
     Serial.println(input_load);
     Serial.println();
     Serial1.write(rpm % 256);
     Serial1.write(input_load);              //% 256);
     rpm = 0;                                // reset the pulse counter to 0
     start_millis = current_millis;

     // read in max load cell from DAQ
     int max_load_matlab_bytes = Serial1.read();
     max_load = max_load_matlab_bytes;
     Serial.println(max_load);

     }

}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
     // increase pulse counter by one
     rpm++;
}