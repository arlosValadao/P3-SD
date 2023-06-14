#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "Node"
#define QOS         1
#define TIMEOUT     10000L

#define TOPIC       "respostas_das_nodes"
#define PAYLOAD     "0x1"


MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;


void iniciarMQTT() {
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;

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


int main(int argc, char* argv[])
{
    iniciarMQTT();
    int rc;
    rc = enviarMensagemMQTT(TOPIC, PAYLOAD);
    if(rc == MQTTCLIENT_SUCCESS){
        printf("Mensagem Entregue com Sucesso: %d\n", rc);
    }

    desconectarMQTT();

    return rc;
}