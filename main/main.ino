#include <Wire.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>
#include "HX711.h"

// DOUT, SCK scale for food -> 14, 12
HX711 scaleFood(14, 12);
// DOUT, SCK scale for water -> 25, 26
HX711 scaleWater(25, 26);

// LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// calibration factor for 1kg scale for food is -1024, -896
float calibrationFactorScaleFood = -1208;
float weightFood;
float ouncesFood;
// calibration factor for 1kg scale for water is -1208
float calibrationFactorScaleWater = -1209;
float weightWater;
float ouncesWater;

// switch for the water pump
const int waterPumpSwitch = 23;

// food stepper motor
const int stepperFoodEnablePin = 19;
const int stepperFoodDirPin = 13;
const int stepperFoodStepPin = 5;

// cookies stepper motor
const int stepperCookiesEnablePin = 16;
const int stepperCookiesDirPin = 18;
const int stepperCookiesStepPin = 17;

// buttons
const int buttonUpPin = 2;
const int buttonDownPin = 3;
const int buttonOKPin = 4;

// variables for setting time and dog size
int setParametersState = 0; // 0: Hours, 1: Minutes, 2: Seconds, 3: Dog Size, 4: Done
int enteredHours = 0;
int enteredMinutes = 0;
int enteredSeconds = 0;
int selectedDogSize = 0; // 0: Small Dog, 1: Medium Dog, 2: Big Dog

// food amounts for dog sizes (in grams)
int foodAmountSmallDog = 30;
int foodAmountMediumDog = 60;
int foodAmountBigDog = 90;

// water amount for the bowl (bowl has capacity of 140ml)
int waterMaxAmount = 130;

unsigned long startTime; 
bool waterPouredAfterAnHour = false; 

void setup() {
  Serial.begin(9600);
 
  scaleFood.set_scale(calibrationFactorScaleFood);
  scaleFood.tare(); 

  scaleWater.set_scale(calibrationFactorScaleWater);
  scaleWater.tare();

  Serial.println("Readings: ");

  lcd.begin(); 
  lcd.print("Automatic Dog");
  lcd.setCursor(0, 1);
  lcd.print("   Feeder");
  delay(5000);
  lcd.clear();
  lcd.print("Enter time");
  delay(3000);

  // turn off both drivers for stepper motors and water pump when starting
  digitalWrite(stepperFoodEnablePin, HIGH);
  digitalWrite(stepperCookiesEnablePin, HIGH);
  digitalWrite(waterPumpSwitch, LOW);

  pinMode(waterPumpSwitch, OUTPUT);
  pinMode(stepperFoodEnablePin, OUTPUT);
  pinMode(stepperFoodDirPin, OUTPUT);
  pinMode(stepperFoodStepPin, OUTPUT);
  pinMode(stepperCookiesEnablePin, OUTPUT);
  pinMode(stepperCookiesDirPin, OUTPUT);
  pinMode(stepperCookiesStepPin, OUTPUT);
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonOKPin, INPUT_PULLUP);
}

void loop() {
  // turn off both drivers for stepper motors and water pump when starting
  digitalWrite(stepperFoodEnablePin, HIGH);
  digitalWrite(stepperCookiesEnablePin, HIGH);
  digitalWrite(waterPumpSwitch, LOW);

  handleButtons();
  printOnLCD();
  delay(500);

  if (setParametersState == 4) {
    updateTime();
  }

  // scale for food
  Serial.println("Reading: ");
  weightFood = scaleFood.get_units(), 10;
  if (weightFood < 0) {
    weightFood = 0.00;
  }
  ouncesFood = weightFood * 0.035274;
  Serial.print(weightFood);
  Serial.println(" g of food"); 

  // scale for water
  Serial.println("Reading: ");
  weightWater = scaleWater.get_units(), 10;
  if (weightWater < 0) {
    weightWater = 0.00;
  }
  ouncesWater = weightWater * 0.035274;
  Serial.print(weightWater);
  Serial.println(" ml of water"); 
  
  // dispensing food and cookies, and pouring water in morning
  if(hour() == 7 && minute() == 0 && second() <= 59) {
    if(selectedDogSize == 0 && (weightFood < foodAmountSmallDog)) { // small dog
      dispenseFood();
      dispenseCookies();
    }
    if(selectedDogSize == 1 && (weightFood < foodAmountMediumDog)) { // medium dog
      dispenseFood();
      dispenseCookies();
    }
    if(selectedDogSize == 2 && (weightFood < foodAmountBigDog)) { // big dog
      dispenseFood();
      dispenseCookies();
    }
    if(weightWater < waterMaxAmount) { 
      pourWater();
    }
  }

  // dispensing food and cookies, and pouring water in the evening
  if(hour() == 19 && minute() == 0 && second() <= 59) {
    if(selectedDogSize == 0 && (weightFood < foodAmountSmallDog)) { // small dog
      dispenseFood();
      dispenseCookies();
    }
    if(selectedDogSize == 1 && (weightFood < foodAmountMediumDog)) { // medium dog
      dispenseFood();
      dispenseCookies();
    }
    if(selectedDogSize == 2 && (weightFood < foodAmountBigDog)) { // big dog
      dispenseFood();
      dispenseCookies();
    }
    if(weightWater < waterMaxAmount) { 
      pourWater();
    }
  }

  // checking every hour if there is enough water in the bowl
  if (!waterPouredAfterAnHour) {
    if(weightWater < waterMaxAmount) {
      pourWater();
      waterPouredAfterAnHour = true;
      startTime = millis(); 
    }
  }

  // 3600000 miliseconds equals to 1 hour
  if (waterPouredAfterAnHour && millis() - startTime >= 3600000) {
    waterPouredAfterAnHour = false;
  }
}

