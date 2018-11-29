#include "DHT.h"

// DHT (Temp & Humidity) Input Pins
#define DHT_PIN               A2
#define WATER_SENSOR_PIN      A0
#define BATTERY_VOLTAGE_PIN   A4
#define DHT_TYPE              DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Output Pins
#define BUZZER_PIN         7
#define START_STOP_PIN     45
#define YAC_ESTOP_PIN      49
#define FAN_PIN            50
#define NORMAL_LED_PIN     51
#define COOLANT_LED_PIN    52
#define TEMP_LED_PIN       53

// 7 Segment Pins
#define pinA    22
#define pinB	 23
#define pinC    24
#define pinD    25
#define pinE    26
#define pinF    27
#define pinG    28
#define D1	 29
#define D2	 30
#define D3	 31
#define D4	 32

// Thresholds
#define COOLANT_THRESH_VAL 00      // 50
#define TEMP_THRESH_VAL    30      // 30
int FAN_THRESH_VAL = 100;          // 100 - aka maximum RPM

// MATLAB string: Keys
String str_estop_status = "estop_status: ";
String str_main_relay = ", main_relay: ";
String str_load_cell = ", load_cell: ";
String str_optical_counter = ", optical_counter: ";
String str_temp = ", temp: ";
String str_coolant = ", coolant: ";
String str_led_normal_op = ", led_normal_op: ";
String str_led_high_temp = ", led_high_temp: ";
String str_led_low_coolant = ", led_low_coolant: ";
String str_battery_voltage = ", batt_voltage: ";

// MATLAB string: initialization Values
String estop_status = "test_estop";
String main_relay_status = "test_relay";
String temp_status = "test_temp";
String coolant_status = "test_coolant";
String led_normal_op_status = "test_led_normalop";
String led_high_temp_status = "test_led_hightemp";
String led_low_coolant_status = "test_led_lowcoolant";

int load;      // load cell value
int rpm;       // rpm value
float battery_voltage;

int start_stop_flag;     // flag for main_controller start/stop

// Payload from MATLAB to DAQ to main_controller
String readString;       // main captured string from MATLAB (to be sent to main_controller)

// Strings for data from MATLAB to DAQ to main_controller
String start_stop;
String rpm_matlab;
String max_load_matlab;

// section indicators for start_stop, max_rpm, max_load
int ind1;
int ind2;
int ind3;

// timer acting as delay
unsigned long current_millis;
unsigned long start_millis = 0;

// =============================================================================

void setup() {
     Serial.begin(9600);      // begin serial over USB
     Serial1.begin(9600);     // begin serial1 over 18TX1 & 19RX1
     dht.begin();             // initialize DHT library for temp/humidity sensor

     pinMode(BUZZER_PIN,           OUTPUT);
     pinMode(COOLANT_LED_PIN,      OUTPUT);
     pinMode(TEMP_LED_PIN,         OUTPUT);
     pinMode(NORMAL_LED_PIN,       OUTPUT);
     pinMode(FAN_PIN,              OUTPUT);
     pinMode(YAC_ESTOP_PIN,        OUTPUT);

     pinMode(START_STOP_PIN,       OUTPUT);

     // 7 Segment Outputs
     pinMode(pinA,  OUTPUT);
     pinMode(pinB,  OUTPUT);
     pinMode(pinC,  OUTPUT);
     pinMode(pinD,  OUTPUT);
     pinMode(pinE,  OUTPUT);
     pinMode(pinF,  OUTPUT);
     pinMode(pinG,  OUTPUT);
     pinMode(D1,    OUTPUT);
     pinMode(D2, 	OUTPUT);
     pinMode(D3, 	OUTPUT);
     pinMode(D4, 	OUTPUT);


}

// =============================================================================

