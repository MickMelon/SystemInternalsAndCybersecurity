import paho.mqtt.client as mqtt
import datetime

def on_connect(client, userdata, flags, rc):
	print("Connected with result code: " + str(rc))
	client.subscribe("my/test")
	
def on_message(client, userdata, msg):
	print("================")
	print(datetime.datetime.now())
	print("Topic: " + msg.topic)
	print("Payload: " + str(msg.payload))
	print("Qos: " + str(msg.qos))
	print("Retain: " + str(msg.retain))
	
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.username_pw_set("michael", "P@ssw0rd")
client.connect("3.88.162.190", 1883, 60)

client.loop_forever()
