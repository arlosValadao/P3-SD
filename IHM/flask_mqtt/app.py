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

dados = {
        "analog": [],
        "digital": [],
        }
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

@mqtt_client.on_message()
def handle_mqtt_message(client, userdata, message):
   data = {} 
   data['topic'] = message.topic
   data['payload'] = message.payload.decode()
   

#    if "0x1" in str(data.get('payload')):
   if 'Analog' in str(data.get('payload')):
        ids['analog'].append(json.loads(data.get('payload'))['Id'])
        dados['analog'].append(json.loads(data.get('payload'))['Analog'])

        if len(dados['analog']) == 11:
            dados['analog'].pop(0)
        print(json.dumps(dados['analog']))
   

#def save_last_10_values():

  # elif 'Analog' not in data.get('payload'):
  #     print('asdkasldkalkd')

@app.route('/publish', methods=['POST'])
def publish_message():
   request_data = request.get_json()
   publish_result = mqtt_client.publish(request_data['topic'], request_data['msg'])
   return jsonify({'code': publish_result[0]})

@app.route('/', methods=['GET','POST'])
def index():
    if request.method == 'GET': 
        print('index rolando')
        print(json.dumps(dados['analog']))
        # return json.dumps(dados['analog'])
        return json.dumps(dados['analog'])
        # return render_template("chartgpt.html", title="OverviewAnalog", data = json.dumps(dados['analog']))


if __name__ == "__main__":
    app.run(host='127.0.0.1', port=8000)
