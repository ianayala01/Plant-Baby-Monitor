//https://deepbluembedded.com/esp32-lcd-display-16x2-without-i2c-arduino/

#include <LiquidCrystal.h>
 
// Create An LCD Object. Signals: [ RS, EN, D4, D5, D6, D7 ]
LiquidCrystal lcd(23, 22, 21, 19, 18, 5);

#define thermPin 32
#define SERIES_RESISTOR 10000.0
#define NOMINAL_RESISTANCE 10000.0
#define NOMINAL_TEMPERATURE 25.0
#define B_COEFFICIENT 3950.0

float highTemp;
float lowTemp;
float aveTemp;

#define lightPin 33

int lightVal = 0;
int lightSecs = 0;
int mins = 0;
int hrs = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  lcd.begin(16, 2);
  delay(50);

  // Clears The LCD Display
  lcd.clear();
 
  lcd.print("Ave Temp:");
  lcd.setCursor(0, 1);
  lcd.print("Light:");

  lowTemp = getTemp();
  highTemp = getTemp();
}
 
void loop() {

  float tempTemp = getTemp();

  if (tempTemp < lowTemp) lowTemp = tempTemp;
  if (tempTemp > highTemp) highTemp = tempTemp;

  aveTemp = (lowTemp + highTemp) / 2;

  lightVal = analogRead(lightPin);
  Serial.println(lightVal);

  if (lightVal > 1000){
    lightSecs++;
  }

  Serial.print("low Temp: ");
  Serial.println(lowTemp);

  Serial.print("high temp: ");
  Serial.println(highTemp);

  Serial.print("ave temp: ");
  Serial.println(aveTemp);

  lcd.setCursor(10, 0);
  lcd.print(aveTemp);
  lcd.print("");

  printTime(lightSecs);

  delay(1000);
}

void printTime(int secs){
  if (secs >= 60) {
    mins = secs/60;
    secs = secs % 60;
    if (mins >= 60){
      hrs = mins/60;
      mins = mins % 60;
    }
  }

  lcd.setCursor(7,1);
  if (hrs < 10) lcd.print("0");
  lcd.print(hrs);
  lcd.print(":");
  if (mins < 10) lcd.print("0");
  lcd.print(mins);
  lcd.print(":");
  if (secs < 10) lcd.print("0");
  lcd.print(secs);

}

float getTemp(){
  int adcValue = analogRead(thermPin);

  float voltage = adcValue * (3.3 / 4095.0);
  float resistance = SERIES_RESISTOR * (3.3 / voltage - 1);

  float temperature;
  temperature = resistance / NOMINAL_RESISTANCE;
  temperature = log(temperature);
  temperature /= B_COEFFICIENT;
  temperature += 1.0 / (NOMINAL_TEMPERATURE + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15;

  float freedom = ((temperature * 2) + 30);

  //return temperature for Celscius
  //return freedom for freedom units
  return freedom;
}