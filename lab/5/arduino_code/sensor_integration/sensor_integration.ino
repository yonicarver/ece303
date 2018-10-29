#include "DHT.h"

// Input Pins
#define BUZZER_PIN         7
#define DHT_PIN            2
#define WATER_SENSOR_PIN   A0

#define DHT_TYPE           DHT11

// Output Pins
#define COOLANT_LED_PIN    52
#define TEMP_LED_PIN       53
#define NORMAL_LED_PIN     51
#define FAN_PIN            50

// Thresholds
#define COOLANT_THRESH_VAL 50
#define TEMP_THRESH_VAL    30
#define FAN_THRESH_VAL     100

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.begin();
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(COOLANT_LED_PIN, OUTPUT);
  pinMode(TEMP_LED_PIN, OUTPUT);
  pinMode(NORMAL_LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  
  float waterLevel = analogRead (WATER_SENSOR_PIN)-250.0;
  float temp = dht.readTemperature();
  Serial.write("Temperature: ");
  Serial.println(temp);
  Serial.write("Water Level: ");
  Serial.println(waterLevel);
  
  if (waterLevel <= COOLANT_THRESH_VAL && temp >= TEMP_THRESH_VAL) {
    tone(BUZZER_PIN, 500);
    digitalWrite(NORMAL_LED_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, HIGH);
    digitalWrite(COOLANT_LED_PIN, HIGH);
  }
  else if (waterLevel <= COOLANT_THRESH_VAL) {
    tone(BUZZER_PIN, 500);
    digitalWrite(NORMAL_LED_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, LOW);
    digitalWrite(COOLANT_LED_PIN, HIGH);
  }
  else if (temp >= TEMP_THRESH_VAL) {
    tone(BUZZER_PIN, 500);
    digitalWrite(NORMAL_LED_PIN, LOW);
    digitalWrite(TEMP_LED_PIN, HIGH);
    digitalWrite(COOLANT_LED_PIN, LOW);
  }
  else {
    digitalWrite(NORMAL_LED_PIN, HIGH);
    digitalWrite(TEMP_LED_PIN, LOW);
    digitalWrite(COOLANT_LED_PIN, LOW);
    noTone(BUZZER_PIN);
  }

//  if (rpm >= FAN_THRESH_VAL) {
//    digitalWrite(FAN_PIN, HIGH);
//  }
//  else {
//    digitalWrite(FAN_PIN, LOW);
//  }
  
  delay(500);

}

void send_string_to_yac() {
	
}
