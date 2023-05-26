#include <stdio.h>
#include <mosquitto.h>

#define PASSWORD "@luno*123"
#define LOGIN    "aluno"

#define TOPIC "p3-test/t"
#define PUB_ID "P3-teste"

#define BROKER_IP "10.0.0.101"
#define BROKER_PORT 1883

int main() {
    int rc;
    struct mosquitto* mosq;
    mosquitto_lib_init();
    mosq = mosquitto_new(PUB_ID, true, NULL);
    mosquitto_username_pw_set(mosq, LOGIN, PASSWORD);
    rc = mosquitto_connect(mosq, BROKER_IP, BROKER_PORT, 20);
    printf("rc code -> %d\n", rc);
                                            // mensage
    mosquitto_publish(mosq, NULL, TOPIC, 4, "P3SD", 0, false);
    mosquitto_disconnect(mosq);
    mosquitto_destroy(mosq);
    mosquitto_lib_cleanup();
    return 0;
}
