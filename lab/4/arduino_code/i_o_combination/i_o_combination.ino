// ECE 303, Lab 4
// Yonatan Carver & Farhan Muhammad
// 10.22.2018

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

	Receive a string (from MATLAB) to turn a fan on a digital pin on or off.

	Attach sensors (i.e. relay, piezo, etc.) to the Arduino.

*/

#define INTERRUPT_PIN 2	// digital pin 2, reads in pulses from rotary encoder
#define INPUT_PIN 5		// analog pin 5, input pin for AWG constant DC voltage
#define OUTPUT_PIN 12	// digital pin 12, output pin for PWM signal (linearly dependent on AWG voltage)

#define FAN_PIN 11

#define RELAY_PIN 50
#define PIEZO_PIN 52

volatile int pulse_counter = 0;	// initialize pulse counter
String incoming_payload = "";		// initialize incoming string from Matlab

int duty_cycle;
int input_signal;

unsigned long start_millis;
unsigned long current_millis;

String incoming = "";

bool fan_flag = false;

void setup() {
	Serial.begin(9600);

	pinMode(INTERRUPT_PIN, INPUT_PULLUP);		// use the internal pullup resistor on the interrupt pin (ensures no floating voltages)
	attachInterrupt(0, count_pulses, RISING);	// trigger interrupt routine on the RISING edge of the signal

	pinMode(FAN_PIN, OUTPUT);				// digital output pin for cooling fan
	digitalWrite(FAN_PIN, HIGH);				// set cooling fan output pin to low by default

	pinMode(RELAY_PIN, OUTPUT);
	pinMode(PIEZO_PIN, OUTPUT);

}

void loop() {

	digitalWrite(RELAY_PIN, HIGH);
//	tone(PIEZO_PIN, 1000, 500);		// pin #, frequency (Hz), duration of tone

	current_millis = millis();
	if (current_millis - start_millis >= 1000)
	{
		Serial.println(pulse_counter);		// print pulse counter every second
		Serial.write(pulse_counter);
		if (pulse_counter > 80) {
			fan_flag = true;
		}
		else {
			fan_flag = false;
		}
		pulse_counter = 0;					// reset the pulse counter to 0
		start_millis = current_millis;
	}

	// ------------------------------
	input_signal = analogRead(INPUT_PIN);   // read the input pin voltage from the AWG (constant DC V)
	//Serial.print(input_signal);   		// serially display the input signal

	//Serial.print("\t");				// formatting for output text (prints a tab)
	duty_cycle = map(input_signal, 0, 1023, 0, 255);		// map the input voltage (from 0 to 1023) to the duty cycle output (from 0 to 255)

	analogWrite(OUTPUT_PIN, duty_cycle);	// write the AWG dependent duty cycle to the analog output pin
	//Serial.println(duty_cycle);

	// read in string from MATLAB
	//incoming = Serial.readStringUntil('\n');
	//if (incoming == "fan_on")
	if (fan_flag == true)
	{
		digitalWrite(FAN_PIN, HIGH);
		//Serial.println("fan on");
	}
	//else if (incoming == "fan_off")
	else if (fan_flag == false)
	{
		digitalWrite(FAN_PIN, LOW);
		//Serial.println("fan off");

	}

}

// INTERRUPT ROUTINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void count_pulses() {
	// increase pulse counter by one
	pulse_counter++;
}





/*
// For sending a string to another arduino (tx to rx, rx to tx, gnd to gnd
char mystr[5] = "Hello"; //String data

void setup() {
  // Begin the Serial at 9600 Baud
  Serial.begin(9600);
}

void loop() {
  Serial.write(mystr,5); //Write the serial data
  delay(1000);
}






*/
