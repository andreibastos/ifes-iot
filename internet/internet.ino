/*
 Conectar o esp32 a internet.
 @description: Esse programa faz um GET na lista de serviços de vacinação da API da Prefeitura de Vitória Espirito Santo. 
 @author: Andrei Bastos
 @date: 08/08/2021
*/

/* 
 * bibliotecas necessárias 
 */
#include <WiFi.h> // wifi
#include <HTTPClient.h> // requisições http
#include <ArduinoJson.h> // json 

/*
 * identificação da rede wifi
 */
char* ssid = "SSID";
char* password = "PASSWORD";

/* 
 *  variáveis globais
 */  
const char* endpointAPI = "https://agendamento.vitoria.es.gov.br/api/categorias/2/servicos"; // url da api da prefeitura
uint16_t CAPACITY = 6144; // alocação de memoria dinâmica
uint16_t period = 60000; // tempo entre requisições // 60 segundos

// configuração inicial
void setup() {
  Serial.begin(115200);
  connectWifi();   // conectar na wifi
}

// loop principal
void loop() {
  // se tiver conectado na wifi
  if(WiFi.status()== WL_CONNECTED){        
        JsonArray services = getServices(); // obtem a lista de serviços        
        printServices(services); // imprime os serviços
  }
  delay(period); // aguarda o periodo
}

// conectar na wifi
void connectWifi() {
  delay(10);
  Serial.println();
  Serial.print("Tentando se conectar a : ");
  Serial.println(ssid);

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


// função que retorna a lista de serviços como jsonArray
JsonArray getServices() {
  Serial.print("obtendo serviços de ");
  Serial.println(endpointAPI);
  String servicesResponse = httpGETRequest(endpointAPI); // requisição de get 
  JsonArray services =  stringToDocument(servicesResponse); // transforma para array de elementos
  return services; // retorna a lista de serviços
}

// função que imprime os serviços
void printServices(JsonArray services){      
  for(JsonObject service : services) {           
    int id = service["id"]; 
    const char* nome = service["nome"]; 
    const char* descricao = service["descricao"]; 
  
    Serial.print("id: ");
    Serial.println(id);
  
    Serial.print("nome: ");
    Serial.println(nome);  

    Serial.println("----------------");
    Serial.println(" ");
  }
}

// função que realiza um get em um endpoint qualquer
String httpGETRequest(const char* endpoint) {
  HTTPClient http;
  String payload = "{}"; 
  
  http.begin(endpoint); // prepara a requisição
  int httpResponseCode = http.GET(); // realiza a requisição get
  if (httpResponseCode==200) {  // se a requisição deu sucesso
    payload = http.getString(); // recebe o response
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);    
  }    
  http.end(); // finaliza a requisição
  
  return payload; // retorna o payload 
}

// função que converte string para document
JsonArray stringToDocument(String jsonString){   
  DynamicJsonDocument doc(CAPACITY); // capacidade do documento

  // tratativa de erro
  DeserializationError error = deserializeJson(doc, jsonString);  
  if (error) {
    Serial.print(F("deserializeJson() falhou: "));
    Serial.println(error.f_str());
    return doc.to<JsonArray>(); // retorna lisa vazia
  } 

  JsonArray array = doc.as<JsonArray>(); // transforma para array
  return array; // retorna o array
}
