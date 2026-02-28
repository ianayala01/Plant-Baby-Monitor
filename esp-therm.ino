#include <LiquidCrystal.h>
#include <Arduino.h>
#include <WiFi.h>
#include <ESP_Mail_Client.h>
 
// Create An LCD Object. Signals: [ RS, EN, D4, D5, D6, D7 ]
LiquidCrystal lcd(23, 22, 21, 19, 18, 5);

#define buttonPin 27
#define lightPin 33
#define thermPin 32
#define SERIES_RESISTOR 10000.0
#define NOMINAL_RESISTANCE 10000.0
#define NOMINAL_TEMPERATURE 25.0
#define B_COEFFICIENT 3950.0
#define MAX_DAYS 10

float highTemp;
float lowTemp;
float aveTemp;

int lightVal = 0;
unsigned long lightSecs = 0;
int mins = 0;
int hrs = 0;

unsigned long darkSecs = 0;
bool isDay;

unsigned long lastTick = 0;
const unsigned long interval = 1000;  // 1 second


struct DayStats{
  float lowTemp;
  float highTemp;
  float aveTemp;
  unsigned long lightSecs;
};
//obviously 7 days in a week, but I'm forgetful and might need a couple extra
DayStats week[MAX_DAYS];
// fighter of the night index (ah ahhhh)
int dayIndex = 0;
int dayCount = 0;

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
  unsigned long now = millis();

  if (digitalRead(buttonPin) == HIGH) printLog();

  if(now - lastTick >= interval){
    lastTick += interval;  // keeps timing stable

    float tempTemp = getTemp();

    if(tempTemp < lowTemp) lowTemp = tempTemp;
    if(tempTemp > highTemp) highTemp = tempTemp;

    aveTemp = (lowTemp + highTemp) / 2;

    lightVal = analogRead(lightPin);

    if(lightVal > 1000){
      isDay = true;
      darkSecs = 0;
      lightSecs++;
    } else {
      darkSecs++;
    }
    if((darkSecs > 5) && isDay){
      isDay = false;
      Serial.println("End of day, storing variables...");
      storeDay();
      Serial.println("Resetting counters...");
      lowTemp = getTemp();
      highTemp = getTemp();
      lightSecs = 0;
      mins = 0;
      hrs = 0;
      Serial.print("Day: ");
      Serial.print(dayIndex);
      Serial.print(" stored sucessfully. Light exposure: ");
      Serial.print(week[dayIndex-1].lightSecs);
      Serial.print(" Average temp: ");
      Serial.println(week[dayIndex-1].aveTemp);
    }

//    Serial.print("low Temp: ");
//    Serial.println(lowTemp);

//    Serial.print("high temp: ");
//    Serial.println(highTemp);

 //   Serial.print("ave temp: ");
//    Serial.println(aveTemp);

    Serial.print("Light value: ");
    Serial.println(lightVal);
//    Serial.print("light Secs: ");
//    Serial.println(lightSecs);
    Serial.print("dark secs: ");
    Serial.println(darkSecs);

    lcd.setCursor(10, 0);
    lcd.print(aveTemp);

    printTime(lightSecs);
  }
}

void storeDay(){
  week[dayIndex].lowTemp = lowTemp;
  week[dayIndex].highTemp = highTemp;
  week[dayIndex].aveTemp = aveTemp;
  week[dayIndex].lightSecs = lightSecs;

  dayIndex = (dayIndex + 1) % MAX_DAYS;
  if (dayCount < MAX_DAYS) dayCount++;
}

void resetWeek(){
  dayIndex = 0;
  dayCount = 0;
}

void printLog(){
  for(int i = 0; i < dayCount; i++){
      Serial.print("Day ");
      Serial.print(i);
      Serial.print(": Light exposure: ");
      Serial.print(week[i].lightSecs);
      Serial.print(" Average temp: ");
      Serial.println(week[i].aveTemp);
  }
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