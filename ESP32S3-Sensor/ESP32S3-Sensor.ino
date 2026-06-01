#include "sensor-config.h"

// ====== Definisi variabel global (sesuai deklarasi extern di header) ======
const char* ssid = "Naxytsa";
const char* password = "Segara.123";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

unsigned long lastMsg = 0;
const unsigned long sendInterval = 10000;  // 10 detik
bool relayState = false;
bool limitSwitchState = false;

// ====== Implementasi fungsi ======
void setup_wifi() {
  delay(10);
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());
}

void reconnect() {
  // Jangan gunakan while loop tanpa yield/delay kecil
  if (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "smarthome-" + String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe("control/door/#");
      client.subscribe("control/lamp/#");
      Serial.println("Subscribed to control/door/#");
      Serial.println("Subscribed to control/lamp/#");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(". Retry in 5 seconds.");
    }
  }
}

void setRelay(bool state) {
  digitalWrite(PIN_RELAY_PAGAR, state ? HIGH : LOW);
  Serial.printf("Relay set to %s\n", state ? "ON" : "OFF");
}

int readLdrPercent() {
  int raw = analogRead(PIN_LDR);
  int pct = map(raw, 0, 4095, 100, 0);
  if (pct < 0) pct = 0;
  if (pct > 100) pct = 100;
  return pct;
}

void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) msg += (char)payload[i];
  msg.trim();
  Serial.printf("MQTT recv [%s] %s\n", topic, msg.c_str());

  String t = String(topic);

  // Kontrol relay / pintu pagar
  if (t.startsWith("control/door/")) {
    bool shouldOpen = false;
    StaticJsonDocument<128> doc;
    DeserializationError err = deserializeJson(doc, msg);
    if (!err && doc.containsKey("state")) {
      String s = doc["state"].as<String>();
      shouldOpen = s.equalsIgnoreCase("on") || s.equalsIgnoreCase("open");
    } else {
      shouldOpen = msg.equalsIgnoreCase("on") || msg.equalsIgnoreCase("open") || msg.equals("1");
    }
    setRelay(shouldOpen);
    relayState = shouldOpen;
    Serial.printf("  -> Door (relay) %s\n", shouldOpen ? "OPEN" : "CLOSE");
    return;
  }
  // (Nanti bisa ditambahkan kontrol lampu, AC, dll.)
}

// ====== Setup ======
void setup() {
  Serial.begin(115200);
  
  pinMode(PIN_RELAY_PAGAR, OUTPUT);
  digitalWrite(PIN_RELAY_PAGAR, LOW);
  pinMode(PIN_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(PIN_LDR, INPUT);
  
  dht.begin();
  delay(2000);  
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

// ====== Loop ======
void loop() {
  client.loop();
  static unsigned long lastReconnectAttempt = 0;
  unsigned long now = millis();
  if (!client.connected() && (now - lastReconnectAttempt > 5000)) {
    lastReconnectAttempt = now;
    reconnect();
  }
  
  // Kirim data sensor setiap sendInterval
  if (now - lastMsg > sendInterval) {
    lastMsg = now;
    
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
    int retry = 0;
    // Coba baca DHT hingga 5 kali
    while (retry < 5) {
      if (!isnan(temperature) && !isnan(humidity)) break;
      delay(200);
      retry++;
    }
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor after retries");
      temperature = 0.0;
      humidity = 0.0;
    }
    
    int ldrPercent = readLdrPercent();
    limitSwitchState = digitalRead(PIN_LIMIT_SWITCH) == LOW;
    
    StaticJsonDocument<256> doc;
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    doc["lux"] = ldrPercent;
    doc["limitSwitch"] = limitSwitchState;
    doc["relay"] = relayState;
    
    char jsonBuffer[256];
    serializeJson(doc, jsonBuffer);
    
    if (client.publish("kamar1/data", jsonBuffer)) {
      Serial.println("Data sent: " + String(jsonBuffer));
    } else {
      Serial.println("Failed to publish data");
    }
  }
  yield();
}