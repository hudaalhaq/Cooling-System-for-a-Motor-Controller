#include <Wire.h>
#include <Adafruit_INA219.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2

const int Pump = 5;
const int Fan = 4;

Adafruit_INA219 ina219;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Alamat sensor DS18B20
DeviceAddress sensor1 = { 0x28, 0x5C, 0xF9, 0x6A, 0x00, 0x00, 0x00, 0x93 };
DeviceAddress sensor2 = { 0x28, 0xDB, 0x57, 0x2C, 0x00, 0x00, 0x00, 0x84 };

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SSD1306_I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(void) {
  Serial.begin(9600);
  sensors.begin();

  pinMode(Pump, OUTPUT);
  digitalWrite(Pump, LOW);
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, LOW);
  
  while (!Serial) {
    delay(1);
  }

  if (!ina219.begin()) {
    Serial.println("INA219 tidak ditemukan!");
    tampilkanError("INA219 tidak ditemukan!");
    while (1) delay(10);
  }

  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println("OLED gagal!");
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Inisialisasi OK");
  display.display();
  delay(1000);
}

void loop(void) {
  sensors.requestTemperatures();
  float suhu1 = sensors.getTempC(sensor1);
  float suhu2 = sensors.getTempC(sensor2);

  float shuntvoltage = ina219.getShuntVoltage_mV();
  float busvoltage = ina219.getBusVoltage_V();
  float current_mA = ina219.getCurrent_mA();
  float loadvoltage = busvoltage + (shuntvoltage / 1000.0);
  float power_mW = ina219.getPower_mW();

  float arus_A = current_mA / 1000.0;
  float daya_W = power_mW / 1000.0;

  // Kirim ke Serial
  Serial.print(arus_A, 3); Serial.print(",");
  Serial.print(suhu1, 2); Serial.print(",");
  Serial.println(suhu2, 2);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Power: "); display.print(daya_W, 2); display.println(" W");
  display.println(" ");
  display.print("Arus:  "); display.print(arus_A, 3); display.println(" A");
  display.println(" ");
  display.print("Suhu1: "); display.print(suhu1, 2); display.println(" C");
  display.println(" ");
  display.print("Suhu2: "); display.print(suhu2, 2); display.println(" C");
  display.display();

  if (sensors.getTempC(sensor2) >= 50) {
    digitalWrite(Pump, HIGH);
    digitalWrite(Fan, HIGH);
  }
  else if (sensors.getTempC(sensor2) < 40) {
    digitalWrite(Pump, LOW);
    digitalWrite(Fan, LOW);
  }
  
  delay(1000);
}

void tampilkanError(String pesan) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(pesan);
  display.display();
}
