const int ledPin = 5;
unsigned long startTime;

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  startTime = millis();  // Catat waktu saat program mulai
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  if (elapsedTime < 600000) { 
    digitalWrite(ledPin, HIGH);
    Serial.println("HIGH");
    delay(10000);
    digitalWrite(ledPin, LOW);
    Serial.println("LOW");
    delay(30000);
  } 
  else {
    digitalWrite(ledPin, HIGH);
    Serial.println("HIGH");
    delay(3000);
    digitalWrite(ledPin, LOW);
    Serial.println("LOW");
    delay(30000);
  }
}
