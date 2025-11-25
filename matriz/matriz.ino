#include <WiFi.h>
#include <PubSubClient.h>
#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 1
#define CS_PIN 14   

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

const char* ssid = "Catatau";      
const char* password = "ze colmeia";

const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

const char* mqtt_topic_subscribe = "esp32/display/letra"; 

WiFiClient espClient;
PubSubClient client(espClient);
String mqtt_client_id; 

void callback(char* topic, byte* payload, unsigned int length) {
  String message = "";
  
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("]: ");
  Serial.println(message);

  display.displayClear();
  
  display.setTextAlignment(PA_CENTER);
  
  display.print(message);
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Conectando em ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Tentando conexão MQTT...");
    
    if (client.connect(mqtt_client_id.c_str())) {
      Serial.println("conectado!");
      
      client.subscribe(mqtt_topic_subscribe);
      Serial.print("Inscrito no tópico: ");
      Serial.println(mqtt_topic_subscribe);
      
    } else {
      Serial.print("falhou, rc=");
      Serial.print(client.state());
      Serial.println(" tente novamente em 5s");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);

  display.begin();
  display.setIntensity(5); 
  display.displayClear();
  display.setTextAlignment(PA_CENTER);
  display.print("?"); 

  setup_wifi();

  mqtt_client_id = "ESP32Matrix-";
  mqtt_client_id += WiFi.macAddress();
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (display.displayAnimate()) {
  }
}