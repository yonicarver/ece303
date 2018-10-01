// ECE 303 - Pre-lab, Lab 2
// Yonatan Carver & Farhan Muhammad

/*
PURPOSE:
	Output a 980Hz signal with a duty cycle that is linearly dependent
	on constant input DC voltage from an Arbitrary Waveform Generator

*/

#define INPUT_PIN 5;
#define OUTPUT_PIN 13;

int frequency = 980;
int duty_cycle;
int input_signal;

void setup() {
	Serial.begin(9600);
}

void loop() {
	input_signal = analogRead(INPUT_PIN);   // read the input pin (DC V)
	Serial.print(input_signal);   // serially display the input signal

	Serial.print("\t");
	duty_cycle = map(input_signal, 0, 1023, 0, 255);

	analogWrite(OUTPUT_PIN, duty_cycle);
	Serial.println(duty_cycle);

}
