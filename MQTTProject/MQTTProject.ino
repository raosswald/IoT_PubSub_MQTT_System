#include "TFT_eSPI.h"
#include "rpcWiFi.h"
#include <PubSubClient.h>

// Declare Global Variables
TFT_eSPI tft;
const char* ssid = "REPLACESSID";
const char* password = "REPLACE PASSWORD";
const int tempY = 100;

// MQTT Variables
const char *ID = "Wio-Terminal-Client";
const char *TOPIC = "LEDControl";
const char *subTopic = "Temperature";
const char *server = "TestBedRpi";


String LastMssg = "";


WiFiClient wifiClient;
PubSubClient client(wifiClient);

void bgRefresh() {
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_RED);
  tft.setTextSize(2);
  tft.drawString("Red", 5, 0);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Green", 60, 0);
  tft.setTextColor(TFT_BLUE);
  tft.drawString("Blue", 150, 0);
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Set last message in white to clear screen on Message Received
  tft.setTextColor(TFT_WHITE);
  tft.drawString(LastMssg, 5, tempY);

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String mssg = "";
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
    mssg += (char)payload[i];
  }
  Serial.println(mssg);
  tft.setTextColor(TFT_BLACK);
  tft.drawString(mssg, 5, tempY);
  LastMssg = mssg;

}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(ID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish(TOPIC, "WT Conn");
      Serial.println("Published connection message successfully!");
      // ... and resubscribe
      client.subscribe(subTopic);
      Serial.print("Subcribed to: ");
      Serial.println(subTopic);
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  while(!Serial);
  delay(1000);

  //Setup Wifi
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.println("Connecting to Wifi...");
    WiFi.begin(ssid, password);
  }
  Serial.println("Connected to WiFi network");
  Serial.print("IP Address: ");
  Serial.println(ssid);

  //MQTT Setup
  client.setServer(server, 1883);
  client.setCallback(callback);

  // SetupLCD Screen
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);

  tft.setTextColor(TFT_RED);
  tft.setTextSize(2);
  tft.drawString("Red", 5, 2);
  tft.setTextColor(TFT_GREEN);
  tft.drawString("Green", 60, 2);
  tft.setTextColor(TFT_BLUE);
  tft.drawString("Blue", 150, 2);
  tft.setTextColor(TFT_BLACK);
  tft.drawString("Temperature in Celsius:", 5, 60);

  // Setup Buttons
  pinMode(WIO_KEY_A, INPUT_PULLUP); //Rightmost button
  pinMode(WIO_KEY_B, INPUT_PULLUP);
  pinMode(WIO_KEY_C, INPUT_PULLUP); //Leftmost button
}

void loop() {
  // put your main code here, to run repeatedly:
  if(!client.connected()) {
    reconnect();
  }
  if (digitalRead(WIO_KEY_A) == LOW) {
    Serial.println("A Key pressed");
    client.publish(TOPIC, "BLUE");
  }
  else if (digitalRead(WIO_KEY_B) == LOW) {
    Serial.println("B Key pressed");
    client.publish(TOPIC, "GREEN");
  }
  else if (digitalRead(WIO_KEY_C) == LOW) {
    Serial.println("C Key pressed");
    client.publish(TOPIC, "RED");
  }
  delay(200);

  client.loop();
}
