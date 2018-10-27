//Sender

void setup() {
  Serial.begin(9600);
}

void loop() { 
  int value=180; //this would be much more exciting if it was a sensor value

  Serial.write(value / 256);
      
  Serial.write(value % 256);
  delay(50); //allows all serial sent to be received together
}


//Receiver

void setup() {
  Serial.begin(9600);
}

void loop() {
  int val;

      
  while (!Serial.available()){}
  byte b1 = Serial.read();
  while (!Serial.available()){}
  byte b2 = Serial.read();

  val = b2  + b1 *256 ; 
  Serial.println(val);

}
