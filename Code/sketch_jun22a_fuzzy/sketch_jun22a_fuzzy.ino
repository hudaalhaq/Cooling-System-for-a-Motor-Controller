#include <Wire.h>
#include <Adafruit_INA219.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Fuzzy.h>

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

Fuzzy *fuzzy = new Fuzzy();

void setupFuzzy() {
  FuzzyInput *temperature = new FuzzyInput(1);
  FuzzySet *safe = new FuzzySet(0, 0, 30, 40);
  FuzzySet *normal = new FuzzySet(30, 40, 50, 60);
  FuzzySet *panas = new FuzzySet(60, 70, 80, 90);
  FuzzySet *kritis = new FuzzySet(85, 90, 100, 100);

  temperature->addFuzzySet(safe);
  temperature->addFuzzySet(normal);
  temperature->addFuzzySet(panas);
  temperature->addFuzzySet(kritis);
  fuzzy->addFuzzyInput(temperature);

  FuzzyOutput *pwm = new FuzzyOutput(1);
  FuzzySet *low = new FuzzySet(0, 0, 30, 60);
  FuzzySet *mid = new FuzzySet(60, 80, 100, 120);
  FuzzySet *high = new FuzzySet(120, 140, 160, 180);
  FuzzySet *full = new FuzzySet(200, 220, 255, 255);

  pwm->addFuzzySet(low);
  pwm->addFuzzySet(mid);
  pwm->addFuzzySet(high);
  pwm->addFuzzySet(full);
  fuzzy->addFuzzyOutput(pwm);

  FuzzyRuleAntecedent *ifSafe = new FuzzyRuleAntecedent();
  ifSafe->joinSingle(safe);
  FuzzyRuleConsequent *thenLow = new FuzzyRuleConsequent();
  thenLow->addOutput(low);
  fuzzy->addFuzzyRule(new FuzzyRule(1, ifSafe, thenLow));

  FuzzyRuleAntecedent *ifNormal = new FuzzyRuleAntecedent();
  ifNormal->joinSingle(normal);
  FuzzyRuleConsequent *thenMid = new FuzzyRuleConsequent();
  thenMid->addOutput(mid);
  fuzzy->addFuzzyRule(new FuzzyRule(2, ifNormal, thenMid));

  FuzzyRuleAntecedent *ifPanas = new FuzzyRuleAntecedent();
  ifPanas->joinSingle(panas);
  FuzzyRuleConsequent *thenHigh = new FuzzyRuleConsequent();
  thenHigh->addOutput(high);
  fuzzy->addFuzzyRule(new FuzzyRule(3, ifPanas, thenHigh));

  FuzzyRuleAntecedent *ifKritis = new FuzzyRuleAntecedent();
  ifKritis->joinSingle(kritis);
  FuzzyRuleConsequent *thenFull = new FuzzyRuleConsequent();
  thenFull->addOutput(full);
  fuzzy->addFuzzyRule(new FuzzyRule(4, ifKritis, thenFull));
}

void setup(void) {
  Serial.begin(9600);
  sensors.begin();

  pinMode(Pump, OUTPUT);
  digitalWrite(Pump, LOW);
  pinMode(Fan, OUTPUT);
  digitalWrite(Fan, LOW);

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

  setupFuzzy();
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

  fuzzy->setInput(1, suhu2);
  fuzzy->fuzzify();
  float outputPWM = fuzzy->defuzzify(1);

  analogWrite(Pump, (int)outputPWM);

  Serial.print(arus_A, 2); Serial.print(",");
  Serial.print(suhu1, 2); Serial.print(",");
  Serial.print(suhu2, 2); //Serial.print(", PWM: ");
//  Serial.println(outputPWM);
  Serial.println(" ");

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Power: "); display.print(daya_W, 2); display.println(" W");
  display.println(" ");
  display.print("Arus:  "); display.print(arus_A, 2); display.println(" A");
  display.println(" ");
  display.print("Suhu1: "); display.print(suhu1, 2); display.println(" C");
  display.println(" ");
  display.print("Suhu2: "); display.print(suhu2, 2); display.println(" C");
  display.display();

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
