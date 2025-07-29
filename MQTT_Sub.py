import paho.mqtt.client as mqtt
import time
import RPi.GPIO as GPIO
from sense_hat import SenseHat

sense = SenseHat()
#sense.show_message("hello")

GPIO.setmode(GPIO.BCM)
GPIO.setwarnings(False)

led_pin = 18
GPIO.setup(led_pin, GPIO.OUT)

O = (0, 0, 0)  
red = (255, 0, 0)
green = (0, 255, 0)
blue = (0, 0, 255)


# What to do when a message is received
def on_message(client, userdata, message):
    message_str = str(message.payload.decode("utf-8"))
    print("Received message: " + str(message.payload.decode("utf-8")) + " on topic " + message.topic)
    if message_str == "GREEN":
        O = green
    elif message_str == "RED":
        O = red
    elif message_str == "BLUE":
        O = blue
    elif message_str == "OFF":
        O = (0, 0, 0)
    else :
        sense.show_message(message_str)
        O = (0, 0, 0)
    Screen = [
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O,
    O, O, O, O, O, O, O, O
    ]
    sense.set_pixels(Screen)



mqttBroker = "TestBedRpi.local"
client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
client.connect(mqttBroker)

print("Connected to MQTT broker at " + mqttBroker)
client.loop_start()
client.subscribe("LEDControl")
client.on_message = on_message
    
while True :
    client.publish("Temperature", str(sense.get_temperature())) 
    print("Published temperature: " + str(sense.get_temperature()))
    time.sleep(1)
    
time.sleep(600)  # Keep the script running for 5 minutes
print("Stopping MQTT client loop")
client.loop_stop()