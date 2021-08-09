/*
 Conectar o esp32 a internet.
 @description: Esse programa faz um GET na lista de serviços de vacinação da API da Prefeitura de Vitória Espirito Santo. 
 @author: Andrei Bastos
 @date: 08/08/2021
*/

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

char* ssid = "SSID";
char* password = "PASSWORD";

const char* endpointAPI = "https://agendamento.vitoria.es.gov.br/api/categorias/2/servicos";
uint16_t CAPACITY = 6144; // alocação de memoria dinâmica
uint16_t period = 60000; // tempo entre requisições // 60 segundos

void setup() {
  Serial.begin(115200);   
  connectWifi();  
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
        JsonArray services = getServices();
        printServices(services);
  }
  delay(period);
}

void connectWifi() {
  delay(10);
  Serial.println();
  Serial.print("Tentando se conectar a : ");
  Serial.println(ssid);

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


JsonArray getServices() {
  Serial.print("obtendo serviços de ");
  Serial.println(endpointAPI);
  String servicesResponse = httpGETRequest(endpointAPI);
  JsonArray services =  stringToDocument(servicesResponse);  
  return services; 
}

void printServices(JsonArray services){      
  for(JsonObject service : services) {      
     
  int id = service["id"]; 
  const char* nome = service["nome"]; 
  const char* descricao = service["descricao"]; 

  Serial.print("id: ");
  Serial.println(id);

  Serial.print("nome: ");
  Serial.println(nome);

  /*
  Serial.print("descrição: ");
  Serial.println(descricao);
  */

  Serial.println("----------------");
  Serial.println(" ");
  }
}

String httpGETRequest(const char* endpoint) {
  HTTPClient http;
  String payload = "{}"; 
    
  http.begin(endpoint);  
  int httpResponseCode = http.GET();    
  if (httpResponseCode>0) {    
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);    
  }    
  http.end();
  
  return payload;
}

JsonArray stringToDocument(String jsonString){   
  DynamicJsonDocument doc(CAPACITY);
  
  DeserializationError error = deserializeJson(doc, jsonString);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return doc.to<JsonArray>();
  } 

  JsonArray array = doc.as<JsonArray>();
  return array;
}
