import paho.mqtt.client as mqtt

def on_connect(client, userdata, flags, rc):
	print("Connected with result code: " + str(rc))
	client.publish("my/test", "Hey mate")
	
client = mqtt.Client()
client.on_connect = on_connect
client.username_pw_set("michael", "P@ssw0rd")
client.connect("3.88.162.190", 1883, 60)

client.loop_forever()
