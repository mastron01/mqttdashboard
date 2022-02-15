/*************************************************************************
 MQTT DASHBOARD
 Bisa di download di playstore

 To install the ESP8266 board :
  - Add the following 3rd party board manager under "File -> Preferences -> Additional Boards Manager URLs":
       http://arduino.esp8266.com/stable/package_esp8266com_index.json
  - Open the "Tools -> Board -> Board Manager" and click install for the ESP8266"
  - Select your ESP8266 in "Tools -> Board"

jika ingin menambahkan esp32 dan digistump:
https://dl.espressif.com/dl/package_esp32_index.json,
http://arduino.esp8266.com/stable/package_esp8266com_index.json,
http://digistump.com/package_digistump_index.json

--> connect isi topic dengan : myTopic/#  -- Rubah menjadi MyTopic tanpa #

- menjalankan demo  : home_31861/toggle1   , test.mosquitto.org
- menjalankan sw4ch : sub : home_12345/sw1
- subscribe 4ch             home_12345/sw2

******************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define Lamp1      D0 // wemos D1mini biultin led: D4( GPIO 2) & D0 nodemcu , high saat upload
#define Lamp2      D1 //

const char* ssid = "Wifi_Ruang1";       // ganti sesuai wifi yang digunakan
const char* password = "wifi12345";

const char* mqtt_server = "test.mosquitto.org";
const char* mqttUser = "";
const char* mqttPassword = "";
const int   mqttPort = 1883;

char* subscribe1 = "home_12345/sw1";   //subscribe topic ganti sesuai dengan topic yang diinginkan
char* subscribe2 = "home_12345/sw2";   
//char* publishdata = "home_12345";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
int cnt_rec=0;
//---------------------------------------------------- setup wifi -------------------
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

  randomSeed(micros());
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
//-------------------------------------------------------------------- call back -----
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  String sTopic = topic;
  
  // detect nilai sw1
  if (!strcmp(topic,subscribe1)){         // strcmp : jika kedua string sama nilai logikanya = 0
      if ((char)payload[0] == '1') {
        Serial.println("Lampu 1 : ON");
        digitalWrite(Lamp1, HIGH);   // Turn the LED on (Note that LOW is the voltage level
      } else {
        Serial.println("Lampu 1 : OFF");
        digitalWrite(Lamp1, LOW);  // Turn the LED off by making the voltage HIGH
      }
  }

  // detect nilai sw2
  if (sTopic==subscribe2){         // topic dirubah ke string
      if ((char)payload[0] == '1') {
        Serial.println("Lampu 2 : ON");
        digitalWrite(Lamp2, HIGH);
      } else {
        Serial.println("Lampu 2 : OFF");
        digitalWrite(Lamp2, LOW);
      }
  }

}
//--------------------------------------------- reconnect --------------------------------
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    cnt_rec++;
    Serial.print(cnt_rec);Serial.print(" - Attempting MQTT connection...");
    String clientId = "ESP8266SMK-";
    clientId += String(random(0xffff), HEX);       // Create a random client ID

//    if (client.connect(clientId.c_str())) {
     if (client.connect(clientId.c_str(), mqttUser, mqttPassword )) {
      Serial.println("connected");
//      client.publish(pubTopic, "-- Started --- ");        
      client.subscribe(subscribe1);                            // ... and resubscribe
      client.subscribe(subscribe2);
  } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
//============================================ Setup ==============================
void setup() {
  pinMode(Lamp1, OUTPUT);
  pinMode(Lamp2, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);
}
//========================================== loop =================================
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();                  
  long now = millis();
  if (now - lastMsg > 3000) {
    lastMsg = now;
    ++value;
    Serial.print(value); Serial.println(" - Stanby"); 
  }
}
