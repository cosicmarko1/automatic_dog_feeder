#include <Wire.h>
#include <TimeLib.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonUpPin = 2;
const int buttonDownPin = 3;
const int buttonOKPin = 4;

int setParametersState = 0;      // 0: Hours, 1: Minutes, 2: Seconds, 3: Dog Size, 4: Done
int enteredHours = 0;
int enteredMinutes = 0;
int enteredSeconds = 0;
int selectedDogSize = 0; // 0: Small Dog, 1: Medium Dog, 2: Big Dog

void setup() {
  Serial.begin(115200);

  lcd.begin();  // If your LCD library doesn't require parameters

  // Display "Enter time" message
  lcd.print("Enter time");

  // Set up button pins with internal pull-up resistors
  pinMode(buttonUpPin, INPUT_PULLUP);
  pinMode(buttonDownPin, INPUT_PULLUP);
  pinMode(buttonOKPin, INPUT_PULLUP);
}

void loop() {
  // Check for button presses and update time accordingly
  handleButtons();

  // Display the current time and dog size on the LCD display
  printOnLCD();

  // Wait for 1000 milliseconds
  delay(1000);

  // If time is set and dog size is selected, update the clock
  if (setParametersState == 4) {
    updateTime();
  }
}

void handleButtons() {
  // Check for "Up" button press
  if (digitalRead(buttonUpPin) == LOW) {
    switch (setParametersState) {
      case 0: // Hours
        enteredHours++;
        break;
      case 1: // Minutes
        enteredMinutes++;
        break;
      case 2: // Seconds
        enteredSeconds++;
        break;
      case 3: // Dog Size
        selectedDogSize++;
        break;
    }
    //delay(50);  // Debounce delay
  }

  // Check for "Down" button press
  if (digitalRead(buttonDownPin) == LOW) {
    switch (setParametersState) {
      case 0: // Hours
        enteredHours--;
        break;
      case 1: // Minutes
        enteredMinutes--;
        break;
      case 2: // Seconds
        enteredSeconds--;
        break;
      case 3: // Dog Size
        selectedDogSize--;
        break;
    }
    //delay(50);  // Debounce delay
  }

  // Check for "OK" button press
  if (digitalRead(buttonOKPin) == LOW) {
    updateState();
    //delay(50);  // Debounce delay
  }
}

void incrementEnteredValue() {
  switch (setParametersState) {
    case 0:  // Hours
      enteredHours++;
      if (enteredHours > 23) enteredHours = 0;
      break;
    case 1:  // Minutes
      enteredMinutes++;
      if (enteredMinutes > 59) enteredMinutes = 0;
      break;
    case 2:  // Seconds
      enteredSeconds++;
      if (enteredSeconds > 59) enteredSeconds = 0;
      break;
    case 3:
      selectedDogSize++;
      if(selectedDogSize > 2) selectedDogSize = 0;
  }
}

void decrementEnteredValue() {
  switch (setParametersState) {
    case 0:  // Hours
      enteredHours--;
      if (enteredHours < 0) enteredHours = 23;
      break;
    case 1:  // Minutes
      enteredMinutes--;
      if (enteredMinutes < 0) enteredMinutes = 59;
      break;
    case 2:  // Seconds
      enteredSeconds--;
      if (enteredSeconds < 0) enteredSeconds = 59;
      break;
    case 3:
      selectedDogSize--;
      if(selectedDogSize < 0) selectedDogSize = 2;
  }
}

/*void incrementTimeValue(int* value) {
  (*value)++;
  if (*value > 59) *value = 0;
}
void decrementTimeValue(int* value) {
  (*value)--;
  if (*value < 0) *value = 59;
}
void incrementDogSize() {
  selectedDogSize++;
  if (selectedDogSize > 2) selectedDogSize = 0;
}
void decrementDogSize() {
  selectedDogSize--;
  if (selectedDogSize < 0) selectedDogSize = 2;
} */

void updateState() {
  setParametersState++;

  if (setParametersState > 3) {
    setParametersState = 4;  // Set to 4 to indicate that time setting and dog size selection are done
    // Set the initial time with the entered values
    setTime(enteredHours, enteredMinutes, enteredSeconds, 1, 1, 2024);
  }
}

void printOnLCD() {
  lcd.clear();

  // Print the current state and value on the LCD display
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
    // Display the running clock
    lcd.print(hour());
    printDigits(minute());
    printDigits(second());
    lcd.setCursor(0, 1);
    switch (selectedDogSize) {
      case 0:
        lcd.print(" Small dog");
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
  // Update the time based on the real-time clock
  adjustTime(0.5);
}
