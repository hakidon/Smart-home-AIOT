#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
const char *WIFI_SSID = "xxx"; //your WiFi SSID
const char *WIFI_PASSWORD = "xx"; // your password
const char *MQTT_SERVER = "x.x.x.x"; // your VM instance public IP address
const int MQTT_PORT = 1883;
const char *MQTT_TOPIC = "iot"; // MQTT topic
const int DHT_PIN = 38;
const int DHT_TYPE = DHT11;
DHT dht(DHT_PIN, DHT_TYPE);
WiFiClient espClient;
PubSubClient client(espClient);
void setup_wifi()
{
 delay(10);
 Serial.println("Connecting to WiFi...");
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 while (WiFi.status() != WL_CONNECTED)
 {
 delay(500);
 Serial.print(".");
 }
 Serial.println("WiFi connected");
 Serial.println("IP address: ");
 Serial.println(WiFi.localIP());
}
void setup()
{
 Serial.begin(115200);
 dht.begin();
 setup_wifi();
 client.setServer(MQTT_SERVER, MQTT_PORT);
}
void reconnect()
{
 while (!client.connected())
 {
 Serial.println("Attempting MQTT connection...");
 if (client.connect("ESP32Client"))
 {
 Serial.println("Connected to MQTT server");
 }
 else
 {
 Serial.print("Failed, rc=");
 Serial.print(client.state());
 Serial.println(" Retrying in 5 seconds...");
 delay(5000);
 }
 }
}
void loop()
{
 if (!client.connected())
 {
 reconnect();
 }
 client.loop();
 delay(5000); // adjust the delay according to your requirements
 float temperature = dht.readTemperature();
 char payload[10];
 sprintf(payload, "%.2f", temperature);
 client.publish(MQTT_TOPIC, payload);
}