#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>

const char* ssid = "scll-1";
const char* password =  "scll-2023#";
String url = "http://192.168.68.101:5001/receive_data";

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}


void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    
    HTTPClient http;
    String fullUrl = url;
    Serial.println("Requesting " + fullUrl);
    if (http.begin(client, fullUrl)) {
      http.addHeader("Content-Type", "application/json", false, true);
      int httpCode = http.POST("{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}"); //Send the request
      Serial.println(httpCode);  
      http.writeToStream(&Serial);
    } else {
       Serial.printf("[HTTPS] Unable to connect\n");
    }
  }
  delay(5000);
}
