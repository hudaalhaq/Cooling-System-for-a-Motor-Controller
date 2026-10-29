const int ledPin = 5;  // Pin PWM untuk LED
const int pwmValues[] = {26, 51, 77, 102, 128, 153, 179, 204, 230, 255};  // 10% - 100%
const int delayTime = 5000;  // Waktu tunggu antar level (ms)

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);  // Inisialisasi Serial Monitor
}

void loop() {
  for (int i = 0; i < 10; i++) {
    analogWrite(ledPin, pwmValues[i]);
    Serial.print("PWM Value: ");
    Serial.println(pwmValues[i]);
    delay(delayTime);
  }
}
