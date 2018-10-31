#include "DHT.h"
//#include "SoftwareSerial.h"

// Input Pins
#define BUZZER_PIN         7
#define DHT_PIN            2
#define WATER_SENSOR_PIN   A0
#define LOAD_CELL_PIN      A1
#define RPM_PIN            A2

#define DHT_TYPE           DHT11
DHT dht(DHT_PIN, DHT_TYPE);
//SoftwareSerial arduinoSerial(10, 11);

// Output Pins
#define COOLANT_LED_PIN    52
#define TEMP_LED_PIN       53
#define NORMAL_LED_PIN     51
#define FAN_PIN            50
#define YAC_ESTOP_PIN      49

// Thresholds
#define COOLANT_THRESH_VAL 50
#define TEMP_THRESH_VAL    30
#define FAN_THRESH_VAL     100

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


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  arduinoSerial.begin(9600);
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(COOLANT_LED_PIN, OUTPUT);
  pinMode(TEMP_LED_PIN, OUTPUT);
  pinMode(NORMAL_LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  pinMode(YAC_ESTOP_PIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:

  int yacEstop = 0;

  // ANALOG VALUE FOR LOAD CELL
  float load = analogRead (LOAD_CELL_PIN);
  
  float waterLevel = analogRead (WATER_SENSOR_PIN)-250.0;
  float temp = dht.readTemperature();
  Serial.write("Temperature: ");
  Serial.println(temp);
  Serial.write("Water Level: ");
  Serial.println(waterLevel);
  
  if (waterLevel <= COOLANT_THRESH_VAL && temp >= TEMP_THRESH_VAL) {
    //tone(BUZZER_PIN, 500);
    digitalWrite(NORMAL_LED_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, HIGH);
    digitalWrite(COOLANT_LED_PIN, HIGH);

    String estop_status = "estop";
    yacEstop = 1;
    String main_relay_status = "estop";
    String temp_status = "high";
    String coolant_status = "low";
    String led_normal_op_status = "off";
    String led_high_temp_status = "on";
    String led_low_coolant_status = "on";
    make_string()
  }
  else if (waterLevel <= COOLANT_THRESH_VAL) {
    //tone(BUZZER_PIN, 500);
    digitalWrite(NORMAL_LED_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, LOW);
    digitalWrite(COOLANT_LED_PIN, HIGH);
    
    String estop_status = "estop";
    yacEstop = 1;
    String main_relay_status = "estop";
    String temp_status = "ok";
    String coolant_status = "low";
    String led_normal_op_status = "off";
    String led_high_temp_status = "off";
    String led_low_coolant_status = "on";
    make_string()
  }
  else if (temp >= TEMP_THRESH_VAL) {
    //tone(BUZZER_PIN, 500);
    digitalWrite(NORMAL_LED_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, HIGH);
    digitalWrite(COOLANT_LED_PIN, LOW);

    String estop_status = "estop";
    yacEstop = 1;
    String main_relay_status = "estop";
    String temp_status = "high";
    String coolant_status = "ok";
    String led_normal_op_status = "off";
    String led_high_temp_status = "on";
    String led_low_coolant_status = "off";
    make_string()
  }
  else {
    digitalWrite(NORMAL_LED_PIN, HIGH);
    digitalWrite(TEMP_LED_PIN, LOW);
    digitalWrite(COOLANT_LED_PIN, LOW);
    noTone(BUZZER_PIN);

    String estop_status = "ok";
    yacEstop = 0;
    String main_relay_status = "ok";
    String temp_status = "ok";
    String coolant_status = "ok";
    String led_normal_op_status = "on";
    String led_high_temp_status = "off";
    String led_low_coolant_status = "off";
    make_string()
  }

//  if (arduinoSerial.available())
//    byte mainControllerRPM = arduinoSerial.read();
//    int rpm = mainControllerRPM * 256;
  int rpm = analogRead(RPM_PIN);
  
  if (rpm >= FAN_THRESH_VAL) {
    digitalWrite(FAN_PIN, HIGH);
  }
  else {
    digitalWrite(FAN_PIN, LOW);
  }
  
  delay(100);

}

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

  String packet_to_yac = main_relay_status;

  if (Serial.available())
    Serial.write(packet_to_matlab);

//  if (arduinoSerial.available())
//    arduinoSerial.write(packet_to_yac);
  digitalWrite(YAK_ESTOP_PIN, yacEstop);
}
