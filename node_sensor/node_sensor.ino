#include <SparkFun_ADXL345.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "KIARA15A";
const char* password = "";
const char* mqtt_server = "192.168.111.134";

int x,y,z;
double ax,ay,az;
char msg[4];

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg;

ADXL345 adxl = ADXL345();

void setup() {
  Serial.begin(9600); 
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  adxl.powerOn();
  adxl.setRangeSetting(2);
  adxl.setAxisOffset(2, 1, -8);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  adxl.readAccel(&x, &y, &z);
  ax = x*0.0039;
  ay = y*0.0039;
  az = z*0.0039;
  Serial.print(ax);
  Serial.print(", ");
  Serial.print(ay);
  Serial.print(", ");
  Serial.println(az); 

  if (millis() - lastMsg > 200) {
    sprintf (msg, "%f", ax);
    Serial.print("Sent: ");
    Serial.println(msg);
    client.publish("sumbux", msg);
    lastMsg = millis();
  }
}

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Received:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-nodeSensor";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("nodeSensor", "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
