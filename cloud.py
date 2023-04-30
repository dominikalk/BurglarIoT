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
    global data_changed

    if not ard: return
    print ('Topic: ' + msg.topic + '\nMessage: ' + str(msg.payload))
    # Decode JSON request
    data = json.loads(msg.payload)
    # Check request method
    if data['method'] == 'setValue':
        params = data['params']
        if 'enabled' in params:
            if alarm_state['enabled'] != params['enabled']: 
                data_changed = True
            alarm_state['enabled'] = params['enabled']

            print(f"Testing enabled {params['enabled']}")
            ard.write(f"Enabled: {params['enabled']}".encode('utf-8'))
            if not params['enabled']:
                ard.write(f"_Triggered: False".encode('utf-8'))
                alarm_state['triggered'] = False
        if 'triggered' in params:
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
    ard.write("Triggered: False".encode('utf-8'))
    ard.write("Enabled: False".encode('utf-8'))

    while True:
        triggered = alarm_state['triggered']
        line = ard.readline().decode().strip()
        if 'Triggered' in line:
            new_triggered = "True" in line

            if triggered != new_triggered:
                data_changed = True
            
            alarm_state['triggered'] = new_triggered
        
        print (f"Triggered: {alarm_state['triggered']}")
        print (f"Enabled: {alarm_state['enabled']}")

        # Sending humidity, temperature data and buzzer status to ThingsBoard
        if data_changed:
            client.publish('v1/devices/me/telemetry', json.dumps(alarm_state), 1)

        data_changed = False
        next_reading += INTERVAL
        sleep_time = next_reading-time.time()
        if sleep_time > 0:
            time.sleep(sleep_time)

except KeyboardInterrupt:
    client.loop_stop()
    client.disconnect()
    print ("Terminated.")
    os._exit(0)