void loop() {
     make_string();      // make string to send to MATLAB

     // read rpm value from main_controller
     while (!Serial1.available()){}
     int rpm_bytes = Serial1.read();
     rpm = rpm_bytes;
     display_seven_seg(rpm);     // display rpm on 7 segment display
     if (rpm >= FAN_THRESH_VAL) {
          digitalWrite(FAN_PIN, HIGH);
     }
     else {
          digitalWrite(FAN_PIN, LOW);
     }

     // ------------------------------------------------------------------------
     
     // Read load cell value from main controller
     while (!Serial1.available()){}
     int load_cell_bytes = Serial1.read();
     load = load_cell_bytes;

     // ------------------------------------------------------------------------

     // payload data from MATLAB
     if (Serial.available() > 0) {
          //   format: "0,0000,1111*" -- 12 bytes, must end with "*"
     
          char c = Serial.read();                              // read each character
          if (c == '*') {
               // Parse over incoming string from MATLAB
               ind1 = readString.indexOf(',');                   // location of first ","
               start_stop = readString.substring(0, ind1);       // first data string
               ind2 = readString.indexOf(',', ind1+1);           // location of second ","
               rpm_matlab = readString.substring(ind1+1, ind2);  // second data string
               ind3 = readString.indexOf(',', ind2+1);           // location of third ","
               max_load_matlab = readString.substring(ind2+1);   // third data string, after last ","

               Serial.println();
               Serial.println();
               Serial.print("Start_stop flag: ");
               Serial.println(start_stop);
               //   digitalWrite(START_STOP_PIN, start_stop_flag);
               start_stop_flag = start_stop.toInt();
               Serial.print("RPM from rpm_matlab: ");
               Serial.println(rpm_matlab.toInt());
               FAN_THRESH_VAL = rpm_matlab.toInt();
               Serial.print("Max Load from MATLAB: ");
               Serial.println(max_load_matlab.toInt());
               Serial.println();
               Serial.println(max_load_matlab);

               long max_load_matlab1 = long(max_load_matlab.toInt());
               long rpm_matlab1 = long(rpm_matlab.toInt());
//               long test1 = atol(max_load_matlab);//.toInt();
//               Serial.print("test1: "); Serial.println(test1);
//               long test2 = atol(rpm_matlab);//.toInt();
//               Serial.print("test2: "); Serial.println(test2);
               Serial1.write(max_load_matlab1);//.toInt());//.toInt());// % 256.0);     // write max_load to main_controller
               Serial.println(max_load_matlab1);
               Serial.println(max_load_matlab.toInt() / 512);
               Serial.println(max_load_matlab.toInt() % 512);
     //          Serial.println(max_load_matlab.toInt());    // debugging
               Serial1.write(rpm_matlab1);//.toInt());//.toInt());// % 256.0);     // write max_rpm to main_controller
               Serial.println(rpm_matlab1);
               Serial.println(rpm_matlab.toInt() / 512);
               Serial.println(rpm_matlab.toInt() % 512);
     
               rpm_matlab = "";         // clear variables for new input
               max_load_matlab = "";
               readString = "";
     }
     else
          readString += c;         // makes the string readString
     //   Serial.println(c);       // debugging

     }

     // write the start/stop pin to main_controller
     if (start_stop_flag == 1)
          digitalWrite(START_STOP_PIN, HIGH);
     else if (start_stop_flag == 0)
          digitalWrite(START_STOP_PIN, LOW);

//     if (start_stop_flag == 1 && accelerometer_flag == 0)
//          digitalWrite(START_STOP_PIN, HIGH);
//     else if (start_stop_flag == 0 && accelerometer_flag == 1)
//          digitalWrite(START_STOP_PIN, LOW);
//     else if (start_stop_flag == 0 && accelerometer_flag == 0)
//          digitalWrite(START_STOP_PIN, LOW);
//     else if (start_stop_flag == 1 && accelerometer_flag == 1)
//          digitalWrite(START_STOP_PIN, LOW);

     // ------------------------------------------------------------------------

     // Read temperature and water levels and set off resepective alarms
     float waterLevel = analogRead (WATER_SENSOR_PIN)-250.0;
     float temp = dht.readTemperature();

     // ------------------------------------------------------------------------

     // read battery voltage
     battery_voltage = analogRead(BATTERY_VOLTAGE_PIN);
     battery_voltage = battery_voltage * 100.0 / 1023.0;    // adjust battery voltage to be sent to MATLAB

     // ------------------------------------------------------------------------

     if (waterLevel <= COOLANT_THRESH_VAL && temp >= TEMP_THRESH_VAL) {
          tone(BUZZER_PIN, 500);
          digitalWrite(NORMAL_LED_PIN, LOW);
          digitalWrite(TEMP_LED_PIN, HIGH);
          digitalWrite(COOLANT_LED_PIN, HIGH);
     
          digitalWrite(YAC_ESTOP_PIN, LOW);
     
          estop_status = "estop";
          main_relay_status = "estop";
          temp_status = "high";
          coolant_status = "low";
          led_normal_op_status = "off";
          led_high_temp_status = "on";
          led_low_coolant_status = "on";
     }

     else if (waterLevel <= COOLANT_THRESH_VAL) {
          tone(BUZZER_PIN, 500);
          digitalWrite(NORMAL_LED_PIN, LOW);
          digitalWrite(TEMP_LED_PIN, LOW);
          digitalWrite(COOLANT_LED_PIN, HIGH);
     
          digitalWrite(YAC_ESTOP_PIN, LOW);
     
          estop_status = "estop";
          main_relay_status = "estop";
          temp_status = "ok";
          coolant_status = "low";
          led_normal_op_status = "off";
          led_high_temp_status = "off";
          led_low_coolant_status = "on";
     }

     else if (temp >= TEMP_THRESH_VAL) {
          tone(BUZZER_PIN, 500);
          digitalWrite(NORMAL_LED_PIN, LOW);
          digitalWrite(TEMP_LED_PIN, HIGH);
          digitalWrite(COOLANT_LED_PIN, LOW);
     
          digitalWrite(YAC_ESTOP_PIN, LOW);
     
          estop_status = "estop";
          main_relay_status = "estop";
          temp_status = "high";
          coolant_status = "ok";
          led_normal_op_status = "off";
          led_high_temp_status = "on";
          led_low_coolant_status = "off";
     }

//     else if (accelerometer_flag == 0) {
//          noTone(BUZZER_PIN);
//          digitalWrite(NORMAL_LED_PIN, LOW);
//          digitalWrite(TEMP_LED_PIN, LOW);
//          digitalWrite(COOLANT_LED_PIN, LOW);
//     
//          digitalWrite(YAC_ESTOP_PIN, LOW);
//     
//          estop_status = "estop";
//          main_relay_status = "estop";
//          temp_status = "ok";
//          coolant_status = "ok";
//          led_normal_op_status = "off";
//          led_high_temp_status = "off";
//          led_low_coolant_status = "off";
//     }
     else {
          noTone(BUZZER_PIN);
          digitalWrite(NORMAL_LED_PIN, HIGH);
          digitalWrite(TEMP_LED_PIN, LOW);
          digitalWrite(COOLANT_LED_PIN, LOW);
     
          digitalWrite(YAC_ESTOP_PIN, HIGH);
     
          estop_status = "ok";
          main_relay_status = "ok";
          temp_status = "ok";
          coolant_status = "ok";
          led_normal_op_status = "on";
          led_high_temp_status = "off";
          led_low_coolant_status = "off";
          }


//     delay(250);
     current_millis = millis();
     while (current_millis - start_millis <= 250) {
          display_seven_seg(rpm);     // display rpm on 7 segment display
     }
     start_millis = current_millis;

}

