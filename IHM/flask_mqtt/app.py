from flask import Flask, request, jsonify, render_template
from flask_mqtt import Mqtt
from flask_cors import CORS
import json

app = Flask(__name__)
app.config['MQTT_BROKER_URL'] = 'localhost'
app.config['MQTT_BROKER_PORT'] = 1883
app.config['MQTT_USERNAME'] = 'aluno'  # Set this item when you need to verify username and password
app.config['MQTT_PASSWORD'] = '@luno*123'  # Set this item when you need to verify username and password
app.config['MQTT_KEEPALIVE'] = 20  # Set KeepAlive time in seconds
app.config['MQTT_TLS_ENABLED'] = False  # If your server supports TLS, set it True
topic = 'MQTTSBC'
mqtt_client = Mqtt(app)
CORS(app)  # Enable CORS for all routes
# ids de todas as nodes conectadas
ids = {
        "analog": [],
        "digital": [],
      }

dados = []
        # {
        # "analog": [],
        # "digital": [],
        # }

dados_node1 = {
    "digital0": [],
    "digital1": [],
    "analog0": []
}

dados_node2 = {
    "digital0": [],
    "digital1": [],
    "analog0": []
}

"""
" { \"Node\" : \"0x1\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"962\" }, 
  { \"Node\" : \"0x2\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"387\" }, 
  { \"Node\" : \"0x3\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"1024\" }", 
" { \"Node\" : \"0x1\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"963\" }, 
  { \"Node\" : \"0x2\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"385\" }, 
  { \"Node\" : \"0x3\", \"D0\" : \"1\", \"D1\" : \"1\", \"A0\" : \"1024\" }", 
"""

# list_labels = []

# for i in range(10):
#     list_labels.append({'label': 'Label '+str(i), 'data': 0})

@mqtt_client.on_connect()
def handle_connect(client, userdata, flags, rc):
   if rc == 0:
       print('Connected successfully')
       mqtt_client.subscribe(topic) # subscribe topic
   else:
       print('Bad connection. Code:', rc)

msg_agrupada = []
"""
    index = 0 (D0)
    index = 1 (D1)
    index = 2 (A0)
"""

# msg_payload = ['0x1','0x81','0xC3','0xC5','0xC1','0x2','0x82','0xC3','0xC5','0xC1']
# whatever = []


@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
    valor_d0_node1 = 0
    valor_d1_node1 = 0
    valor_a0_node1 = 0
    
    data = {} 
    data['topic'] = message.topic
    data['payload'] = message.payload.decode()

    dados_mqtt_json = json.loads(message.payload.decode())
    print("dados de mqtt")
    print(json.dumps(dados_mqtt_json))


    # print(message.topic)
    # print(message.payload.decode())
    # print("asdkaslkdkasldklaskdlakl")
    # print(data['payload'])
    # print(data.get('payload'))

    # whatever.append(data['payload'])

    dados.append(str(data.get('payload')))
    print("dados")
    print(dados)

    if len(dados) == 11:
        dados.pop(0)
        print(json.dumps(dados))

    """
        for i in range(10):
            if '0x1' in dados[i] and i <= 6:
                print('inserindo dados da node 1')
                print(i)
                dados_node1.get("digital0").append(int(dados[i+1]))
                dados_node1.get("digital1").append(int(dados[i+2]))
                dados_node1.get("analog0").append(int(dados[i+3]))
                # limita o tamanho das listas
                dados_node1["digital0"] = dados_node1.get("digital0")[:10]
                dados_node1["digital1"] = dados_node1.get("digital1")[:10]
                dados_node1["analog0"]  = dados_node1.get("analog0")[:10]

            if '0x2' in dados[i] and i <= 6:
                print('inserindo dados da node 2')
                print(i)
                dados_node2.get("digital0").append(int(dados[i+1]))
                dados_node2.get("digital1").append(int(dados[i+2]))
                dados_node2.get("analog0").append(int(dados[i+3]))
                #limita o tamanho das listas (slice)
                dados_node2["digital0"] = dados_node2.get("digital0")[:10]
                dados_node2["digital1"] = dados_node2.get("digital1")[:10]
                dados_node2["analog0"]  = dados_node2.get("analog0")[:10]
            
            # if '0x1' or '0x2' not in str(message.payload.decode()):
            #     print(dados['analog'][i])
                
            # print('i = '+str(i))
    
        print("valores node1")
        print(valor_d0_node1, valor_d1_node1, valor_a0_node1)
        print("valores da node 2")
        print(valor_d0_node1, valor_d1_node1, valor_a0_node1)
    """

    # if "0x1" in message.payload.decode():
    #     print("node 1 selecionada")
    #     entra_no_loop = True

    # if (entra_no_loop == True):
    #     for index in range(0,3):
    #         print(f" dentro do if - {message.payload.decode()}")
    #         msg_agrupada.append(message.payload.decode())
    #         print(f"msg agrupada = {msg_agrupada}")
    #         index += 1
    #     entra_no_loop = False
        

    # for index in range(1,3):
    #     print(message.payload.decode())
    #     ids['analog'].append(json.loads(data.get('payload'))['Id'])
    #     dados['analog'].append(json.loads(data.get('payload'))['Analog'])        

    # if  str(data.get('payload')).startswith('0x'):
    #     print(f"recebi dados da node de id {str(data.get('payload'))}")

    # if 'Analog' in str(data.get('payload')):
    #     ids['analog'].append(json.loads(data.get('payload'))['Id'])
    #     dados['analog'].append(json.loads(data.get('payload'))['Analog'])

    #     if len(dados['analog']) == 11:
    #         dados['analog'].pop(0)
        # print(json.dumps(dados['analog']))s

@app.route('/publish', methods=['POST'])
def publish_message():
   request_data = request.get_json()
   publish_result = mqtt_client.publish(request_data['topic'], request_data['msg'])
   return jsonify({'code': publish_result[0]})

@app.route('/', methods=['GET','POST'])
def index():
    if request.method == 'GET': 
        print('index funfando')
        print(json.dumps(dados))
        return json.dumps(dados)
        # print(json.dumps(dados_node1.get("digital0")))
        # return json.dumps(dados_node1.get("digital0"))
        
        # return render_template("chartgpt.html", title="OverviewAnalog", data = json.dumps(dados['analog']))

@app.route('/node1', methods=['GET','POST'])
def node1():
    if request.method == 'GET':
        return json.dumps(dados)


# @app.route('/node2', methods=['GET','POST'])


if __name__ == "__main__":
    app.run(host='127.0.0.1', port=8000)

"""
#    if "0x1" in str(data.get('payload')):
   if 'Analog' in str(data.get('payload')):
        ids['analog'].append(json.loads(data.get('payload'))['Id'])
        dados['analog'].append(json.loads(data.get('payload'))['Analog'])

        if len(dados['analog']) == 11:
            dados['analog'].pop(0)
        print(json.dumps(dados['analog']))
"""