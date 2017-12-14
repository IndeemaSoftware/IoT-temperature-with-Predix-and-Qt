float temp;

void setup() {
  Serial.begin(57600);
};

void loop () {
  temp = analogRead(0)*5/1024.0;
  temp = temp - 0.5;
  temp = temp / 0.01;
  Serial.println(temp);

    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);  
//  delay(500);
};
