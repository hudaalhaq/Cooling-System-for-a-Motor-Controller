#include <Wire.h>
#include <Adafruit_INA219.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

const int oneWireBus = 2;

Adafruit_INA219 ina219;
OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SSD1306_I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(void) {
  Serial.begin(9600);
  sensors.begin();

  while (!Serial) {
      delay(1);
  }
    
  Serial.println("INA219 connect");
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) { delay(10); }
  }
  // To use a slightly lower 32V, 1A range (higher precision on amps):
  //ina219.setCalibration_32V_1A();
  // Or to use a lower 16V, 400mA range (higher precision on volts and amps):
  //ina219.setCalibration_16V_400mA();

  Serial.println("Measuring voltage and current with INA219 ...");
  
  if (!display.begin(SSD1306_I2C_ADDRESS, 0x3C)) {
    Serial.println("Gagal menginisialisasi OLED!");
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Inisialisasi...");
  display.display();
  delay(1000);
}

void loop(void) {
  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);
  
  Serial.print("Temperature: ");
  Serial.print(temperatureC);
  Serial.println("ºC");
  //Serial.print("Bus Voltage:   "); 
  //Serial.print(busvoltage); Serial.println(" V");
  //Serial.print("Shunt Voltage: "); 
  //Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage:  "); 
  Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current:       "); 
  Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Power:         "); 
  Serial.print(power_mW); Serial.println(" mW");
  Serial.println("");
  
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Suhu: ");
  display.print(temperatureC);
  display.println(" ºC");
  display.println(" ");
  display.print("Arus: ");
  display.print(current_mA);
  display.println(" A");
  display.println(" ");
  display.print("Power: ");
  display.print(power_mW);
  display.println(" mW");  
  display.setTextSize(1);
  display.display();
  
  delay(1000);
}