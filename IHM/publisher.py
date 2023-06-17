import random
import time
import json
from paho.mqtt import client as mqtt_client


broker = 'localhost'
port = 1883
topic = "MQTTSBC"
# Generate a Client ID with the publish prefix.
client_id = f'publish-{random.randint(0, 1000)}'
# username = 'emqx'
# password = 'public'

"""
ORDEM DOS COMANDOS RECEBIDOS:
0xnum_node (SELECIONAR NODE)
0x8num_node (DESELECIONAR NODE)
0xC1
"""
message_list = []
for codigo_node in range(1,10):
    message_list.append(["0x"+str(codigo_node),"0x8"+str(codigo_node),"0xC3","0xC5","0xC1"])

print(message_list)

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    # client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def payload(codigo_node:int):
    print(json.dumps({"message":f'0x{codigo_node}, 0x8{codigo_node},"0xC3","0xC5","0xC1"'}))
    return json.dumps({"message":f'0x{codigo_node}, 0x8{codigo_node},0xC3,0xC5,0xC1'})

def publish(client):
    msg_count = 1
    while True:
        time.sleep(1)
        msg =  json.dumps({
                "Id": str(client_id),
                "Analog": str(msg_count)
                })
        # for message in message_list:
            # for index in range((len(message))):
        # msg = message[index]
        result = client.publish(topic, msg)
        # result: [0, 1]
        status = result[0]
        if status == 0:
            print(f"Send `{msg}` to topic `{topic}`")
        else:
            print(f"Failed to send message to topic {topic}")
        msg_count += 1
        #if msg_count > 5:
        #    break


def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)
    client.loop_stop()


if __name__ == '__main__':
    run()

