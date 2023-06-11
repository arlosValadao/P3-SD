from django.http import HttpResponse, JsonResponse
from . import mqtt
# import client as mqtt_client
import json

def index(request):
    return HttpResponse("olalaskdlaksdlska")

def publish_message(request):
    request_data = json.loads(request.body)
    rc, mid = mqtt.client.publish(request_data['topic'], request_data['msg'])
    # mqtt_client.publish(request_data['topic'], request_data['msg'])
    return JsonResponse({'code': rc})