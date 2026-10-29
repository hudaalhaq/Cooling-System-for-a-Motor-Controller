const int ledPin = 5;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  digitalWrite(ledPin, HIGH);
  Serial.print("HIGH");
  delay(7000);
  digitalWrite(ledPin, LOW);
  Serial.print("LOW");
  delay(40000);
}
