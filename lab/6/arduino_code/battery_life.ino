void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  float battery = analogRead(A0)*100.0/1023.0;
  Serial.println(battery);
  delay(500);
}
