// ECE 303, lab 2

#define INTERRUPT_PIN 2

volatile int pulse_counter = 1;


void setup() {
  Serial.begin(9600);
  
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(0, count_pulses, FALLING);

}

void loop() {
  Serial.println(pulse_counter);
  delay(1000);
  //pulse_counter = 0;
}

void count_pulses() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > 100)
  {
    pulse_counter++;    // increase the counter by 1
  }
  last_interrupt_time = interrupt_time;
}
