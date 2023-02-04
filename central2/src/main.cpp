#include <sMQTTBroker.h>
#include <ArduinoOTA.h>

sMQTTBroker broker;

unsigned long Time;
unsigned long freeRam;

IPAddress local_IP(192, 168, 1, 251);
IPAddress gateway(192, 168, 1, 254);
IPAddress subnet(255, 255, 255, 0);

void otaEsp() {
  ArduinoOTA.setHostname("Broker32");

  ArduinoOTA.onStart([]() {
    String type;
    if(ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    }
    else {
      type = "filesystem";
    }

    Serial.println("Start updating" + type);
  })
  .onEnd([] () {
    Serial.println("\nEnd");
  });
}

void setup() {
  Serial.begin(115200);

  Serial.println("Tentando fixar IP");
  if(!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Falha ao conectar");
  }
  else {
    Serial.println("Conectado com sucesso");
  }

  const char* ssid = "LIVE TIM_F200_2G";
  const char* password = "xpbcn8u37r";

  Serial.println("Tentando conectar a rede");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("Conectado ao WiFi");
  Serial.print("IP address:\t");

  Serial.println(WiFi.localIP());

  otaEsp();

  const unsigned short mqttPort = 1883;
  broker.init(mqttPort);
}

void loop() {
  ArduinoOTA.handle();
  broker.update();

  if(millis() - Time > 1000) {
    Time=millis();
    if(ESP.getFreeHeap() != freeRam) {
      freeRam = ESP.getFreeHeap();
      Serial.print("RAM: ");
      Serial.println(freeRam);
    }
  }
}

