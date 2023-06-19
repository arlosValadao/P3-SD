#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <lcd.h>
#include <wiringPi.h>
#include <wiringSerial.h>
#include "MQTTClient.h"
#include "pthread.h"

//USE WIRINGPI PIN NUMBERS
#define LCD_RS  13               //Register select pin
#define LCD_E   18               //Enable Pin
#define LCD_D4  21               //Data pin 4
#define LCD_D5  24               //Data pin 5
#define LCD_D6  26               //Data pin 6
#define LCD_D7  27               //Data pin 7
#define BUTTON_DOWN  25          // PA07
#define BUTTON_UP  23         // PA10
#define BUTTON_ENTER  19         // PA20

#define SIZE_MENU_1 34
#define SIZE_MENU_2 9
#define TURN_BACK (SIZE_MENU_2 - 1)
#define EXIT      availableUnits
#define TWO_SECONDS 2000
#define ANALOG_PIN index == 1
#define MONITORING_ANALOG index == 2
#define MONITORING_ALL (index == (SIZE_MENU_2 - 2))

#define UART_3 "/dev/ttyS3"
#define BAUD_RATE 9600

#define CONSULT     index % 2
#define MONITORING !(CONSULT)
#define MAX_UNITS 16   // Referente ao maximo de unidades de cada Tipo (UART e MQTT, 32 ao todo)

// Configurações do MQTT
#define ADDRESS     "tcp://10.0.0.101:1883@@luno*123"
#define CLIENTID    "Orange"
#define QOS         1 
#define TIMEOUT     10000L
#define USERNAME    "aluno"
#define PASSWORD    "@luno*123"

#define TOPICO "MQTTNode"
#define TOPICO_ESCUTA "MQTTSBC"

#define UNIDADES_MQTT_SELECT      MQTTselectNode
#define UNIDADES_MQTT_DESELECT      MQTTdeselectNode

int qtd = 0;
char respostaMQTT[100] = "0";

char MQTTselectNode[MAX_UNITS][5] = {
                                        "0x1", "0x2", "0x3", "0x4", "0x5", "0x6", "0x7",
                                        "0x8", "0x9", "0xA", "0xB", "0xC", "0xD", "0xE", "0xF" ,"0x10"
                                    };

char MQTTdeselectNode[MAX_UNITS][5] = {   
                                    "0x81", "0x82", "0x83", "0x84", "0x85", "0x86", "0x87",
                                    "0x88", "0x89", "0x8A", "0x8B", "0x8C", "0x8D", "0x8E", "0x8F",
                                    "0x90" 
                                    };


int pausarThread = 0;

volatile MQTTClient_deliveryToken deliveredtoken;

MQTTClient client;
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
MQTTClient_message pubmsg = MQTTClient_message_initializer;
MQTTClient_deliveryToken token;

// Funções MQTT
int mensagemRecebida(void* context, char* topicName, int topicLen, MQTTClient_message* message);

int verificarNodeNaRede(char* node);

