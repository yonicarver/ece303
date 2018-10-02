// ECE 303, Lab 2
// Yonatan Carver & Farhan Muhammad
// 10.1.2018

/*
YONI CARVER
718.704.8555
yonicarver@gmail.com
*/

/*
PURPOSE OF THE PROGRAM:

	Output a 980Hz signal with a duty cycle that is linearly dependent
	on constant input DC voltage from an Arbitrary Waveform Generator (AWG).
	
	This output PWM signal controls the H-Bridge for a DC motor (i.e.
	the speed of the motor).

	Count the number of pulses received from the optocoupler and rotary
	encoder attached to the DC motor.
	
	Return the counts every second (via the serial monitor).

*/

#define INTERRUPT_PIN 2	// digital pin 2, reads in pulses from rotary encoder
#define INPUT_PIN 5		// analog pin 5, input pin for AWG constant DC voltage
#define OUTPUT_PIN 12	// digital pin 12, output pin for PWM signal (linearly dependent on AWG voltage)

volatile int pulse_counter = 0;	// initialize pulse counter
String incoming_payload = "";		// initialize incoming string from Matlab

int duty_cycle;
int input_signal;

void setup() {
	Serial.begin(9600);

	pinMode(INTERRUPT_PIN, INPUT_PULLUP);		// use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
	attachInterrupt(0, count_pulses, RISING);	// trigger interrupt routine on the RISING edge of the signal
}

void loop() {
	if (Serial.available() > 0)
	{
		incoming_payload = Serial.readStringUntil('\n');
	}
	
	Serial.setTimeout(15);				// allow Serial.readString to read in data faster
	// if Matlab sends a 'reset' string, reset the pulse counter to 0
	if (Serial.readString() == 'reset')
	{
		pulse_counter = 0;
	}
	
	Serial.println(pulse_counter);

	// ------------------------------
	input_signal = analogRead(INPUT_PIN);   // read the input pin voltage from the AWG (constant DC V)
	//Serial.print(input_signal);   		// serially display the input signal

	//Serial.print("\t");				// formatting for output text (prints a tab)
	duty_cycle = map(input_signal, 0, 1023, 0, 255);		// map the input voltage (from 0 to 1023) to the duty cycle output (from 0 to 255)

	analogWrite(OUTPUT_PIN, duty_cycle);	// write the AWG dependent duty cycle to the analog output pin
	//Serial.println(duty_cycle);

}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
	// increase pulse counter by one
	pulse_counter++;
}
