#include <DHT.h>

const int DHT_PIN = 41;
const int DHT_TYPE = DHT11;

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float temperature = dht.readTemperature();
  if (!isnan(temperature)) { // Check if temperature reading is valid
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" °C");
  } else {
    Serial.println("Failed to read temperature from DHT sensor");
  }
  delay(2000); // Delay between temperature readings
}