void dispenseFood() {
  digitalWrite(stepperFoodEnablePin, LOW);
  digitalWrite(stepperFoodDirPin, HIGH);

  // moving stepper motor in four steps
  // each step makes 90 degrees rotation
  for(int x = 0; x < 50; x++){
    digitalWrite(stepperFoodStepPin, HIGH);                                     
    delayMicroseconds(5000); 
    digitalWrite(stepperFoodStepPin, LOW);
    delayMicroseconds(5000); 
  }
  delay(2000);

  digitalWrite(stepperFoodDirPin, LOW);
  digitalWrite(stepperFoodEnablePin, HIGH);
}

void dispenseCookies() {
  digitalWrite(stepperCookiesEnablePin, LOW);
  digitalWrite(stepperCookiesDirPin, HIGH);

  for(int x = 0; x < 50; x++){
    digitalWrite(stepperCookiesStepPin, HIGH);                                      
    delayMicroseconds(5000); 
    digitalWrite(stepperCookiesStepPin, LOW);  
    delayMicroseconds(5000); 
  }
  delay(2000);

  digitalWrite(stepperCookiesDirPin, LOW);
  digitalWrite(stepperCookiesEnablePin, HIGH);
}

void pourWater() {
  digitalWrite(waterPumpSwitch, HIGH); 
  delay(500);             
  digitalWrite(waterPumpSwitch, LOW);  
  delay(500);      
}

void handleButtons() {
  if (digitalRead(buttonUpPin) == LOW) {
    switch (setParametersState) {
      case 0:  
      if (enteredHours < 23) enteredHours++;
      else enteredHours = 0;
      break;
    case 1:  
      if (enteredMinutes < 59) enteredMinutes++;
      else enteredMinutes = 0;
      break;
    case 2:  
      if (enteredSeconds < 59) enteredSeconds++;
      else enteredSeconds = 0;
      break;
    case 3:
      if (selectedDogSize < 2) selectedDogSize++;
      else selectedDogSize = 0;
      break;
    }
  }

  if (digitalRead(buttonDownPin) == LOW) {
    switch (setParametersState) {
      case 0:  
      if (enteredHours > 0) enteredHours--;
      else enteredHours = 23;
      break;
    case 1:  
      if (enteredMinutes > 0) enteredMinutes--;
      else enteredMinutes = 59;
      break;
    case 2:  
      if (enteredSeconds > 0) enteredSeconds--;
      else enteredSeconds = 59;
      break;
    case 3:
      if (selectedDogSize > 0) selectedDogSize--;
      else selectedDogSize = 2;
      break;
    }
  }

  if (digitalRead(buttonOKPin) == LOW) {
    updateState();
  }
}

void updateState() {
  setParametersState++;

  if (setParametersState > 3) {
    setParametersState = 4;  // Set to 4 to indicate that time setting and dog size selection are done
    setTime(enteredHours, enteredMinutes, enteredSeconds, 1, 1, 2024);
  }
}

void printOnLCD() {
  lcd.clear();

  if (setParametersState <= 2) {
    lcd.print("Set ");
    if (setParametersState == 0) {
      lcd.print("hours: ");
      lcd.print(enteredHours);
    } else if (setParametersState == 1) {
      lcd.print("minutes: ");
      lcd.print(enteredMinutes);
    } else if (setParametersState == 2) {
      lcd.print("seconds: ");
      lcd.print(enteredSeconds);
    }

  } else if (setParametersState == 3) {
    lcd.print("Select dog size:");
    lcd.setCursor(0, 1);
    switch (selectedDogSize) {
      case 0:
        lcd.print("Small (1-5kg)");
        break;
      case 1:
        lcd.print("Medium (5-15kg)");
        break;
      case 2:
        lcd.print("Big (15-30kg)");
        break;
    }

  } else {
    lcd.print(hour());
    printDigits(minute());
    printDigits(second());
    lcd.setCursor(0, 1);
    switch (selectedDogSize) {
      case 0:
        lcd.print("Small dog");
        break;
      case 1:
        lcd.print("Medium dog");
        break;
      case 2:
        lcd.print("Big dog");
        break;
    }
  }
}

void printDigits(int digits) {
  lcd.print(":");
  if (digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

void updateTime() {
  adjustTime(0.5);
}