// =============================================================================

// Collect and send data to MATLAB
void make_string() {
     // Assemble string to be sent to MATLAB
     String packet_to_matlab =
         str_estop_status + estop_status +
         str_main_relay + main_relay_status +
         str_load_cell + load +
         str_optical_counter + rpm +
         str_temp + temp_status +
         str_coolant + coolant_status +
         str_led_normal_op + led_normal_op_status +
         str_led_high_temp + led_high_temp_status +
         str_led_low_coolant + led_low_coolant_status +
         str_battery_voltage + battery_voltage
         ;

     //    char packet_to_mlab[10] = "000111000";      // testing

     //  if (Serial.available())
     Serial.flush();
     Serial.println(packet_to_matlab);
     //   Serial.println(packet_to_mlab);
}

void display_seven_seg(int rpm) {
     // rpm

     int seven_seg_volt_thousand = extractDigit(rpm, 4);
     int seven_seg_volt_hundred  = extractDigit(rpm, 3);
     int seven_seg_volt_ten      = extractDigit(rpm, 2);
     int seven_seg_volt_unit     = extractDigit(rpm, 1);

     digitalWrite(D1, LOW);
     digitalWrite(D2, HIGH);
     digitalWrite(D3, HIGH);
     digitalWrite(D4, HIGH);
     //thousand
     displayDigit(seven_seg_volt_thousand);
     delay(4);

     digitalWrite(D1, HIGH);
     digitalWrite(D2, LOW);
     digitalWrite(D3, HIGH);
     digitalWrite(D4, HIGH);
     //hundred
     displayDigit(seven_seg_volt_hundred);
     delay(4);

     digitalWrite(D1, HIGH);
     digitalWrite(D2, HIGH);
     digitalWrite(D3, LOW);
     digitalWrite(D4, HIGH);
     //ten
     displayDigit(seven_seg_volt_ten);
     delay(4);

     digitalWrite(D1, HIGH);
     digitalWrite(D2, HIGH);
     digitalWrite(D3, HIGH);
     digitalWrite(D4, LOW);
     //unit
     displayDigit(seven_seg_volt_unit);
     delay(4);

   }

