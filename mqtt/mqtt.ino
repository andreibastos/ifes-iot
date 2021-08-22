/*
 Conectar o esp32 a internet.
 @description: Esse programa faz um GET na lista de serviços de vacinação da API
 da Prefeitura de Vitória Espirito Santo.
 @author: Andrei Bastos
 @date: 22/08/2021
*/

/*
 * bibliotecas necessárias
 */
#include <PubSubClient.h> // mqtt
#include <WiFi.h>         // wifi

/*
 * identificação da rede wifi
 */
char *ssid = "SSID";
char *password = "PASSWORD";

/*
 *  variáveis globais
 */
uint16_t period = 60000; // tempo entre requisições // 60 segundos

WiFiClient wifiClient;               // client wifi
PubSubClient mqttClient(wifiClient); // cliente mqtt

char *mqttServer = "broker.hivemq.com"; // endereço do servidor
int mqttPort = 1883;                    // porta do servidor

// configuração inicial
void setup() {
  Serial.begin(115200);
  connectWifi(); // conectar na wifi
  setupMQTT();   // configurar o mqtt
}

// loop principal
void loop() {
  // se tiver não conectado na wifi
  if (!wifiIsConnected()) {
    connectWifi(); // conecta na wifi
  }
  // se não tiver conectado no servidor
  if (!mqttClient.connected()) {
    connectMQTTServer(); // conecta no servidor
  }

  float temperature = getTemperature(); // obtém dados de temperatura
  String data = String(temperature);    // converte float para string
  mqttClient.publish("/andrei/room/temperature", data.c_str()); // envia o dado
  Serial.print("Enviado ");
  Serial.print(data);
  Serial.println(" para /andrei/room/temperature");  

  mqttClient.loop(); // loop do mqtt client
  delay(period);     // aguarda o periodo
}

// verificar se o wifi está conectado
bool wifiIsConnected() { return (WiFi.status() == WL_CONNECTED); }

// conectar na wifi
void connectWifi() {
  delay(10);
  Serial.println();
  Serial.print("Tentando se conectar a : ");
  Serial.print(ssid);

  // tenta conectar com as credenciais
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereço IP: ");
  Serial.println(WiFi.localIP());
}

// setar o servidor e conectar uma vez
void setupMQTT() {
  mqttClient.setServer(mqttServer, mqttPort);
  connectMQTTServer();
}

// conectar no servidor mqtt
void connectMQTTServer() {
  while (!mqttClient.connected()) {
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    if (mqttClient.connect(clientId.c_str())) { // tenta conectar
      // Serial.print("MQTT Connected on ");
      // Serial.println(mqttServer);
    } else {
      Serial.print("falhou, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" tentando em 5 segundos...");
      delay(5000);
    }
  }
}

// retorna a temperatura entre 16.0 e 80.0 °C
float getTemperature() { return random(1600, 8000) / 100.0; }
