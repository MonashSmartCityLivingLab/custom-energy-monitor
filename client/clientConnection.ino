#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "scll-1";
const char* password =  "scll-2023#";
String url = "http://192.168.68.101:5001/receive_data";

void setup() {
  Serial.begin(9600);  // Start serial communication
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
    // Check if serial data is available
    if (Serial.available() > 0) {
      // 1. Read the state (1 byte)
      int sensorState = Serial.read();  // Read the first byte (ON/OFF state)
      
      JsonDocument doc;

      // 2. Read the message (until newline or a known terminator)
      String message = "";  // to store any string messages like "State changed"
      while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') break;  // Stop reading the message at a newline
        message += c;  // Accumulate message characters
      }

      // 3. Read the time difference (4 bytes for unsigned long)
      unsigned long timeDifference = 0;
      if (Serial.available() >= 4) {  // Check if there are 4 bytes to read
        byte timeBytes[4];  // Store the 4 bytes for time
        for (int i = 0; i < 4; i++) {
          timeBytes[i] = Serial.read();  // Read each byte
        }
        // Convert bytes to unsigned long
        timeDifference = *((unsigned long*)timeBytes);  // Convert the byte array back to an unsigned long
      }

      // Print the received data for debugging purposes
      // Serial.println("Received sensor state: " + String(sensorState == 1 ? "ON" : "OFF"));
      // Serial.println("Message: " + message);
      // Serial.println("Time difference: " + String(timeDifference));

      // Send the data to your server
      WiFiClient client;
      HTTPClient http;
      String fullUrl = url;

      if (http.begin(client, fullUrl)) {
        http.addHeader("Content-Type", "application/json");

        // Prepare the JSON payload with the received data
        // String jsonPayload = "{\"sensor_state\":\"" + String(sensorState == 1 ? "ON" : "OFF") + "\",";
        // jsonPayload += "\"message\":\"" + message + "\",";
        // jsonPayload += "\"time_difference\":" + String(timeDifference) + "}";
        doc["message"] = message;
        doc["time_difference"] = String(timeDifference);

        // Create a String to hold the JSON data
        String payload;
        serializeJson(doc, payload);

        // Debug: Print the JSON payload to Serial
        Serial.println("Serialized JSON: " + payload);

        // Send the POST request with the serialized JSON data
        int httpCode = http.POST(payload);
        
        // Print the HTTP response code
        Serial.println("HTTP Response Code: " + String(httpCode));

        // Print the server response
        if (httpCode > 0) {
          String response = http.getString();
          Serial.println("Server Response: " + response);
        }

        http.end();  // Close the connection
      } else {
        Serial.println("[HTTP] Unable to connect");
      }
    }
  } else {
    Serial.println("WiFi Disconnected");
  }

  delay(5000);  // Wait 5 seconds before checking again
}
