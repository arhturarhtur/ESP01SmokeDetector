/*
Smoke detector MQTT with ESP01
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SSID-pwd.h>

// Update these with values suitable for your network.

const char* mqtt_server = "192.168.0.143";
const char* mqttTopic = "domoticz/in";
const int idxSmokeDetector = 2945;
int smokeDetector = 0;
volatile int smokeDetectorState = HIGH;
String smokeDetectorOn = "";
char mqttmsgSmokeDetectorOn[128];

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    // Client connexion with lastwill
    // https://pubsubclient.knolleary.net/api.html#connect2
    if (client.connect(clientId.c_str(),  mqttusername, mqttpassword, "outTopic", 0, true, "Disconnected")) {
      client.publish("Debug", "Connected to Mosquitto");
      Serial.println("Connected to Mosquitto");
    }
  }
}

void sendSmokeDetectorOn() {
  if (smokeDetectorState == HIGH) {
    //When smoke detector went on send a MQTT message to change domoticz virtual switch to ON
    smokeDetectorOn = "{\"command\" : \"switchlight\",\"idx\" : " + String(idxSmokeDetector) + ",\"switchcmd\" : \"On\"}";
    smokeDetectorOn.toCharArray(mqttmsgSmokeDetectorOn, smokeDetectorOn.length() + 1);
    client.publish( mqttTopic, mqttmsgSmokeDetectorOn);
    client.publish("Debug", "Alert");
    Serial.print(mqttTopic);
    Serial.println(mqttmsgSmokeDetectorOn);
  }
}

void setup() {
  pinMode(smokeDetector, INPUT);
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  
  // read the input pin:
  smokeDetectorState = digitalRead(smokeDetector);
  // print out the state of the smoke detector:
  Serial.println(smokeDetectorState);
  sendSmokeDetectorOn();
  delay(1000);        // delay in between reads for stability
}
