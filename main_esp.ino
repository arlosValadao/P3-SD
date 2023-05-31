#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <Timer.h>
#include <WiFiUdp.h>

#include <string.h> 


#ifndef STASSID
#define STASSID "INTELBRAS"
#define STAPSK  "Pbl-Sistemas-Digitais"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

//ESP na rede
const char* host = "ESP-10.0.0.107";
IPAddress local_IP(10, 0, 0, 107);
IPAddress gateway(10, 0, 0, 1);
IPAddress subnet(255, 255, 0, 0);

// Definições do servidor MQTT
//const char* BROKER_MQTT = "broker.emqx.io";  // broker MQTT 
const char* BROKER_MQTT = "10.0.0.101";        // broker MQTT 
int BROKER_PORT = 1883;
              
// Definições do ID
#define ID_MQTT   "ESP-107"  // ID desta nodeMCU (ID Client)
#define USER      "aluno"
#define PASSWORD  "@luno*123"
#define QOS       1
WiFiClient wifiClient;
PubSubClient MQTT(wifiClient);   // Instancia o Cliente MQTT passando o objeto espClient

// Topicos a serem subescritos
#define SBC_ESP           "MQTTNode"

// Topicos a serem publicados
#define SENSORES_D        "MQTTSBC"
#define SENSORES_A        "MQTTSBC"
#define SENSOR_ANALOG     "MQTTSBC"
#define SENSOR_DIGITAL    "MQTTSBC"
#define STATUS            "MQTTSBC"
#define LED               "MQTTSBC"

// tempo entre as medicoes automaticas
int tempo_medicoes = 60;  // padrao 1 minuto
Timer t;

/**
 * Reconecta-se ao broker Tenta se conectar ao broker constantemente
 */
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT, USER, PASSWORD, SBC_ESP, QOS, false, "1F")){
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(SBC_ESP, 1); 
    } else{
      Serial.println("Falha ao se reconectar no broker!");
      Serial.println("\nTentando se conectar em 2s...\n");
      delay(2000);
    }
  }
}


/**
 * Caso a NodeMCU não esteja conectado ao WiFi, a conexão é restabelecida.
*/
void reconnectWiFi() {
  //se já está conectado a rede WI-FI, nada é feito. 
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;
        
  WiFi.begin(ssid, password); // Conecta na rede WI-FI
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
}


/**
 * Verifica se o cliente está conectado ao broker MQTT e ao WiFi.
 * Em caso de desconexão, a conexão é restabelecida.
*/
void checkMQTTConnection(void) {
  reconnectWiFi();
  if (!MQTT.connected()){
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita
  } 
}


/**
 * Realiza as medições dos sensores
*/
void medicoes(void){
  char digitais[10];
  sprintf(digitais, "%d%d%d%d%d%d%d%d", digitalRead(D0), digitalRead(D1), 
                                        digitalRead(D2), digitalRead(D3), 
                                        digitalRead(D4), digitalRead(D5),
                                        digitalRead(D6), digitalRead(D7));
  MQTT.publish(SENSORES_D, digitais);   // envia atualizacao para o topico dos sensores digitais

  char analogicos[6];
  sprintf(analogicos, "%d", analogRead(A0)); 
  MQTT.publish(SENSORES_A, analogicos); // envia atualizacao para o topico dos sensores analogicos
}


/**
 * Configura a comunicacao com o nodemcu via WIFI
*/
void config_connect(){
  Serial.begin(9600);
  Serial.println("Booting");

  // Configuração do IP fixo no roteador, se não conectado, imprime mensagem de falha
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname(host);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


/**
 * Recebe as mensagens via mqtt 
 * @param topic - Topico que enviou a mensagem
 * @param payload - Mensagem recebida
 * @param length - Tamanho da mensagem
*/
void on_message(char* topic, byte* payload, unsigned int length){
  unsigned char recvd = *payload;
  const char *ptr = null;
  if(length){
    if(recvd == NODE_ID) {
      selectedUnit = true;
      digitalWrite(LED_BUILTIN, HIGH);
      Serial.write(NODE_ID);
    }
    if(recvd == DESELECT_NODE) {
      selectedUnit = false;
      Serial.write(DESELECT_NODE);
      digitalWrite(LED_BUILTIN, LOW);
      delay(2000);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(2000);
    }
    if(selectedUnit && recvd != NODE_ID) {
      switch(recvd) {
        // Acender LED_BUILTIN
        case(0xC0):
            digitalWrite(LED_BUILTIN, LOW);
            break;
        // Consultar D0
        case(0xC3):
            Serial.write(SENSORES_D, digitalRead(D0));
            break;
        // Consultar D1
        case(0xC5):
           MQTT.publish(SENSORES_D, digitalRead(D1));
            break;
        case(0xC1):
            int analogData;
            unsigned char quocient, rest;
            analogData = analogRead(A0);
            quocient = analogData / 10;
            rest = analogData % 10;
            MQTT.publish(SENSORES_A, quocient);
            delay(2);
            MQTT.publish(SENSORES_A, rest);
            break;
      }
    }
  }
}


/**
 * Inicia as configuracoes no momento do upload 
 */
void setup() {
  // realiza a configuracao inicial para conexao via wifi com nodemcu
  config_connect();
  //Serial.begin(9600);

  // definicao dos pinos
  pinMode(LED_BUILTIN, OUTPUT);  

  // inicia a comunicacao mqtt
  MQTT.setServer(BROKER_MQTT, BROKER_PORT); 
  MQTT.setCallback(on_message);

  //t.every(tempo_medicoes * 1000, medicoes); 

  // pisca o led do nodemcu no momento da execucao
  for(int i=0; i<10; i++){
    digitalWrite(LED_BUILTIN,LOW);
    delay(50);
    digitalWrite(LED_BUILTIN,HIGH);
    delay(50);
  }
}

#define NODE_ID 0x1
#define DESELECT_NODE 0x81
bool selectedUnit = false;

void loop() {
  ArduinoOTA.handle();
  checkMQTTConnection();
  MQTT.loop();
  t.update();
}
