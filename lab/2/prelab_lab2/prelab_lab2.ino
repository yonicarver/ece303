// ECE 303 - Pre-lab, Lab 2

//#include <PWM.h>

int input_pin = 5;
int output_pin = 13;

int frequency = 980;
int duty_cycle;
int input_signal;

void setup() {
  Serial.begin(9600);
  //pinMode(output_pin, OUTPUT);
}

void loop() {
  input_signal = analogRead(input_pin);   // read the input pin (DC V)
  Serial.print(input_signal);   // serially display the input signal

  Serial.print("\t");
  duty_cycle = map(input_signal, 0, 1023, 0, 255);

  analogWrite(output_pin, duty_cycle);
  Serial.println(duty_cycle);

}
