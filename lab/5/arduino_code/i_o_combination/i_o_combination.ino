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

#define WHITE_LED 22	// normal operation
#define RED_LED 23		// high temperature
#define BLUE_LED 24		// low coolant level

int pulse_counter = 0;	// initialize pulse counter
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

	pinMode(RELAY_PIN, OUTPUT);				// digital output pin for relay
	pinMode(PIEZO_PIN, OUTPUT);				// digital output pin for piezo buzzer

	// LEDs
	pinMode(WHITE_LED, OUTPUT);				// normal operation LED
	pinMode(RED_LED, OUTPUT);				// high temperature LED
	pinMode(BLUE_LED, OUTPUT);				// low coolant level LED
	
}

void loop() {

	
	digitalWrite(RELAY_PIN, HIGH);			// if out of e-stop, close relay

	current_millis = millis();
	if (current_millis - start_millis >= 1000)
	{
		make_string();
		//String stringThree = stringOne + pulse_counter;
		//Serial.println(stringThree);		// print pulse counter every second
		//Serial.write(pulse_counter);
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

void estop() {
	// estop interrupt routine
	digitalWrite(RELAY_PIN, LOW);			// open relay & cut power to motor
	digitalWrite(WHITE_LED, LOW);			// turn off normal operation LED
}



/*
{
estop_status: estop -or- ok,
main_relay: estop -or- ok,
load_cell: <analog_val>,
optical_counter: <analog_val>,
temp: ok -or- high,
coolant: ok -or- low,
led_normal_op: off -or- on,
led_high_temp: off -or- on,
led_low_coolant: off -or- on
}
*/

String estop_status = "estop";
String main_relay_status = "estop";
String temp_status = "ok";
String coolant_status = "ok";
String led_normal_op_status = "off";
String led_high_temp_status = "off";
String led_low_coolant_status = "off";


String str_header = "{";
String str_estop_status = "estop_status: ";
String str_main_relay = ", main_relay: ";
String str_load_cell = ", load_cell: ";
String str_optical_counter = ", optical_counter: ";
String str_temp = ", temp: ";
String str_coolant = ", coolant: ";
String str_led_normal_op = ", led_normal_op: ";
String str_led_high_temp = ", led_high_temp: ";
String str_led_low_coolant = ", led_low_coolant: ";
String str_footer = "}";


void make_string(){
	String packet_to_matlab = str_header +
		str_estop_status + estop_status +
		str_main_relay + main_relay_status +
		str_load_cell + 
		str_optical_counter + pulse_counter +
		str_temp + temp_status +
		str_coolant + coolant_status +
		str_led_normal_op + led_normal_op_status +
		str_led_high_temp + led_high_temp_status +
		str_led_low_coolant + led_low_coolant_status +
		str_footer
		
		;

	String packet_to_yac = main_relay_status;

	Serial.println(packet_to_matlab);
	Serial.println(packet_to_yac);
}
