#include <Arduino.h>

#include <WiFi.h>
#include <PubSubClient.h>

#define pinLED1 2

// Informações de conexão WiFi
const char* SSID = "LIVE TIM_F200_2G";
const char* PASSWORD = "xpbcn8u37r";
WiFiClient wifiClient;

// Informações do servidor MQTT
const char* BROKER_MQTT = "192.168.1.251";
const int BROKER_PORT = 1883;

#define ID_MQTT "BCI02"
#define TOPIC_SUBESCRIBE "BCI"
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
    conectarMQTT();
  }

  String clientId = "ENGEASIER_MQTT";
  clientId += String(random(0Xffff), HEX);
  if(MQTT.connect(clientId.c_str())) {
      Serial.println("Conectado.");

      MQTT.subscribe("BCIBotao1");
    } 

  conectarWifi();
}
void receberPacote(char* topic, byte* payload, unsigned int length) {
  String msg;

  for(int i = 0; i < length; i++) {
    Serial.println((char)payload[i]);
    
    char c = (char)payload[i];
    msg += c;
  }

  if(msg == "1") {
    digitalWrite(pinLED1, HIGH);
  }
  if(msg == "0") {
    digitalWrite(pinLED1, LOW);
  }
}


void setup() {
  pinMode(pinLED1 , OUTPUT);
  Serial.begin(115200);

  conectarWifi();
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(receberPacote);
}

void loop() {
  mantemConexoes();
  MQTT.loop();
}


