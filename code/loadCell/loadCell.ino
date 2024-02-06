#include "HX711.h"

HX711 scale(23, 19); 

// calibration factor for 1kg scale is -915
float calibration_factor = -915;
float units;
float ounces;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 weighing");
  scale.set_scale(calibration_factor);
  scale.tare();  // Reset the scale to 0
  Serial.println("Readings: ");
}

void loop() {
  Serial.print("Reading: ");
  units = scale.get_units(), 10;
  if (units < 0) {
    units = 0.00;
  }
  ounces = units * 0.035274;
  Serial.print(units);
  Serial.println(" grams");
  delay(1000);
}
