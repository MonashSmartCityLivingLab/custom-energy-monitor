#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "scll-1";
const char* password = "scll-2023#";
String baseUrl = "http://192.168.68.101:5001";
String receiveDataEndpoint = baseUrl + "/receive_data";

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

String createPayload(char* message, String timeDifference) {
  JsonDocument doc;
  doc["message"] = message;
  doc["time_difference"] = timeDifference;
  String payload;
  serializeJson(doc, payload);
  return payload;
}

struct ServerResponse {
  int httpCode;
  bool sendSuccess;
  String message;
};

/**
Input:
  WifiClient client:
  String fullUrl:
      eg: http://example.com, http://192.168.68.101:5001/receive_data
  String payload: 
      This is a json payload which has already been serialised into a string. 
      Allowing custom header may be allowed in the future but for now it has to be a json string

@output
*/
ServerResponse sendDataTo(HTTPClient& http, WiFiClient& client, String fullUrl, String payload, bool verboseLogging = true) {
  ServerResponse serverResponse;

  if (http.begin(client, fullUrl)) {
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(payload);
    serverResponse.httpCode = httpCode;

    if (httpCode > 0) {
      String response = http.getString();
      serverResponse.message = response;
      serverResponse.sendSuccess = true;
    } else {
      serverResponse.message = "tried to send data but some error occurred";
      serverResponse.sendSuccess = false;
    }

    if (verboseLogging) {
      Serial.println("[HTTP RESPONSE CODE]" + String(httpCode));
      Serial.println("[SERVER RESPONSE]" + serverResponse.message);
    }

    http.end();
  } else {
    if (verboseLogging) {
      Serial.println("[CONNEcTION ERROR] Unable to connect");
    }
    serverResponse.message = "error occurred";
    serverResponse.sendSuccess = false;
  }

  return serverResponse;
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }

  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    // Check if serial data is available
    if (Serial.available() > 0) {
      // 1. Read the state (1 byte)
      int sensorState = Serial.read();  // Read the first byte (ON/OFF state)

      // 2. Read the message (until newline or a known terminator)
      String message = "";  // to store any string messages like "State changed"
      while (Serial.available() > 0) {
        char c = Serial.read();
        if (c == '\n') break;  // Stop reading the message at a newline
        message += c;          // Accumulate message characters
      }

      // 3. Decode received string -> split by " " and the item after [SEND] is the time difference
      String timeDifference = getValue(message, ' ', 1);
      String payload = createPayload("sending", timeDifference);

      // Send the data to your server
      WiFiClient client;
      HTTPClient http;

      ServerResponse postRequest = sendDataTo(http, client, receiveDataEndpoint, payload, true);
      if (postRequest.sendSuccess) {
      }
    }

    delay(1000);
  }
}