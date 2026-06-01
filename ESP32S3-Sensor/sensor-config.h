#ifndef SENSOR_CONFIG_H
#define SENSOR_CONFIG_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "DHT.h"

// ====== Pin definitions ======
#define DHTPIN 2
#define DHTTYPE DHT11
#define PIN_LDR 1

// RGB1 pins
#define PIN_RGB1_R 13
#define PIN_RGB1_G 12
#define PIN_RGB1_B 14

// RGB2 pins
#define PIN_RGB2_R 27
#define PIN_RGB2_G 26
#define PIN_RGB2_B 25

#define PIN_RELAY_PAGAR 16
#define PIN_LIMIT_SWITCH 21
#define PIN_IR_LED 17

// ====== WiFi & MQTT config ======
extern const char* ssid;
extern const char* password;
extern const char* mqtt_server;

// ====== Global objects ======
extern WiFiClient espClient;
extern PubSubClient client;
extern DHT dht;

// ====== Global variables ======
extern unsigned long lastMsg;
extern const unsigned long sendInterval;
extern bool relayState;
extern bool limitSwitchState;

// ====== Function prototypes ======
void setup_wifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);
void setRelay(bool state);
int readLdrPercent();

#endif