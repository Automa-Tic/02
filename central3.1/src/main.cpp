#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define pinBotao1 12

// Informações de conexão WiFi
const char* SSID = "LIVE TIM_F200_2G";
const char* PASSWORD = "xpbcn8u37r";
WiFiClient wifiClient;

// Informações do servidor MQTT
const char* BROKER_MQTT = "mqtt.eclipseprojects.io";
const int BROKER_PORT = 1883;

#define ID_MQTT "BCI01"
#define TOPIC_PUBLISH "BCIBotao1"
PubSubClient MQTT(wifiClient);

//Funções 
void conectarWifi() {
  while (WiFi.status() == WL_CONNECTED) {
    return;
  }

  Serial.print("Conectando-se a rede: ");
  Serial.print(SSID);
  Serial.println("  Aguarde!");

  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println();
  Serial.print("Conectado com sucesso a rede: ");
  Serial.print(SSID);
  Serial.print("  IP obtido: ");
  Serial.println(WiFi.localIP());

}
void conectarMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentando conectar ao broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if(MQTT.connect(ID_MQTT)) {
      Serial.println("Conectado ao broker com sucesso.");
    }
    else {
      Serial.print("Falha ao conectar ao broker.");
      Serial.println("Nova tentativa de conexão em 5 segundos.");
    }
  }

}
void mantemConexoes() {
  if(!MQTT.connected()) {
    //conectarMQTT();
  }

  conectarWifi();
}
void enviaValores() {
 static bool estadoBotao1 = HIGH;
 static bool estadoBotao1Ant = HIGH;
 static unsigned long debounceBotao1;

 estadoBotao1 = digitalRead(pinBotao1);
 if((millis() - estadoBotao1) > 30) {
  if(!estadoBotao1 && estadoBotao1Ant) {
    MQTT.publish(TOPIC_PUBLISH, "1");
    Serial.println("botao APERTADO. playload enviado");

    debounceBotao1 = millis();
  }

  else if(estadoBotao1 && !estadoBotao1Ant) {
    MQTT.publish(TOPIC_PUBLISH, "0");
    Serial.println("botao SOLTO. playload enviado");

    debounceBotao1 = millis();
  }
 }
 estadoBotao1Ant = estadoBotao1;
}


void setup() {
  pinMode(pinBotao1, INPUT_PULLUP);
  Serial.begin(115200);

  conectarWifi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
}

void loop() {
  mantemConexoes();
  enviaValores();

  MQTT.loop();
}


