#define SENSOR_PIN 20
#define LED_PIN 13

volatile int vibration = 0;
int vibration_count = 0;

unsigned long start_millis = 0;
unsigned long current_millis;

void setup()
{
  Serial.begin(9600);

  pinMode(SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), count_vibrations, RISING);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop()
{
  current_millis = millis();

  if (current_millis - start_millis >= 1000)
  {
    Serial.println(vibration);                              // print pulse counter every second
    if (vibration >= 650) {
      vibration_count++;
    }
    else {
      vibration_count = 0;
    }
    vibration = 0;                                         // reset the pulse counter to 0
    start_millis = current_millis;
  }

  if (vibration_count > 4) {
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }
}

void count_vibrations() {
  // increase pulse counter by one
  vibration++;
}
