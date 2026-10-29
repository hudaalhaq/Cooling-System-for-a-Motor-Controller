const int ledPin = 5;
unsigned long startTime;
bool triggeredAt13Minutes = false;  // Flag agar hanya sekali eksekusi

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);
  startTime = millis();  // Catat waktu saat program mulai
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - startTime;

  // Jalankan sekali pada menit ke-13 (780.000 ms)
  if (!triggeredAt13Minutes && elapsedTime >= 780000) {
    digitalWrite(ledPin, HIGH);
    Serial.println("Manual HIGH at minute 13");
    triggeredAt13Minutes = true; // Supaya tidak diulang lagi
  }

  if (elapsedTime < 600000) { 
    digitalWrite(ledPin, HIGH);
    Serial.println("HIGH");
    delay(8000);
    digitalWrite(ledPin, LOW);
    Serial.println("LOW");
    delay(40000);
  } else {
    digitalWrite(ledPin, HIGH);
    Serial.println("HIGH");
    delay(4000);
    digitalWrite(ledPin, LOW);
    Serial.println("LOW");
    delay(20000);
  }
}
