import random
import time
import json
from paho.mqtt import client as mqtt_client


broker = 'localhost'
port = 1883
topic = "MQTTSBC"
# Generate a Client ID with the publish prefix.
client_id = f'publish-{random.randint(0, 1000)}'
username = 'aluno'
password = '@luno*123'

"""
ORDEM DOS COMANDOS RECEBIDOS:
0xnum_node (SELECIONAR NODE)
0x8num_node (DESELECIONAR NODE)
0xC1
"""


"""
" { \"Node\" : \"0x1\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"962\" }, 
  { \"Node\" : \"0x2\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"387\" }, 
  { \"Node\" : \"0x3\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"1024\" }", 
" { \"Node\" : \"0x1\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"963\" }, 
  { \"Node\" : \"0x2\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"385\" }, 
  { \"Node\" : \"0x3\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"1024\" }", 
"""

# mqtt_payload = '{ \"Node\" : \"0x1\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"962\" }'


# z_um = random.randint(0,1)
# for index in range(1,2):
#     mqtt_payload = '{ \"Node\" : \"0x'+str(index)+'\", \"D0\" : \1\", \"D1\" : \"1\", \"A0\" : \"962\" }'
#     print('mqtt_payload '+str(mqtt_client))

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
    client.username_pw_set(username, password)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client


def payload(codigo_node:int):
    print(json.dumps({"message":f'0x{codigo_node}, 0x8{codigo_node},"0xC3","0xC5","0xC1"'}))
    return json.dumps({"message":f'0x{codigo_node}, 0x8{codigo_node},0xC3,0xC5,0xC1'})

def publish(client):
    while True:
       for index in range(1,3):
            # z_um = random.randint(0,1)
            # mqtt_payload = '{ "Node" : "0x'+str(index)+', "D0" : '+str(random.randint(0,1))+', "D1" : "' +str(random.randint(0,1))+ '"A0" : '+str(random.randint(0,1025))+'}'
            mqtt_payload = {}
            mqtt_payload["Node"] = '0x'+str(index)
            #+str(index)
            mqtt_payload["D0"] = str(random.randint(0,1))
            mqtt_payload["D1"] = str(random.randint(0,1))
            mqtt_payload["A0"] = str(random.randint(3,20))
            # mqtt_payload["A0"] = str(random.randint(0,1025))

            print('mqtt_payload '+str(mqtt_payload))
            time.sleep(1)
            result = client.publish(topic, json.dumps(mqtt_payload))
            status = result[0]
            if status == 0:
                print(f"Send `{payload}` to topic `{topic}`")
            else:
                print(f"Failed to send message to topic {topic}")


def run():
    client = connect_mqtt()
    client.loop_start()
    publish(client)
    client.loop_stop()


if __name__ == '__main__':
    run()


"""
payload_list = ['0x1','0xC3','0xC5','0xC1','0x81','0x2','0xC3','0xC5','0xC1','0x82']
# payload_list = ['0x','0x8','0xC3','0xC5','0xC1']
def publish(client):
    msg_count = 1
    while True:
        for payload in payload_list:
            # payload = msg_code+str(index)
            time.sleep(1)
            # result: [0, 1]
            result = client.publish(topic, payload)
            status = result[0]
            if status == 0:
                print(f"Send `{payload}` to topic `{topic}`")
            else:
                print(f"Failed to send message to topic {topic}")
        msg_count += 1
        #if msg_count > 5:
        #    break
"""
"""
payload_list = ['0x1','0xC3','0xC5','0xC1','0x81','0x2','0xC3','0xC5','0xC1','0x82']
# payload_list = ['0x','0x8','0xC3','0xC5','0xC1']
def publish(client):
    msg_count = 1
    while True:
        for payload in payload_list:
            # payload = msg_code+str(index)
            time.sleep(1)
            # result: [0, 1]
            result = client.publish(topic, payload)
            status = result[0]
            if status == 0:
                print(f"Send `{payload}` to topic `{topic}`")
            else:
                print(f"Failed to send message to topic {topic}")
        msg_count += 1
"""
