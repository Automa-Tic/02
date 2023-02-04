#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#include <WebServer.h>
#include <ArduinoOTA.h>
#include <SPI.h>

#include "DHT.h"

#define DHTTYPE DHT11
#define DHTPIN 4
DHT dht(DHTPIN, DHTTYPE);


// Informações de conexão WiFi
const char* ssid = "LIVE TIM_F200_2G";
const char* password = "xpbcn8u37r";

// Informações do servidor MQTT
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char* mqtt_user = "central";
const char* mqtt_password = "central";

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

char LED;
char sensor;

// Informações do esp
bool led;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando a rede: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());
  Serial.println("");
  Serial.println("Conectado ao WiFi");
  Serial.println("Endereço de IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]");

  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ((char)payload[0] == 'L') {
    digitalWrite(LED, HIGH);
    snprintf(msg, MSG_BUFFER_SIZE, "O led está aceso");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("engeasier/led", msg);
  }
  if ((char)payload[0] == 'l') {
    digitalWrite(LED, LOW);
    snprintf(msg, MSG_BUFFER_SIZE, "O led está apagado");
    Serial.print("Publica mensagem: ");
    Serial.println(msg);
    client.publish("engeasier/led", msg);
  }
}

void reconnect () {
  while(!client.connected()) {
    Serial.print("Tentando reconectar ao servidor MQTT");
    String clientId = "ENGEASIER_MQTT";
    clientId += String(random(0Xffff), HEX);

    if(client.connect(clientId.c_str())) {
      Serial.println("Conectado.");

      client.subscribe("engeasier/publisher");
    } else {
      Serial.print("Failed, rc= ");
      Serial.print(client.state());
      Serial.println("Tentando de novvo em 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  LED = 5;
  sensor = 15;
  
  pinMode(LED, OUTPUT);
  pinMode(sensor, OUTPUT);

  Serial.begin(115200);
  setup_wifi();

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if(!client.connected()) {
    reconnect();
  }
  client.loop();
}
