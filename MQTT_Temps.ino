#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 5

// Setup a oneWire instance to communicate with any OneWire devices 
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

// Update these with values suitable for your network.
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "mqtt_server";
const int mqtt_port = 1883;
const char *mqtt_user = "mqtt_user";
const char *mqtt_pass = "mqtt_pass";
const char *mqtt_client_id = "My-Temps"; //This will be used for your MQTT topics
byte i;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
float temp = 0;
int inPin = 5;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_id, mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
 
void setup()
{
  Serial.begin(115200);
  setup_wifi(); 
  client.setServer(mqtt_server, 1883);
  pinMode(inPin, INPUT);
  sensors.begin();
}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 60000) {
    lastMsg = now;

    Serial.println("Requesting temperatures....................");
    sensors.setResolution(12);
    sensors.requestTemperatures(); // Send the command to get temperatures
    for (i = 0; i < 3; i++) {
      Serial.println(i);
      temp = sensors.getTempFByIndex(i);
      if((temp > -20) && (temp <110)) {
        if(i == 0) {
          client.publish("My-Temps/Device1", String(temp).c_str(),true);
          Serial.println("My-Temps/Device1");
          Serial.println(temp);
        }else if(i == 1){
          client.publish("My-Temps/Device2", String(temp).c_str(),true);
          Serial.println("My-Temps/Device2");
          Serial.println(temp);
        }else if(i == 2){
          client.publish("My-Temps/Device3", String(temp).c_str(),true);
          Serial.println("My-Temps/Device3");
          Serial.println(temp);
        }
      }
      delay(1000);
    }

  }
}
