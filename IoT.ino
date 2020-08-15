#include <Servo.h>

#include "FirebaseESP8266.h"
#include <ESP8266WiFi.h>
#include "DHT.h"

#define FIREBASE_HOST "iotroom-a30b0.firebaseio.com" 
#define FIREBASE_AUTH "hTRVpGdBX2ltOYoUsO7Vcg9pWK7nDM5QAmmyYUxX"
#define WIFI_SSID "MasterBulb"
#define WIFI_PASSWORD "celab123"

#define DHTPIN D6
#define DHTTYPE DHT11

DHT dht(DHTPIN,DHTTYPE);
Servo servo;
Servo servo1;

FirebaseData firebaseData;

String Path = "/sensors/12345678/readsensors/-MB_8zjOPpD-kf9FWzit";
String temp = "";
String humidity = "";
String lamp = "";
String motorCondition = "";
int tempInt = 0;
int counter = 0;

bool send = false;

void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("gateway:");
  Serial.print(gateway);
  Serial.println(" ");
}



void setup() {
  Serial.begin(9600);
  dht.begin();

  servo.attach(D4);
  servo1.attach(D5);
  
  WiFi.mode(WIFI_STA);                      // Mode Station
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);     // Mencocokan SSID dan Password
  Serial.print("");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.println(".");
    delay(500);
  }
  Serial.println();
  printWifiStatus();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);

  //Set database read timeout to 1 minute (max 15 minutes)
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  //tiny, small, medium, large and unlimited.
  //Size and its write timeout e.g. tiny (1s), small (10s), medium (30s) and large (60s).
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

  Serial.println("------------------------------------");
  servo.write(160);
  servo1.write(0);

  
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  String dataHumidity = String(h);
  String dataTemperature = String(t);
  
  Serial.print("Humidity: ");
  Serial.println(dataHumidity);
  Serial.print("Temperature: ");
  Serial.println(dataTemperature);
  

  
  if (Firebase.getString(firebaseData, Path + "/value"))
  {
    if (firebaseData.dataType() == "string")
    {
      temp = firebaseData.stringData();
      Serial.print("Ini yang didapat SUHU : ");
      Serial.println(temp);
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  
  if (Firebase.getString(firebaseData, Path + "/humidity"))
  {
    if (firebaseData.dataType() == "string")
    {
      humidity = firebaseData.stringData();
      Serial.print("Ini yang didapat KELEMBAPAN: ");
      Serial.println(humidity);
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  if (Firebase.getString(firebaseData, Path + "/LDR"))
  {
    if (firebaseData.dataType() == "string")
    {
      lamp = firebaseData.stringData();
      Serial.print("Ini yang didapat LAMPU : ");
      Serial.println(lamp);
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }

  if (Firebase.getString(firebaseData, Path + "/motor"))
  {
    if (firebaseData.dataType() == "string")
    {
      motorCondition = firebaseData.stringData();
      Serial.print("Ini yang didapat kondisi servo : ");
      Serial.println(motorCondition);
    }
  }
  else
  {
    Serial.println("FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }


  
  int value;
  if(motorCondition == "ON")
  {
    Serial.println("DATA MOTOR CONDITION IS ON");
    servo.write(0);
    delay(1000);
    servo.write(160);
    
  }
  if(motorCondition == "OFF")
  {
    
    Serial.println("DATA MOTOR CONDITION IS OFF");
    servo1.write(160);
    delay(1000);
    servo1.write(0);
  }
  value = analogRead(A0);
  Serial.print("Data sensor LDR: ");
  Serial.println(value);
  Firebase.setString(firebaseData, Path + "/value",dataTemperature);
  if(value > 500)
  {
    Firebase.setString(firebaseData, Path + "/LDR","ON");
  }
  else
  {
    Firebase.setString(firebaseData, Path + "/LDR","OFF");
  }
  
  delay(1000);
  
}
