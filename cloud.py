import os
import time
import paho.mqtt.client as mqtt
import json
import serial
import serial.tools.list_ports

THINGSBOARD_HOST = 'thingsboard.cs.cf.ac.uk'
ACCESS_TOKEN = 'Xh4fjhD3N8xsnc7QvlVx' # <== Insert your own access token here.

global ard
data_changed = True

# Check if we can successfully push a data to thingsboard
def on_publish(client,userdata,result):
    print("Successful Publish")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    if not ard: return
    print ('Topic: ' + msg.topic + '\nMessage: ' + str(msg.payload))
    # Decode JSON request
    data = json.loads(msg.payload)
    # Check request method
    if data['method'] == 'setValue':
        params = data['params']
        if 'enabled' in params:
            # alarm_state['enabled'] = params['enabled']
            ard.write(f"Enabled: {params['enabled']}".encode('utf-8'))
        if 'triggered' in params:
            # alarm_state['triggered'] = params['triggered']
            ard.write(f"Triggered: {params['triggered']}".encode('utf-8'))
    
# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, rc, *extra_params):
    print('Connected to thingsboard with result code ' + str(rc))

# Data capture and upload interval in seconds. Less interval will eventually hang the DHT11.
INTERVAL = 1
alarm_state = {'enabled': False, 'triggered': False}
next_reading = time.time() 

# Generate your client
client = mqtt.Client()
# Set access token
client.username_pw_set(ACCESS_TOKEN)
# Connect to ThingsBoard using default MQTT port and 60 seconds keepalive interval
client.connect(THINGSBOARD_HOST, 1883, 60)
# Register connect callback
client.on_connect = on_connect
# Register publish callback
client.on_publish = on_publish
# Registed publish message callback
client.on_message = on_message
# subscribe to RPC commands from the server - This will let you to control your buzzer.
client.subscribe('v1/devices/me/rpc/request/+')
client.loop_start()

try:
    while not client.is_connected():
        continue
    ports = serial.tools.list_ports.comports()
    portsList = []
    for onePort in ports:
        portsList.append(str(onePort))
        print(str(onePort))
    val = input("Select Port: COM")
    for x in range(0,len(portsList)):
        if portsList[x].startswith("COM" + str(val)):
            com_port = "COM" + str(val)
            print("----------")

    ard = serial.Serial(com_port, 9600, timeout=0.1)

    while True:
        enabled = False
        triggered = False
        for i in range(2):
            line = ard.readline().decode().strip()
            if i == 0: triggered = "True" in line
            else: enabled = "True" in line
            if line: print(line)
        
        alarm_state['enabled'] = enabled
        alarm_state['triggered'] = triggered

        # Sending humidity, temperature data and buzzer status to ThingsBoard
        client.publish('v1/devices/me/telemetry', json.dumps(alarm_state), 1)

        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)

except KeyboardInterrupt:
    client.loop_stop()
    client.disconnect()
    print ("Terminated.")
    os._exit(0)
