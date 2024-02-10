#include <Wire.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonUpPin = 2;
const int buttonDownPin = 3;
const int buttonOKPin = 4;

int setParametersState = 0; // 0: Hours, 1: Minutes, 2: Seconds, 3: Dog Size, 4: Done
int enteredHours = 0;
int enteredMinutes = 0;
int enteredSeconds = 0;
int selectedDogSize = 0; // 0: Small Dog, 1: Medium Dog, 2: Big Dog

void setup() {
  Serial.begin(115200);

  lcd.begin();  

  lcd.print("Automatic Dog");
  lcd.setCursor(0, 1);
  lcd.print("   Feeder");
  delay(2000);
  lcd.clear();

  lcd.print("Enter time");

  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonOKPin, INPUT_PULLUP);
}

void loop() {
  handleButtons();

  printOnLCD();

  delay(500);

  if (setParametersState == 4) {
    updateTime();
  }
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