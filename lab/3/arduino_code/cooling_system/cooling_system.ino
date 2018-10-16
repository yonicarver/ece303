// ECE 303, Lab 3
// Yonatan Carver & Farhan Muhammad
// 10.15.2018

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

#define FAN_PIN 11

volatile int pulse_counter = 0;	// initialize pulse counter
String incoming_payload = "";		// initialize incoming string from Matlab

int duty_cycle;
int input_signal;

unsigned long start_millis;
unsigned long current_millis;

String incoming = "";

void setup() {
	Serial.begin(9600);

	pinMode(INTERRUPT_PIN, INPUT_PULLUP);		// use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
	attachInterrupt(0, count_pulses, RISING);	// trigger interrupt routine on the RISING edge of the signal

	pinMode(FAN_PIN, OUTPUT);
	digitalWrite(FAN_PIN, LOW);
}

void loop() {
	//if (Serial.available() > 0)
	//{
	//	incoming_payload = Serial.readStringUntil('\n');
	//}
	
//	Serial.setTimeout(15);				// allow Serial.readString to read in data faster
//	// if Matlab sends a 'reset' string, reset the pulse counter to 0
//	if (Serial.readString() == 'reset')
//	{
//		pulse_counter = 0;
//	}
	
	

	current_millis = millis();
	if (current_millis - start_millis >= 1000)
	{
		Serial.println(pulse_counter);
		pulse_counter = 0;
		start_millis = current_millis;
	}


	// ------------------------------
	input_signal = analogRead(INPUT_PIN);   // read the input pin voltage from the AWG (constant DC V)
	//Serial.print(input_signal);   		// serially display the input signal

	//Serial.print("\t");				// formatting for output text (prints a tab)
	duty_cycle = map(input_signal, 0, 1023, 0, 255);		// map the input voltage (from 0 to 1023) to the duty cycle output (from 0 to 255)

	analogWrite(OUTPUT_PIN, duty_cycle);	// write the AWG dependent duty cycle to the analog output pin
	//Serial.println(duty_cycle);


		incoming = Serial.readStringUntil('\n');
		if (incoming == "fan_on")
		{
			digitalWrite(FAN_PIN, HIGH);
			Serial.println("fan on");
		}
		else if (incoming == "fan_off")
		{
			digitalWrite(FAN_PIN, LOW);
			Serial.println("fan off");
	
		}


	
}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
	// increase pulse counter by one
	pulse_counter++;
}
