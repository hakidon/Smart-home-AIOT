import pymongo
import paho.mqtt.client as mqtt
from datetime import datetime

# MongoDB configuration
mongo_client = pymongo.MongoClient("mongodb://localhost:27017/")
db = mongo_client["smarthome"]
collection = db["iot"]

# MQTT configuration
mqtt_broker_address = 'x.x.x.x'
mqtt_dht_topic = 'dht'
mqtt_ir_topic = 'ir'

def on_message(client, userdata, message):
    payload = message.payload.decode('utf-8')
    print(f'Received message: {payload}')
    
    # Convert MQTT timestamp to datetime
    timestamp = datetime.utcnow()  # Use current UTC time
    datetime_obj = timestamp.strftime("%Y-%m-%dT%H:%M:%S.%fZ")
    
    # Determine the topic and process the payload accordingly
    if message.topic == mqtt_dht_topic:
        sensor_name = "DHT"
    elif message.topic == mqtt_ir_topic:
        sensor_name = "IR"
    else:
        return  # Ignore messages from unknown topics
    
    # Insert data into MongoDB with proper timestamp
    document = {
        'timestamp': datetime_obj,
        'sensor': sensor_name,
        'data': payload
    }
    collection.insert_one(document)
    print('Data ingested into MongoDB')

client = mqtt.Client()
client.on_message = on_message

# Connect to MQTT broker
client.connect(mqtt_broker_address, 1883, 60)

# Subscribe to MQTT topics
client.subscribe(mqtt_dht_topic)
client.subscribe(mqtt_ir_topic)

# Start the MQTT loop
client.loop_forever()
