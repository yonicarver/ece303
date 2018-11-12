#include "DHT.h"

// Input Pins
#define DHT_PIN            A2
#define WATER_SENSOR_PIN   A0

#define DHT_TYPE           DHT11
DHT dht(DHT_PIN, DHT_TYPE);

// Output Pins
#define BUZZER_PIN         7
#define COOLANT_LED_PIN    52
#define TEMP_LED_PIN       53
#define NORMAL_LED_PIN     51
#define FAN_PIN            50
#define YAC_ESTOP_PIN      49

// Thresholds
#define COOLANT_THRESH_VAL 50
#define TEMP_THRESH_VAL    30
#define FAN_THRESH_VAL     100

//50
//30
//100

// Matlab String
String str_estop_status = "estop_status: ";
String str_main_relay = ", main_relay: ";
String str_load_cell = ", load_cell: ";
String str_optical_counter = ", optical_counter: ";
String str_temp = ", temp: ";
String str_coolant = ", coolant: ";
String str_led_normal_op = ", led_normal_op: ";
String str_led_high_temp = ", led_high_temp: ";
String str_led_low_coolant = ", led_low_coolant: ";


String estop_status = "test_estop";
String main_relay_status = "test_relay";
String temp_status = "test_temp";
String coolant_status = "test_coolant";
String led_normal_op_status = "test_led_normalop";
String led_high_temp_status = "test_led_hightemp";
String led_low_coolant_status = "test_led_lowcoolant";



// Declare load cell value and rpm as ints
int load;
int rpm;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(COOLANT_LED_PIN, OUTPUT);
  pinMode(TEMP_LED_PIN, OUTPUT);
  pinMode(NORMAL_LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(YAC_ESTOP_PIN, OUTPUT);

}

void loop() {
  make_string();

  while (!Serial1.available()){}
  byte b2 = Serial1.read();
//  rpm = b2 + b1*256;
  rpm = b2; //+ 256;
  if (rpm >= FAN_THRESH_VAL) {
    digitalWrite(FAN_PIN, HIGH);
  }
  else {
    digitalWrite(FAN_PIN, LOW);
  }

  // Read load cell value from main controller
//  while (!Serial1.available()){}
//  byte b3 = Serial1.read();
  while (!Serial1.available()){}
  byte b4 = Serial1.read();
  load = b4; // + b3*256;

  // Read temperature and water levels and set off resepective alarms
  float waterLevel = analogRead (WATER_SENSOR_PIN)-250.0;
  float temp = dht.readTemperature();
//  Serial.println(temp);
//  Serial.println(waterLevel);

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
  else {
    digitalWrite(NORMAL_LED_PIN, HIGH);
    digitalWrite(TEMP_LED_PIN, LOW);
    digitalWrite(COOLANT_LED_PIN, LOW);
    noTone(BUZZER_PIN);

    digitalWrite(YAC_ESTOP_PIN, HIGH);

    estop_status = "ok";
    main_relay_status = "ok";
    temp_status = "ok";
    coolant_status = "ok";
    led_normal_op_status = "on";
    led_high_temp_status = "off";
    led_low_coolant_status = "off";
  }

  delay(1000);

}

// Collect and send data to MATLAB
void make_string(){
  String packet_to_matlab =
    str_estop_status + estop_status +
    str_main_relay + main_relay_status +
    str_load_cell + load +
    str_optical_counter + rpm +
    str_temp + temp_status +
    str_coolant + coolant_status +
    str_led_normal_op + led_normal_op_status +
    str_led_high_temp + led_high_temp_status +
    str_led_low_coolant + led_low_coolant_status
    ;

    char packet_to_mlab[10] = "000111000";

//  if (Serial.available())
    Serial.println(packet_to_matlab);
//	Serial.println(packet_to_mlab);
}
