#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Orange"
#define QOS         1
#define TIMEOUT     10000L

char respostaMQTT[100];

volatile MQTTClient_deliveryToken deliveredtoken;


MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;

int mensagemRecebida(void* context, char* topicName, int topicLen, MQTTClient_message* message);


void iniciarMQTT() {
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

    MQTTClient_setCallbacks(client, NULL, NULL, mensagemRecebida, NULL);

    int rc;
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS) {
        printf("Falha na conexão MQTT, Codigo de retorno: %d\n", rc);
        exit(-1);
    }
}

void desconectarMQTT() {
    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
}

int enviarMensagemMQTT(const char* topico, const char* mensagem) {
    pubmsg.payload = (void*)mensagem;
    pubmsg.payloadlen = strlen(mensagem);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;

    MQTTClient_publishMessage(client, topico, &pubmsg, &token);

    int rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
    return rc;
}

int mensagemRecebida(void* context, char* topicName, int topicLen, MQTTClient_message* message) {
    printf("Mensagem Recebida no Tópico: %s\n", topicName);
    //printf("Conteúdo: %.*s\n", message->payloadlen, (char*)message->payload);
    
    // Copiar o valor recebido para a variável respostaMQTT que é Global
    strncpy(respostaMQTT, (char*)message->payload, sizeof(respostaMQTT) - 1);
    respostaMQTT[sizeof(respostaMQTT) - 1] = '\0';
    printf("Valor recebido: %s\n", respostaMQTT);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void escutarTopicoMQTT(const char* topico) {
    MQTTClient_subscribe(client, topico, QOS);
    printf("Subscrito ao tópico: %s\n", topico);
}

int verificarNodeNaRede(char* node){
    enviarMensagemMQTT("node", node);
    delay(1);
    if(respostaMQTT == node){
        return 1;
    }
    return 0;
}


int main(int argc, char* argv[])
{   
    iniciarMQTT();

    escutarTopicoMQTT("respostas_das_nodes");
    //escutarTopicoMQTT("teste1");
    //escutarTopicoMQTT("teste2");

    // Encontrar todas as nodes
    // Mandar msg de node em node e ver se está na rede
    int nodeExiste;
    for(int i = 0; i < 32; i++){
        nodeExiste = verificarNodeNaRede("0x1") //0x1 é só um exemplo
        if(nodeExiste == 1){
            // Node existe
        }else{
            // Node não existe
        }
    }


    
    while (1) {
        // Mantenha o programa em execução para receber mensagens
    }

    int rc;
    rc = enviarMensagemMQTT("Topico_Teste", "Hello Silas");
    
    if(rc == MQTTCLIENT_SUCCESS){
        printf("Mensagem Entregue com Sucesso: %d\n", rc);
    }

    desconectarMQTT();

    return rc;
}