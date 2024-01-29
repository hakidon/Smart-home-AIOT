#include <DHT.h>
#include <PubSubClient.h>
#include <WiFiEsp.h>

const int relay_motor = 10;
#define RED_LED_PIN 48
#define GREEN_LED_PIN 38
#define IR_SENSOR_PIN 7
#define DHT_PIN 41
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);
bool securityEnabled = false;
bool motorState = false;
bool greenLedState = false;

char ssid[] = "xxx";
char pass[] = "xxx_xxx";
const char* mqtt_server = "x.x.x.x";
const int mqtt_port = 1883;
//Apply authentication
const char* mqtt_user = "xxx";
const char* mqtt_password = "xxx";

WiFiEspClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(relay_motor, OUTPUT);
  pinMode(IR_SENSOR_PIN, INPUT);
  
  Serial.begin(9600);
  
  dht.begin();
  setup_wifi();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  
  if (!client.connected()) {
    reconnect();
  }

  client.subscribe("motor");
  client.subscribe("green_led");
}

//Connect to wifi
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect("ArduinoClient", mqtt_user, mqtt_password)) {
    } else {
      delay(5000);
    }
  }
}

void loop() {
  int irValue = digitalRead(IR_SENSOR_PIN);
  controlMotorAndLED(irValue);
  checkTemperature();
  handleAlarm();
  handleMQTT();
}

//Turn on light and fan if presence detected
void controlMotorAndLED(int irValue) {
  if (irValue == HIGH && !securityEnabled) {
    digitalWrite(GREEN_LED_PIN, HIGH);
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(relay_motor, HIGH);
  } else {
    digitalWrite(GREEN_LED_PIN, LOW);
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(relay_motor, LOW);
  }

  digitalWrite(relay_motor, motorState ? HIGH : LOW);
  digitalWrite(GREEN_LED_PIN, greenLedState ? HIGH : LOW);
}

//Turn on fan when treshold passed e.g. 40 degree
void checkTemperature() {
  float temperature = dht.readTemperature();
  if (!isnan(temperature) && temperature >= 40) {
    digitalWrite(relay_motor, HIGH);
  } else {
    digitalWrite(relay_motor, LOW);
  }
}

//Set blinking red LED and buzzer as alarm
void setAlarm() {
  int total = 5000;

  for (int i = 0; i < total / 100; i++) {
    tone(12, (i % 2 == 0) ? 2000 : 1000, 500);
    delay(i * 10);
    digitalWrite(RED_LED_PIN, !digitalRead(RED_LED_PIN));
    delay(50);
  }

  noTone(12);
  digitalWrite(RED_LED_PIN, LOW);
}


void handleAlarm() {
  // Listen to the MQTT topic "security" for setting the security status
  if (client.connected()) {
    client.subscribe("security");
    client.loop();
  }

  // If security is enabled and IR sensor detects something, set off the alarm
  if (securityEnabled && digitalRead(IR_SENSOR_PIN) == HIGH) {
    setAlarm();
  }
}

//Sent data to MQTT server
void handleMQTT() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  float temperature = dht.readTemperature();
  if (!isnan(temperature)) {
    client.publish("dht", String(temperature).c_str());
  }

  int irValue = digitalRead(IR_SENSOR_PIN);
  client.publish("ir_sensor", irValue == HIGH ? "1" : "0");
}

//Listen data from MQTT server
void callback(char* topic, byte* payload, unsigned int length) {
  String topicStr = String(topic);

  if (topicStr == "motor") {
    motorState = (payload[0] == 'O' && payload[1] == 'N') ? true : false;
  } else if (topicStr == "green_led") {
    greenLedState = (payload[0] == 'O' && payload[1] == 'N') ? true : false;
  } else if (topicStr == "security") {
    // Set the security status based on the payload
    securityEnabled = (payload[0] == 'O' && payload[1] == 'N') ? true : false;
  }
}