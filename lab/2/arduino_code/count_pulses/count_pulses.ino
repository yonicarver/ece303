// ECE 303, lab 2
// Yonatan Carver & Farhan Muhammad

/*
PURPOSE:
	Count the number of pulses received from an optocoupler and rotary encoder
	Return the counts every second

*/

#define INTERRUPT_PIN 2		// digital pin 2

volatile int pulse_counter = 0;
String incoming_payload = "";



void setup() {
	Serial.begin(9600);

	pinMode(INTERRUPT_PIN, INPUT_PULLUP);
	attachInterrupt(0, count_pulses, RISING);

}

void loop() {
	if (Serial.available() > 0)
	{
		//incoming_payload = Serial.read();
		incoming_payload = Serial.readStringUntil('\n');
		//incoming_payload = "";
	}
	Serial.setTimeout(15);
	if (Serial.readString() == 'reset')
	{
		pulse_counter = 0;
	}
	
	//Serial.print(incoming_payload.toInt());
	//Serial.print("\t");
	//if (second_counter % 1000 == 0) {
	//	pulse_counter = 0;
	//}

	
	Serial.println(pulse_counter);

}

void count_pulses() {
//	static unsigned long last_interrupt_time = 0;	// debouncer
//	unsigned long interrupt_time = millis();		// debouncer
//	if (interrupt_time - last_interrupt_time > 5)
//	{
//		pulse_counter++;    // increase the counter by 1 & return new value
//	}
//	last_interrupt_time = interrupt_time;
	pulse_counter++;
}
