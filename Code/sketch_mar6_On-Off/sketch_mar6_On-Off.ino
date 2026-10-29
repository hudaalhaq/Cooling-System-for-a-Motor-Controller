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

DeviceAddress sensor1 = { 0x28, 0x5C, 0xF9, 0x6A, 0x00, 0x00, 0x00, 0x93 };
DeviceAddress sensor2 = { 0x28, 0xDB, 0x57, 0x2C, 0x00, 0x00, 0x00, 0x84 };

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SSD1306_I2C_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup(void){
  Serial.begin(9600);
  sensors.begin();
  
  pinMode(Pump, OUTPUT);
  digitalWrite(Pump, LOW);
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, HIGH);

  while (!Serial) {
      delay(1);
  }
    
  Serial.println("INA219 connect");
  
  // Initialize the INA219.
  // By default the initialization will use the largest range (32V, 2A).  However
  // you can call a setCalibration function to change this range (see comments).
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
	  display.clearDisplay();
	  display.setTextSize(1);
	  display.setTextColor(SSD1306_WHITE);
	  display.setCursor(0, 0);
	  display.println("INA219 not connect");
	  display.display();
	  delay(1000);
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

void loop(void){ 
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

  Serial.print("Temperature sensor 1  : ");
  Serial.print(sensors.getTempC(sensor1)); 
  Serial.println("ºC");
  Serial.print("Temperature sensor 2  : ");
  Serial.print(sensors.getTempC(sensor2)); 
  Serial.println("ºC");
  //Serial.print("Bus Voltage:   "); 
  //Serial.print(busvoltage); Serial.println(" V");
  //Serial.print("Shunt Voltage: "); 
  //Serial.print(shuntvoltage); Serial.println(" mV");
  Serial.print("Load Voltage          : "); 
  Serial.print(loadvoltage); Serial.println(" V");
  Serial.print("Current               : "); 
  Serial.print(current_mA); Serial.println(" mA");
  Serial.print("                        "); 
  Serial.print(current_mA/1000); Serial.println(" A");
  Serial.print("Power                 : "); 
  Serial.print(power_mW); Serial.println(" mW");
  Serial.print("                        "); 
  Serial.print(power_mW/1000); Serial.println(" W");
  Serial.println("");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Power: ");
  display.print(power_mW/1000);
  display.println(" W");
  display.println(" ");
  if (current_mA <= 0){
    display.print("Arus: 0.00 A");
  }
  else{
    display.print("Arus: ");
    display.print(current_mA/1000);
    display.println(" A");
  }
  display.println(" ");
  display.print("Suhu_1: ");
  display.print(sensors.getTempC(sensor1));
  display.println(" C");
  display.println(" ");
  display.print("Suhu_2: ");
  display.print(sensors.getTempC(sensor2));
  display.println(" C");
  display.setTextSize(1);
  display.display();
  
  if (sensors.getTempC(sensor2) >= 50) {
    digitalWrite(Pump, HIGH);
  }
  else if (sensors.getTempC(sensor2) < 40) {
    digitalWrite(Pump, LOW);
  }
  delay(1000);
}
