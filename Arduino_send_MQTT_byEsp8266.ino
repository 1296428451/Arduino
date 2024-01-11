#include <SoftwareSerial.h>
#include <ESP8266WiFi.h> 
#include <SoftwareSerial.h>
#include <PubSubClient.h>

const char* ssid     = "Shinyboy";                // WiFi Name.
const char* password = "password";             // WiFi Password.
SoftwareSerial softSerial(12,13); // RX, TX=D6,D7
WiFiClient wifiClient;

const char *mqttServer = "192.168.1.2"; // MQTT Server Address.
const int MQTT_PROT = 1883;                    // MQTT Server Port.
const char *mqttUserName = "username";
const char *mqttPassword = "password";
PubSubClient mqttClient(wifiClient);

int ledPin = 13;

void setup()
{
    Serial.begin(115200);
    delay(5000);
    ConnWifi();
    softSerial.begin(9600);
    pinMode(ledPin,OUTPUT);

    mqttClient.setServer(mqttServer, 1883);
    connectMQTTServer();
    pubMQTTmsg("Device Started");
}
void loop()
{
  if (!mqttClient.connected()) { // When the IOT connnected the MQTT Service.
    pubMQTTmsg("Device Started");
    mqttClient.loop();
  } 
  String smsg=softSerial.readString();
  // softSerial.println("HELLO");
  softSerial.print("<>");
  softSerial.println(smsg);
  pubMQTTmsg("MSG:"+smsg);
  delay(2000);
}

void ConnWifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);                   // Connecting the Wifi.
  Serial.print("\r\nConnecting to ");           // Output the information to your monitor.
  Serial.print(ssid); Serial.println(" ...");   // Displaying the NodeMCU is connecting the Wifi.
  int i = 0;                                    // WiFi Statue.
  while (WiFi.status() != WL_CONNECTED)          
  {                                             // When the WiFi is connected, which will output WL_CONNECTED.
    delay(1000);    
  }
  Serial.println("WiFi connected!");
  Serial.println(WiFi.localIP());            // Reback the LOCAL IP.
}

void connectMQTTServer(){
  // Seting the WiFi User Name by ESP8266's MAC
  String clientId = "esp8266-" + WiFi.macAddress();
  // Connecting MQTT Server.
  if (mqttClient.connect(clientId.c_str(), mqttUserName, mqttPassword)) {
    Serial.println("MQTT Server Connected.");
    Serial.print("Server Address: ");
    Serial.println(mqttServer);
    Serial.print("ClientId: ");
    Serial.println(clientId);
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }  
}
// Output informations.
void pubMQTTmsg(String msg){
  static int value;
  // Topic Name will due to taichi/Pub-, and the end will link the facility's MAC Address.
  // Sustain the User Name do not to be identity.
  String topicString = "HealthWater/Monitor"; // + WiFi.macAddress();
  char publishTopic[topicString.length() + 1];  
  strcpy(publishTopic, topicString.c_str());
  String messageString = msg;
  char publishMsg[messageString.length() + 1];  
  strcpy(publishMsg, messageString.c_str());
  // ESP8266 send messages to the Server.
  if(mqttClient.publish(publishTopic, publishMsg)){
    Serial.println("Publish Topic:");Serial.println(publishTopic);
    Serial.println("Publish message:");Serial.println(publishMsg);      
  } else {
    Serial.println("Message Publish Failed.");
  }
}
