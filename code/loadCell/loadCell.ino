#include "HX711.h"

HX711 scaleFood(22, 21); 
HX711 scaleWater(23, 19);

// calibration factor for 1kg scale for food is -900
// calibration factor for 1kg scale for water is -1000
float calibration_factor_scaleFood = -900;
float calibration_factor_scaleWater = -1000;
float unitsFood;
float ouncesFood;
float unitsWater;
float ouncesWater;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 weighing");
  scaleFood.set_scale(calibration_factor_scaleFood);
  scaleFood.tare();  // Reset the scale to 0
  scaleWater.set_scale(calibration_factor_scaleWater);
  scaleWater.tare();  // Reset the scale to 0
  Serial.println("Readings: ");
}

void loop() {
  Serial.println("Reading: ");
  unitsFood = scaleFood.get_units(), 10;
  unitsWater = scaleWater.get_units(), 10;
  if (unitsFood < 0) {
    unitsFood = 0.00;
  }
  if (unitsWater < 0) {
    unitsWater = 0.00;
  }
  ouncesFood = unitsFood * 0.035274;
  ouncesWater = unitsWater * 0.035274;
  Serial.print(unitsFood);
  Serial.println(" grams of food");
  Serial.print(unitsWater);
  Serial.println(" mililiters of water");
  delay(1000);
}
