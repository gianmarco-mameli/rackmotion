#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <private.h>

// wifi
const char ssid[] = SECRET_SSID;
const char pass[] = SECRET_PASS;
WiFiClient espClient;
PubSubClient client(espClient);

// mqtt
const char mqtt_broker[] = MQTT_BROKER;
const int mqtt_port = 1883;
const char *client_id = "rackmotion";
const char *status_topic = "rackmotion/status";
const char *motion_topic = "rackmotion/motion";

// pir
#define PIRPIN 14 // D5

unsigned long previousMillis = 0;

void InitWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
}

void reconnect()
{
  while (!client.connected())
  {
    if (client.connect(client_id))
    {
      Serial.println("MQTT broker connected");
      client.publish(status_topic, "connected");
      client.publish(motion_topic, "0", true);
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(5000);
    }
  }
}

void InitMqtt()
{
  client.setServer(mqtt_broker, mqtt_port);
  // client.setCallback(callback);
  reconnect();
}

void getPir()
{
  int pir = digitalRead(PIRPIN);
  digitalWrite(LED_BUILTIN, not(pir));
  char message[5];
  itoa(pir, message, 10);
  client.publish(motion_topic, message);
  // Serial.println(pir);
  Serial.println(message);
}

void setup()
{
  Serial.begin(115200);
  InitWiFi();
  delay(1000);
  InitMqtt();
  delay(1000);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIRPIN, INPUT);
  delay(100);
}

void loop()
{

  if (client.connected())
  {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 2000)
    {
      previousMillis = currentMillis;
      getPir();
    }
  }
  else
  {
    reconnect();
  }
}