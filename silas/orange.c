// gcc -Wall -Wextra -o main main.c -lpaho-mqtt3c
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "MQTTClient.h"


#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Orange"
#define QOS         1
#define TIMEOUT     10000L

// #define ADDRESS     "tcp://10.0.0.101:1883@@luno*123"
// #define CLIENTID    "Orange"
// #define QOS         1 
// #define TIMEOUT     10000L
// #define USERNAME    "aluno"
// #define PASSWORD    "@luno*123"

#define MAX_UNITS 32

char respostaMQTT[100] = "0";

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
    // conn_opts.username = USERNAME;
    // conn_opts.password = PASSWORD;

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
    //printf("Mensagem Recebida no Tópico: %s\n", topicName);
    //printf("Conteúdo: %.*s\n", message->payloadlen, (char*)message->payload);
    
    // Copiar o valor recebido para a variável respostaMQTT que é Global
    strncpy(respostaMQTT, (char*)message->payload, sizeof(respostaMQTT) - 1);
    respostaMQTT[sizeof(respostaMQTT) - 1] = '\0';
    //printf("Valor recebido: %s\n", respostaMQTT);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);

    return 1;
}

void escutarTopicoMQTT(const char* topico) {
    MQTTClient_subscribe(client, topico, QOS);
    printf("Subscrito ao tópico: %s\n", topico);
}

void delay(unsigned long ms) {
    struct timespec req = {0};
    time_t sec = (int)(ms / 1000);
    ms = ms - (sec * 1000);
    req.tv_sec = sec;
    req.tv_nsec = ms * 1000000L;
    nanosleep(&req, NULL);
}

int verificarNodeNaRede(char* node){
    printf("Enviar Mensagem para node: %s - ", node);
    enviarMensagemMQTT("comandos_orange", node);
    delay(2);
    printf("Valor recebido: %s ", respostaMQTT);
    if(strcmp(respostaMQTT, node) == 0){
        printf("NODE MQTT ENCONTRADA: \n");
        sprintf(respostaMQTT, "0");
        return 1;
    }
    return 0;
}


int main(int argc, char* argv[])
{   
    char monitoringLabels[3][2] = { {"D0"}, {"D1"}, {"A0"} };
    char* monitoringArrayMQTT[3] = { "0xC3", "0xC5", "0xC1"};

    // Definição de comandos
    char MQTTselectNode[MAX_UNITS][5] = {
                                        "0x1", "0x2", "0x3", "0x4", "0x5", "0x6", "0x7",
                                        "0x8", "0x9", "0xA", "0xB", "0xC", "0xD", "0xE", "0xF" ,"0x10", "0x11",
                                        "0x12", "0x13", "0x14", "0x15", "0x16", "0x17", "0x18", "0x19",
                                        "0x1A", "0x1B", "0x1C", "0x1D", "0x1E", "0x1F", "0x20"
                                    };

    char MQTTdeselectNode[MAX_UNITS][5] = {   
                                        "0x81", "0x82", "0x83", "0x84", "0x85", "0x86", "0x87",
                                        "0x88", "0x89", "0x8A", "0x8B", "0x8C", "0x8D", "0x8E", "0x8F",
                                        "0x90", "0x91", "0x92", "0x93", "0x94", "0x95", "0x96", "0x97",
                                        "0x98", "0x99", "0x9A", "0x9B", "0x9C", "0x9D", "0x9E", "0x9F", "0xA0"
                                    };
    
    char MQTTconsultCommands[8][5] = { "0xC0", "0xC1", "0xC2", "0xC3", "0xC4", "0xC5", "0xC6", "0xC7" };

    int rc;
    //

    iniciarMQTT();

    escutarTopicoMQTT("respostas_das_nodes");
    //escutarTopicoMQTT("teste1");
    //escutarTopicoMQTT("teste2");

    // Encontrar todas as nodes
    // Mandar msg de node em node e ver se está na rede
    int nodeExiste;
    for(int i = 0; i < MAX_UNITS - 31; i++){
        nodeExiste = verificarNodeNaRede(MQTTselectNode[i]); //0x1 é só um exemplo
        if(nodeExiste != 1){
            // Node não existe
            sprintf(MQTTselectNode[i], "0");
            sprintf(MQTTdeselectNode[i], "0");
            printf("Node não encontrada \n");
        }else{
            enviarMensagemMQTT("comandos_orange", MQTTdeselectNode[i]);
            delay(2);
            sprintf(respostaMQTT, "0");
        }
    }

    int contM;
    for (int i = 0; i < MAX_UNITS; i++){
        if(MQTTselectNode[i][0] != '0'){
            strcpy(MQTTselectNode[contM], MQTTselectNode[i]);
            strcpy(MQTTdeselectNode[contM], MQTTdeselectNode[i]);
            contM++;
        }
        // if(vetor_menu01[i][0] != '\0'){
        //     strcpy(vetor_menu01[cont3], vetor_menu01[i]);
        //     cont3++;
        // }
    }


    // for (int i = 0; i < MAX_UNITS; i++) {
    //     printf("MQTTselectNode[%d]: %s\n", i, MQTTselectNode[i]);
    // }
    // printf("\n\n");
    // for (int i = 0; i < MAX_UNITS; i++) {
    //     printf("MQTTselectNode[%d]: %s\n", i, MQTTdeselectNode[i]);
    // }


    rc = enviarMensagemMQTT("comandos_orange", "0x1");
    delay(2);
    sprintf(respostaMQTT, "0");
    
    while (1) {
        // Mantenha o programa em execução para receber mensagens
        printf("Valor Enviado: %s -> ", monitoringArrayMQTT[2]);
        rc = enviarMensagemMQTT("comandos_orange", monitoringArrayMQTT[2]);
        delay(2);
        printf("Valor recebido: %s \n", respostaMQTT);
        sprintf(respostaMQTT, "0");
        delay(1000);
    }





    rc = enviarMensagemMQTT("Topico_Teste", "Hello Silas");
    
    if(rc == MQTTCLIENT_SUCCESS){
        printf("Mensagem Entregue com Sucesso: %d\n", rc);
    }

    desconectarMQTT();

    return rc;
}
