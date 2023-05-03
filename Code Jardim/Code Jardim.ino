#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

// Replace with your network credentials
const char* ssid               = "WIFI-FACENS";
const char* password           = "iOt#F@c0504";

// Replace with your MQTT broker address
const char* mqttServer = "34.234.193.23";
const int mqttPort = 1883;

// Replace with your MQTT topic
const char* mqttTopic = "/smart/jardim_A/sub/";

// Define NTP client to get the time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", -10800, 60000);

// Relay pin
#define relayPin   14
#define ledEnvio   12

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  pinMode(ledEnvio, OUTPUT);
  pinMode(2,OUTPUT);
  digitalWrite(2,HIGH);
  connectToWiFi();
  // Connect to Wi-Fi
  

  // Initialize NTP client
  timeClient.begin();

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);
  client.setCallback(mqttCallback);
}

void loop() {

    if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi connection lost. Reconnecting...");
    connectToWiFi();
  }
  
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  timeClient.update();

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();

  if (currentHour == 16 && currentMinute >= 0 && currentMinute <= 5) {
    client.publish("/smart/jardim_A/desativado/", "RELAY DESATIVADO");
    digitalWrite(relayPin, HIGH); // Turn on the relay
    digitalWrite(ledEnvio,HIGH);
  } else {
    digitalWrite(relayPin, LOW); // Turn off the relay
    client.publish("/smart/jardim_A/ativado/", "RELAY ATIVADO");

  }

  delay(1000); // Update every second
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  if (message == "SHAZAM") {
    Serial.println("ATIVADO");
    digitalWrite(relayPin, HIGH); // Turn on the relay
    client.publish("/smart/jardim_A/ativado_shazam/", "RELAY ATIVADO PELO O PODER");
    Seri0   al.println("RELAY ATIVADO PELO O PODER");
    digitalWrite(2,LOW);
    delay(300000);
    digitalWrite(relayPin, LOW); // Turn on the relay
    client.publish("/smart/jardim_A/desativado_shazam/", "RELAY DESATIVADO");
    Serial.println("RELAY DESATIVADO");
    digitalWrite(2,HIGH);
    
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqttTopic);
      client.publish("/smart/jardim_A/conectado/", "CONECTADO");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void connectToWiFi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
     client.publish("/smart/jardim_A/conectado/", "CONECTADO");
  }
  Serial.println("Connected to WiFi");
}
