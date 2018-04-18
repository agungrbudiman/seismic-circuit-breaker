#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#define relay 12
#define led 13

const char* ssid = "KIARA15A";
const char* password = "";
const char* mqtt_server = "192.168.111.134";
double sum=0;
byte counter;
unsigned long prevHitung;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(9600);
  pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);
  digitalWrite(relay, HIGH);
  digitalWrite(led, LOW);
  
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(millis() - prevHitung > 1000) {
    double acc_avg = sum/counter;
    if(acc_avg > 0.2) {
      Serial.print("Gempa:");
      Serial.println(acc_avg);
      char msg[10];
      sprintf(msg, "Gempa:%f",acc_avg);
      client.publish("notifikasi",msg);
      digitalWrite(relay, LOW);
      digitalWrite(led, HIGH);
    }
    sum=0;counter=0;
    prevHitung = millis();
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
  Serial.print("Received[");
  Serial.print(topic);
  Serial.print("]:");
  if(strcmp(topic,"relay") == 0) {
    char msg = (char)payload[0];
    Serial.println(msg);
    if(msg == '1') {
      digitalWrite(relay, HIGH);
      digitalWrite(led, LOW);
    }
    else {
      digitalWrite(relay, LOW);
      digitalWrite(led, HIGH);
    }
  }
  else {
    char buff[length];
    for (int i = 0; i < length; i++) {
      buff[i] = (char)payload[i];
    }
    double acc = atof(buff);
    Serial.println(acc);
    if(acc < 0) {acc=acc*-1;}
    sum += acc;
    counter++;
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-nodeServer";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.publish("nodeServer", "hello world");
      client.subscribe("sumbux");
      client.subscribe("relay");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
