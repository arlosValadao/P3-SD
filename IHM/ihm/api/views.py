from dmqtt.signals import connect, regex, topic
from django.dispatch import receiver

@receiver(connect)
def on_connect(sender, **kwargs):
    sender.subscribe("#")

@topic("django/mqtt")
def simple_topic(sender, topic, data, **kwargs):
    print(topic)
    print(data)

"""
from django.http import HttpResponse, JsonResponse
import paho.mqtt.client as mqtt
import json

MQTT_SERVER = "localhost"
MQTT_PORT = 1883
MQTT_KEEPALIVE = 60
#MQTT_USER = "aluno"
#MQTT_PASSWORD = "@luno*123"

def on_connect(mqtt_client, userdata, flags, rc):
    if rc == 0:
        print("Conectado")
        mqtt_client.subscribe("django/mqtt")
    else:
        print("Conexao ruim, codigo de erro:", rc)

def on_message(mqtt_client, userdata, msg):
    print(f"Mensagem {msg.payload} recebida no topico: {msg.topic}")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(
        host = settings.MQTT_SERVER,
        port = settings.MQTT_PORT,
        keepalive = settings.MQTT_KEEPALIVE
)

def publish_message(request):
    request_data = json.loads(request.body)
    rc, mid = client.publish(request_data['topic'], request_data['msg'])
    return JsonResponse({'code': rc})
"""

def index(request):
    return HttpResponse("OlaZZ!")
