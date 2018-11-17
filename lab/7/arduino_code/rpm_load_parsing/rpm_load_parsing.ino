
#define START_STOP_PIN	  45

String readString;		// main captured string

// Strings for data from MATLAB to DAQ to main_controller
String start_stop;
String rpm_matlab;
String max_load_matlab;

// section indicators for start_stop, max_rpm, max_load
int ind1;
int ind2;
int ind3;

void setup() {
  Serial.begin(9600);
  pinMode(START_STOP_PIN, OUTPUT);
}

void loop() {
  if (Serial.available() == 12)
  {
//  	format: "0,0000,1111*" -- 12 bytes

  char c = Serial.read();
  if (c == '*') {
  	// Parse over incoming string
  	ind1 = readString.indexOf(',');				// location of first ","
  	start_stop = readString.substring(0, ind1);		// first data string
  	ind2 = readString.indexOf(',', ind1+1);			// location of second ","
  	rpm_matlab = readString.substring(ind1+1, ind2);	// second data string
  	ind3 = readString.indexOf(',', ind2+1);			// location of third ","
  	max_load_matlab = readString.substring(ind2+1);	// third data string, after last ","

	Serial.print("Start_stop flag: ");
	Serial.println(start_stop);
	digitalWrite(START_STOP_PIN, start_stop.toInt());
  	Serial.print("RPM from rpm_matlab: ");
  	Serial.println(rpm_matlab.toInt());
  	Serial.print("Max Load from MATLAB: ");
  	Serial.println(max_load_matlab.toInt());
  	Serial.println();
  	Serial.println();

  	rpm_matlab = "";		// clear variables for new input
  	max_load_matlab = "";
  	readString = "";
  }
  else
  	readString += c;		// makes the string readString
//  	Serial.println(c);

  
  }

}
