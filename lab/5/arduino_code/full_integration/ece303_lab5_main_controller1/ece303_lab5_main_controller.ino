#define INTERRUPT_PIN 2  // digital pin 2, reads in pulses from rotary encoder
#define LOAD_CELL_PIN 5  // analog pin 5, input pin for load cell
#define OUTPUT_PIN    12 // digital pin 12, output pin for PWM signal (linearly dependent on load cell value)
#define YAC_ESTOP_PIN 49 // digital pin 49, receives signal from DAQ if an alarm goes off
#define RELAY_PIN     50 // digital pin 50, relay to control motor for emergency shutdown

volatile int rpm = 0;  // initialize pulse counter

int duty_cycle;
int input_load;

unsigned long start_millis = 0;
unsigned long current_millis;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);     // use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
  attachInterrupt(0, count_pulses, RISING); // trigger interrupt routine on the RISING edge of the signal

  pinMode(RELAY_PIN, OUTPUT);               // digital output pin for relay
  pinMode(YAC_ESTOP_PIN, INPUT);		    // digital input pin for estop signal from DAQ

}

void loop() {

  if (digitalRead(YAC_ESTOP_PIN) == LOW) {
    digitalWrite(RELAY_PIN, HIGH);      // if out of e-stop, close relay
  }
  else if (digitalRead(YAC_ESTOP_PIN) == HIGH) {
    digitalWrite(RELAY_PIN, LOW);      // open relay & cut power to motor
  }
  
  current_millis = millis();

  input_load = analogRead(LOAD_CELL_PIN);
  duty_cycle = map(input_load, 0, 1023, 0, 255);    // map the input voltage (from 0 to 1023) to the duty cycle output (from 0 to 255)
  analogWrite(OUTPUT_PIN, duty_cycle);                // write the load cell dependent duty cycle to the analog output pin
    
  if (current_millis - start_millis >= 1000)
  {
    Serial.println(rpm);                              // print pulse counter every second
    Serial1.write(rpm / 256);
    Serial1.write(rpm % 256);
    Serial1.write(input_load / 256);
    Serial1.write(input_load % 256);
    rpm = 0;                                         // reset the pulse counter to 0
    start_millis = current_millis;
  }

}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
  // increase pulse counter by one
  rpm++;
}