void displayDigit(int digit) {
     switch (digit) {
       case 0:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, HIGH);
         digitalWrite(pinF, HIGH);
         digitalWrite(pinG, LOW);
         break;
       case 1:
         digitalWrite(pinA, LOW);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, LOW);
         digitalWrite(pinE, LOW);
         digitalWrite(pinF, LOW);
         digitalWrite(pinG, LOW);
         break;
       case 2:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, LOW);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, HIGH);
         digitalWrite(pinF, LOW);
         digitalWrite(pinG, HIGH);
         break;
       case 3:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, LOW);
         digitalWrite(pinF, LOW);
         digitalWrite(pinG, HIGH);
         break;
       case 4:
         digitalWrite(pinA, LOW);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, LOW);
         digitalWrite(pinE, LOW);
         digitalWrite(pinF, HIGH);
         digitalWrite(pinG, HIGH);
         break;
       case 5:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, LOW);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, LOW);
         digitalWrite(pinF, HIGH);
         digitalWrite(pinG, HIGH);
         break;
       case 6:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, LOW);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, HIGH);
         digitalWrite(pinF, HIGH);
         digitalWrite(pinG, HIGH);
         break;
       case 7:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, LOW);
         digitalWrite(pinE, LOW);
         digitalWrite(pinF, LOW);
         digitalWrite(pinG, LOW);
         break;
       case 8:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, HIGH);
         digitalWrite(pinF, HIGH);
         digitalWrite(pinG, HIGH);
         break;
       case 9:
         digitalWrite(pinA, HIGH);
         digitalWrite(pinB, HIGH);
         digitalWrite(pinC, HIGH);
         digitalWrite(pinD, HIGH);
         digitalWrite(pinE, LOW);
         digitalWrite(pinF, HIGH);
         digitalWrite(pinG, HIGH);
         break;
     }
}

int extractDigit(int V, int P) {
     return int(V/(pow(10,P-1))) - int(V/(pow(10,P)))*10;
}
