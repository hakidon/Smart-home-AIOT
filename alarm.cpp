#include <DHT.h>

const int DHT_PIN = 41;
const int DHT_TYPE = DHT11;

#define TEMPERATURE_THRESHOLD 34 // Temperature threshold for sounding the alarm (in °C)
#define ALARM_FREQUENCY 1000 // Frequency for the alarm sound (in Hz)
#define LED_PIN 48
#define IR_SENSOR_PIN 7 // Assuming IR sensor is connected to pin 13
const int SPEAKER_PIN = 12; // Assuming speaker is connected to pin 12
int count = 0;
int melody[] = { ALARM_FREQUENCY };
int noteDurations[] = { 100 }; // Duration of the alarm sound (in milliseconds)

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  pinMode(LED_PIN, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT); // Set IR sensor pin as input
  Serial.begin(9600);
  dht.begin();
}


void setAlarm() {
  // Assuming only one note in the melody array
  int total = 5000; // Total duration of the alarm (in milliseconds)

  // Loop for generating the alarm tone
  for (int i = 0; i < total / 100; i++) {
    // Assuming you have a frequency value defined somewhere for the alarm tone
    // tone() function parameters: (pin, frequency, duration)
    tone(12, (i % 2 == 0) ? 2000 : 1000, 500); // Adjusted duration based on loop iteration

    // Delay for the tone (on)
    delay(i * 10);

    // Toggle the LED
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));

    // Delay for LED toggling (off)
    delay(50);
  }

  // After the alarm, turn off the tone and LED
  noTone(12); // Stop the tone
  digitalWrite(LED_PIN, LOW); // Turn off LED
}

void soundAlarm() {
  for (int thisNote = 0; thisNote < 1; thisNote++) {
    int noteDuration = noteDurations[thisNote];
    tone(12, melody[thisNote], noteDuration);
    delay(noteDuration);
  }
}

void ledAlarm() {
  digitalWrite(LED_PIN, HIGH); // Turn on LED
  delay(500); // Wait for 0.5 second
  digitalWrite(LED_PIN, LOW); // Turn off LED
  delay(500); // Wait for 0.5 second
}

void loop() {
  float temperature = dht.readTemperature();
  int sensorStatus = digitalRead(IR_SENSOR_PIN); // Read sensor status

  if (sensorStatus == LOW) { // Assuming HIGH means motion detected
    setAlarm();
    Serial.println("!!!!!"); // Print "Motion Detected!" in serial monitor
  }

  delay(1000); // Delay between temperature readings
}