void iniciarMQTT() {
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;

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


void* eviarDadosParaInterface(void *arg){
    char varEnviar[200] = "";
    while(1){ 
        delay(5);
        if (pausarThread == 0 ){ 
            sprintf(varEnviar, "");
            for(int i = 0; i < qtd; i++){
                enviarMensagemMQTT(TOPICO, UNIDADES_MQTT_SELECT[i]);
                delay(25);
                sprintf(varEnviar, "%s { \"Node\" : \"%s\",", varEnviar, UNIDADES_MQTT_SELECT[i]);
                
                enviarMensagemMQTT(TOPICO, "0xC3");
                delay(25);
                sprintf(varEnviar, "%s \"D0\" : \"%s\",", varEnviar, respostaMQTT);
                
                enviarMensagemMQTT(TOPICO, "0xC5");
                delay(25);
                sprintf(varEnviar, "%s \"D1\" : \"%s\",", varEnviar, respostaMQTT);
                
                enviarMensagemMQTT(TOPICO, "0xC1");
                delay(25);
                
                if(i == (qtd - 1)){
                    sprintf(varEnviar, "%s \"A0\" : \"%s\" }", varEnviar, respostaMQTT);
                }else{
                    sprintf(varEnviar, "%s \"A0\" : \"%s\" },", varEnviar, respostaMQTT);
                }

                enviarMensagemMQTT(TOPICO, UNIDADES_MQTT_DESELECT[i]);
                delay(25);
            }
            printf("\n%s\n", varEnviar);
            enviarMensagemMQTT("interfaceReceber", varEnviar);
        }  
    }

    return (NULL);
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


int verificarNodeNaRede(char* node){
    printf("Enviar Mensagem para node: %s - ", node);
    enviarMensagemMQTT(TOPICO, node);
    delay(50);
    printf("Valor recebido: %s - ", respostaMQTT);
    if(strcmp(respostaMQTT, node) == 0){
        printf("NODE MQTT ENCONTRADA: \n");
        sprintf(respostaMQTT, "0");
        return 1;
    }
    return 0;
}


// Funções UART
void sendData(int fd, unsigned char* array, unsigned char pos) {
    serialPutchar(fd, array[pos]);
    delay(2);
}

// Implementa o timeout
int recvDigitalData(int fd) {
    if (serialDataAvail(fd) > 0) {
        printf("CONSEGUIU LER\n");
        return serialGetchar(fd); 
    }
    delay(8);
    if (serialDataAvail(fd) > 0) {
        printf("CONSEGUIU LER\n");
        return serialGetchar(fd); 
    }
    printf("NAO CONSEGUIU LER\n");
    return -1;
}

int reachUnit(int fd, char *str, unsigned char *select, unsigned char *deselect, int unitId) {
    int recvData;
    sendData(fd, select, unitId);
    recvData = recvDigitalData(fd);
    if(recvData > -1) {
        sprintf(str, "Select Unit %d", unitId + 1);
        printf("REACH UNIT\n");
        return 1;
    }
    sendData(fd, deselect, unitId);
    return 0;
}

int bytes2int(int fd) {
    if(serialDataAvail(fd) > 0) {
        int quocient = serialGetchar(fd);
        delay(2);
        int remainder = serialGetchar(fd);
        return (quocient * 10) + remainder;
    }
    return -1;
}

int recvAnalogData(int fd) {
    int analogData = bytes2int(fd);
    if(analogData > -1) return analogData;
    delay(8);
    analogData = bytes2int(fd);
    if(analogData > -1) return analogData;
    return analogData; // -1
}


void refreshPos(int* pos, signed short size) {
    if (*pos < 0) *pos = size - 1;
    else if (*pos == size) *pos = 0;
}

void lcdddPuts(int lcdfd, char str[], int time) {
    lcdClear(lcdfd);
    lcdPuts(lcdfd, str);
    delay(time);
}


int main() {
    int indexMenu01 = 0;

    int buttonDownState;
    int buttonUpState;
    int buttonEnterState;

    int idxMonitoring;
    int lcdfd;
    int uartfd;
    // Guarda o indice da node selecionada
    int selectedNode;
    // Menu ativo no momento
    int meun1Active = TRUE;
    int menu2Active = FALSE;

    int availableUnits = 0;

    int choiceMenu1;
    int choiceMenu2;

    // Guarda os dados provenientes da Node
    int recvData = 0;

    // Variavel para controlar o que vai ser mostrado no Menu
    int index = 0;

    // Var para MQTT
    int rc;

    char monitoringLabels[3][10] = { {"D0"}, {"D1"}, {"A0"} };
    char monitoringLabels2[3][10] = { {"A0"}, {"D0"}, {"D1"} };
    unsigned char monitoringArray[] = { 0xC3, 0xC5, 0xC1 };
    unsigned char consultCommands[] = { 0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7 };
    unsigned char selectNode[MAX_UNITS] = {
                                        0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7,
                                        0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF ,0x10
                                    };
    unsigned char deselectNode[MAX_UNITS] = {   
                                        0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
                                        0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
                                        0x90
                                    };
    
    // Definição de comandos MQTT
    char* monitoringArrayMQTT[3] = { "0xC3", "0xC5", "0xC1"};

    char MQTTconsultCommands[8][5] = { "0xC0", "0xC1", "0xC2", "0xC3", "0xC4", "0xC5", "0xC6", "0xC7" };

    // Menus
    char vetor_menu01[SIZE_MENU_1][30];
    char vetor_menu02[SIZE_MENU_2][30] = {
                                            {"Turn on LED"},
                                            {"Query A0"},
                                            {"Monitoring A0"},
                                            {"Query D0"},
                                            {"Monitoring D0"},
                                            {"Query D1"},
                                            {"Monitoring D1"},
                                            {"Monitoring all of them"},
                                            {"Turn Back"}
                                        };

    // Inicializar biblioteca
    if (wiringPiSetup () == -1)
    {
        fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
        return 1;
    }

    // Inicializar MQTT
    iniciarMQTT();
    escutarTopicoMQTT(TOPICO_ESCUTA);

    // Setar botões como entrada
    pinMode(BUTTON_DOWN, INPUT);
    pinMode(BUTTON_UP, INPUT);
    pinMode(BUTTON_ENTER, INPUT);
    

    // Configurar UART
    if ((uartfd = serialOpen (UART_3, BAUD_RATE)) < 0){
        fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
        return 1;
    }

    // Iniciar LCD
    if ((lcdfd = lcdInit (2, 16, 4, LCD_RS, LCD_E, LCD_D4, LCD_D5, LCD_D6, LCD_D7, 0, 0, 0, 0) > 0))
    {
        printf("Unable to init LCD");
        return 1;
    }

    serialFlush(uartfd);

    // DESCOBRINDO UNIDADES ONLINES
    for(int i = 0; i < MAX_UNITS; i++) {
        recvData = reachUnit(uartfd, vetor_menu01[i], selectNode, deselectNode, i);
        if(recvData > 0) {
            availableUnits++;
        }else{
            selectNode[i] = -1;
            deselectNode[i] = -1;
            vetor_menu01[i][0] = '\0';
        }
    }
    int cont = 0;
    int cont2 = 0;
    int cont3 = 0;
    for (int i = 0; i < MAX_UNITS; i++){
        if(selectNode[i] < 255){
            selectNode[cont] = selectNode[i];
            cont++;
        }
        if(deselectNode[i] < 255){
            deselectNode[cont2] = deselectNode[i];
            cont2++;
        }
        if(vetor_menu01[i][0] != '\0'){
            strcpy(vetor_menu01[cont3], vetor_menu01[i]);
            cont3++;
        }
    }

    // Encontrar todas as nodes MQTT
    // Mandar msg de node em node e ver se está na rede
    int nodeExiste;
    int contAumentarMenu1 = cont3;
    for(int i = 0; i < MAX_UNITS; i++){
        nodeExiste = verificarNodeNaRede(MQTTselectNode[i]); //0x1 é só um exemplo
        if(nodeExiste != 1){
            // Node não existe
            sprintf(MQTTselectNode[i], "0");
            sprintf(MQTTdeselectNode[i], "0");
            printf("Node não encontrada \n");
        }else{
            enviarMensagemMQTT(TOPICO, MQTTdeselectNode[i]);
            delay(25);
            sprintf(vetor_menu01[contAumentarMenu1], "Select Unit %d MQTT", i + 1);
            sprintf(respostaMQTT, "0");
            contAumentarMenu1++;
            availableUnits++;      
        }
    }

    qtd = 0;
    for (int i = 0; i < MAX_UNITS; i++){
        if(strcmp(MQTTselectNode[i], "0") != 0){
            strcpy(MQTTselectNode[qtd], MQTTselectNode[i]);
            strcpy(MQTTdeselectNode[qtd], MQTTdeselectNode[i]);
            qtd++;
        }
    }

    sprintf(vetor_menu01[availableUnits], "Monitor All");
    availableUnits++;
    sprintf(vetor_menu01[availableUnits], "Sair");
   
    lcdddPuts(lcdfd, vetor_menu01[index], 0);
    
    pthread_t thread;
    int result = pthread_create(&thread, NULL, eviarDadosParaInterface, NULL);
    if(result == 0){
        printf("Dados sendo mandados para interface. \n");
    }

    while(TRUE){
        //eviarDadosParaInterface(qtd);
        // Verificar se algum botão foi pressionado
        buttonDownState = digitalRead(BUTTON_DOWN);
        buttonUpState = digitalRead(BUTTON_UP);
        buttonEnterState = digitalRead(BUTTON_ENTER);
        // Delay de debounce
        delay(200);
        if (!buttonDownState) {
            // O botão foi pressionado
            index++;
            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (meun1Active) {
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
                refreshPos(&index, availableUnits + 1);
                lcdddPuts(lcdfd, vetor_menu01[index], 0);
            
            }else if (menu2Active) {
                // Se posição passar da quantidade de itens do menu, levar para a posição zero do menu
                refreshPos(&index, SIZE_MENU_2);
                lcdddPuts(lcdfd, vetor_menu02[index], 0);
            }
        }
        else if (!buttonUpState) {
            // O botão foi pressionado
            index--;
            // Verificar se o que vai ser mostrado é o menu 1 ou 2
            if (meun1Active) {
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 32 do menu
                refreshPos(&index, availableUnits + 1);
                lcdddPuts(lcdfd, vetor_menu01[index], 0);
            
            }else if (menu2Active) {
                // Se posição for menor que a quantidade de itens do menu, levar para a posição 6 do menu
                refreshPos(&index, SIZE_MENU_2);
                lcdddPuts(lcdfd, vetor_menu02[index], 0);
            }
        }
        else if (!buttonEnterState) {
            // O botão foi pressionado
            if (meun1Active) {
                // Verificar se apertou enter na posição Sair
                choiceMenu1 = index;
                if (choiceMenu1 == EXIT) {
                    lcdddPuts(lcdfd, ":[", 0);
                    pthread_join(thread, NULL);
                    break;
                }else if (choiceMenu1 == availableUnits - 1){ // Monitorar tudo em todas as nodes
                    pausarThread = 1;
                    delay(500);
                    printf("VOCE DECIDIU MONITORAR TODAS AS UNIDADES\n");
                    while(digitalRead(BUTTON_DOWN)) {
                        idxMonitoring = 0;
                        for (int i = 0; i < availableUnits - 1; i++){
                            if(i < cont3){ // Nodes conectadas a UART
                                printf("\nDADOS DA UART\n");
                                lcdddPuts(lcdfd, "Selecting Unit UART...", TWO_SECONDS);
                                sendData(uartfd, selectNode, i);
                                recvData = recvDigitalData(uartfd);
                                lcdddPuts(lcdfd, "NODE SELECTED", TWO_SECONDS);
                                // Verificar estado dos Pinos
                                idxMonitoring = 0;
                                while(digitalRead(BUTTON_DOWN)) {
                                    serialFlush(uartfd);
                                    lcdClear(lcdfd);
                                    sendData(uartfd, monitoringArray, idxMonitoring);
                                    if(idxMonitoring == 2) recvData = recvAnalogData(uartfd);
                                    else recvData = recvDigitalData(uartfd);
                                    printf("Dado Recebido -> %d\n", recvData);
                                    lcdPrintf(lcdfd, "Value %s: %d", monitoringLabels[idxMonitoring], recvData);
                                    lcdPosition(lcdfd, 0, 1);
                                    lcdPuts(lcdfd, "<ENTER TO EXIT>");
                                    delay(1000);
                                    if (idxMonitoring == 2) break; 
                                    idxMonitoring++;
                                }
                                // Tirar seleção da node
                                sendData(uartfd, deselectNode, i);
                                recvData = recvDigitalData(uartfd);
                                printf("DESELECT RECV DATA -> %d\n", recvData);
                            }else if(i >= cont3){ // Nodes conectadas a MQTT
                                printf("\nDADOS DA MQTT\n");
                                rc = verificarNodeNaRede(MQTTselectNode[i - (cont3)]);
                                lcdddPuts(lcdfd, "Selecting Unit MQTT...", 1000); // 1 segundo de espera
                                if(rc == 1){
                                    printf("NODE SELECIONADA \n");
                                }
                                lcdddPuts(lcdfd, "NODE SELECTED", 1000);
                                // Verificar estado dos Pinos
                                idxMonitoring = 0;
                                while(digitalRead(BUTTON_DOWN)) {
                                    sprintf(respostaMQTT, "0");
                                    lcdClear(lcdfd);
                                    rc = enviarMensagemMQTT(TOPICO, monitoringArrayMQTT[idxMonitoring]);
                                    delay(50);
                                    printf("Dado Recebido -> %s \n", respostaMQTT);
                                    lcdPrintf(lcdfd, "Value %s: %s", monitoringLabels[idxMonitoring], respostaMQTT);
                                    lcdPosition(lcdfd, 0, 1);
                                    lcdPuts(lcdfd, "<ENTER TO EXIT>");
                                    delay(1000);
                                    if (idxMonitoring == 2) break; 
                                    idxMonitoring++;
                                }
                                // Tirar seleção da node
                                enviarMensagemMQTT(TOPICO, MQTTdeselectNode[i - (cont3)]);
                                delay(100);
                                printf("DESELECT RECV DATA -> %s\n", respostaMQTT);
                                sprintf(respostaMQTT, "0");
                            }
                            
                        }
                    }
                    pausarThread = 0;
                    delay(500);
                    while(!digitalRead(BUTTON_DOWN));
                    lcdddPuts(lcdfd, vetor_menu01[index], 0);
                    continue;
                }
                else {
                    if(index < cont3){ // Nodes UART
                        pausarThread = 1;
                        delay(500);
                        lcdddPuts(lcdfd, "Selecting Unit...", TWO_SECONDS);
                        // Seleciona a Node com o ID escolhido
                        sendData(uartfd, selectNode, index);
                        // Salva a index da node selecionada no vetor de ID das Nodes
                        recvData = recvDigitalData(uartfd);
                        if(recvData > -1) {
                            printf("SELECT RECV DATA -> %d\n", recvData);
                            selectedNode = index;
                            // Desabilita menu 1
                            meun1Active = FALSE;
                            // Habilita menu 2
                            menu2Active = TRUE;
                            // Salvar o valor de index do menu 1, para saber no 2
                            // Necessario para saber se foi MQTT ou UART selecionada
                            indexMenu01 = index; 
                            // Reseta o contador de posicoes
                            index = 0;
                            lcdddPuts(lcdfd, "Success on selecting the unit", TWO_SECONDS);
                            lcdddPuts(lcdfd, vetor_menu02[index], 0);
                        }
                        else {
                            lcdddPuts(lcdfd, "Unreachable Unit", TWO_SECONDS);
                            lcdddPuts(lcdfd, vetor_menu01[index], 0);
                        }
                        
                    }else{ // Nodes MQTT
                        pausarThread = 1;
                        delay(500);
                        lcdddPuts(lcdfd, "Selecting Unit...", TWO_SECONDS);
                        // Seleciona a Node com o ID escolhido
                        int testeVerificar;
                        if(cont3 == 0){
                            testeVerificar = verificarNodeNaRede(MQTTselectNode[index]);
                        }
                        else{
                            testeVerificar = verificarNodeNaRede(MQTTselectNode[index - (cont3)]);
                        }
                        if(testeVerificar == 1) {
                            delay(5);
                            printf("SELECT RECV DATA -> %s\n", respostaMQTT);
                            selectedNode = index - (cont3);
                            // Desabilita menu 1
                            meun1Active = FALSE;
                            // Habilita menu 2
                            menu2Active = TRUE;
                            // Salvar o valor de index do menu 1, para saber no 2
                            // Necessario para saber se foi MQTT ou UART selecionada
                            indexMenu01 = index; 
                            // Reseta o contador de posicoes
                            index = 0;
                            lcdddPuts(lcdfd, "Success on selecting the unit", TWO_SECONDS);
                            lcdddPuts(lcdfd, vetor_menu02[index], 0);
                        }
                        else {
                            lcdddPuts(lcdfd, "Unreachable Unit", TWO_SECONDS);
                            lcdddPuts(lcdfd, vetor_menu01[index], 0);
                        }
                    }
                    
                }

            }else if (menu2Active) {
                choiceMenu2 = index;
                // Verificar se apertou enter na posição Voltar
                if (choiceMenu2 == TURN_BACK) {
                    if (indexMenu01 < cont3){
                        // Desseleciona a Node previamente selecionada
                        lcdddPuts(lcdfd, "Deselecting the unit...", TWO_SECONDS);
                        sendData(uartfd, deselectNode, selectedNode);
                        recvData = recvDigitalData(uartfd);
                        printf("DESELECT RECV DATA -> %d\n", recvData);
                        pausarThread = 0;
                        delay(500);
                        meun1Active = TRUE;
                        menu2Active = FALSE;
                        index = choiceMenu1;
                        lcdddPuts(lcdfd, "Unit successfully deselected", TWO_SECONDS);
                        lcdddPuts(lcdfd, vetor_menu01[index], 0);
                    }else{
                        // Desseleciona a Node previamente selecionada
                        lcdddPuts(lcdfd, "Deselecting the unit...", TWO_SECONDS);
                        enviarMensagemMQTT(TOPICO, MQTTdeselectNode[selectedNode]);
                        delay(50);
                        printf("DESELECT RECV DATA -> %s\n", respostaMQTT);
                        pausarThread = 0;
                        delay(500);
                        meun1Active = TRUE;
                        menu2Active = FALSE;
                        index = choiceMenu1;
                        lcdddPuts(lcdfd, "Unit successfully deselected", TWO_SECONDS);
                        lcdddPuts(lcdfd, vetor_menu01[index], 0);
                    }
                    

                }else{
                     if(MONITORING_ALL) {
                         idxMonitoring = 0;
                         printf("VOCE DECIDIU MONITORAR TODOS OS SENSORES\n");
                         while(digitalRead(BUTTON_DOWN)) {
                            lcdClear(lcdfd);
                            if (indexMenu01 < cont3){
                                sendData(uartfd, monitoringArray, idxMonitoring);
                                if(idxMonitoring == 2) recvData = recvAnalogData(uartfd);
                                else recvData = recvDigitalData(uartfd);
                                printf("Dado Recebido -> %d\n", recvData);
                                idxMonitoring == 2 ? (idxMonitoring = 0) : idxMonitoring++; 
                                lcdPrintf(lcdfd, "Value %s: %d", monitoringLabels2[idxMonitoring], recvData);
                                
                            }else{
                                sprintf(respostaMQTT, "0");
                                rc = enviarMensagemMQTT(TOPICO, monitoringArrayMQTT[idxMonitoring]);
                                delay(100);
                                printf("Dado Recebido -> %s\n", respostaMQTT);
                                idxMonitoring == 2 ? (idxMonitoring = 0) : idxMonitoring++;
                                lcdPrintf(lcdfd, "Value %s: %s", monitoringLabels2[idxMonitoring], respostaMQTT);
                            }
                            lcdPosition(lcdfd, 0, 1);
                            lcdPuts(lcdfd, "<ENTER TO EXIT>");
                            delay(1000);
                         }
                         while(!digitalRead(BUTTON_DOWN));
                         lcdddPuts(lcdfd, vetor_menu02[index], 0);
                    }
                   if(index == 0) {
                        lcdddPuts(lcdfd, "Turning on LED", TWO_SECONDS);
                        if (indexMenu01 < cont3){
                            sendData(uartfd, consultCommands, index);
                        }
                        else{
                            rc = enviarMensagemMQTT(TOPICO, MQTTconsultCommands[index]);
                            delay(100);
                        }
                        // Verificacao da resposta da UNIDADE
                        continue;
                    }
                    // Opcao de monitorar sensor
                   if(MONITORING) {
                        printf("VOCE ESCOLHEU MONITORAR\n");
                        while(digitalRead(BUTTON_DOWN)) {
                            lcdClear(lcdfd);
                            if (indexMenu01 < cont3){
                                sendData(uartfd, consultCommands, index - 1);
                                if(MONITORING_ANALOG) recvData = recvAnalogData(uartfd);
                                else recvData = recvDigitalData(uartfd);
                                lcdPrintf(lcdfd, "Value: %d", recvData);
                            }
                            else{
                                sprintf(respostaMQTT, "0");
                                rc = enviarMensagemMQTT(TOPICO, MQTTconsultCommands[index - 1]);
                                delay(100);
                                lcdPrintf(lcdfd, "Value: %s", respostaMQTT);
                            }
                            lcdPosition(lcdfd, 0, 1);
                            lcdPuts(lcdfd, "<ENTER TO EXIT>");
                            delay(500);
                        }
                        while(!digitalRead(BUTTON_DOWN));
                        lcdddPuts(lcdfd, vetor_menu02[index], 0);
                    }
                    else if(CONSULT) {
                        // Mandar mensagem para a node e pegar o dado para exibir no LCD
                        lcdddPuts(lcdfd, "Sending Command...", TWO_SECONDS);
                        // Enviando comando a Node selecionada
                        // Logica para implementar a consulta ou monitoramento
                        if (indexMenu01 < cont3){
                            sendData(uartfd, consultCommands, index);
                            lcdddPuts(lcdfd, "Successfully Sent", TWO_SECONDS);
                            if(ANALOG_PIN) { 
                                // LOGICA DO SENSOR ANALOGICO
                                recvData = recvAnalogData(uartfd);
                            }
                            else recvData = recvDigitalData(uartfd);
                            if(recvData > -1) {
                                lcdClear(lcdfd);
                                lcdPrintf(lcdfd, "Sensor Val.: %d", recvData);
                                delay(TWO_SECONDS);
                            }
                            else { 
                                lcdddPuts(lcdfd, "Unreachable Unit", TWO_SECONDS);
                            }
                        }else{
                            sprintf(respostaMQTT, "0");
                            rc = enviarMensagemMQTT(TOPICO, MQTTconsultCommands[index]);
                            delay(100);
                            lcdddPuts(lcdfd, "Successfully Sent", 1000);
                            lcdClear(lcdfd);
                            lcdPrintf(lcdfd, "Sensor Val.: %s", respostaMQTT);
                            delay(TWO_SECONDS);
                        }
                        lcdddPuts(lcdfd, vetor_menu02[index], 0);
                    }
                }   
            }
        }
    }
    return EXIT_SUCCESS;
}
