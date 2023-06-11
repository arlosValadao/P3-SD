import paho.mqtt.client as mqtt
from . import settings

def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print("conectado ao broker")
        mqtt_client.subscribe("django/mqtt")
    else:
        print("Erro, nao foi possivel conectar\ncodigo de erro: ", rc)

def on_message(mqtt_client, userdata, msg):
    print(f"Mensagem {msg.payload} recebida no tópico: {msg.topic}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(
        host = settings.MQTT_SERVER,
        port = settings.MQTT_PORT,
        keepalive = settings.MQTT_KEEPALIVE
)

