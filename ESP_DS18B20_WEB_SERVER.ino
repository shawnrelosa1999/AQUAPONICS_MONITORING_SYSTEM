#ifdef ESP32
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#else
#include <Arduino.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#endif
#include <OneWire.h>
#include <DallasTemperature.h>


// Replace with your network credentials
const char* ssid = "RelosaWifi";
const char* password = "PLDTWIFI47qPv";

// Data wire is plugged into pin 27 on the ESP32
#define ONE_WIRE_BUS 27

// TDS sensor pin
#define TdsSensorPin 35

const float TDScalibrationFactor = 32.5; // Calibration factor for TDS sensor

// pH sensor pin
#define PH_PIN 34

float slope = 0.20;  // change this value to match your calibration curve
float intercept = 2.5;  // change this value to match your calibration curve

OneWire oneWire(ONE_WIRE_BUS); // Create a OneWire instance with the pin number where the data wire is connected
DallasTemperature sensors(&oneWire); // Create a DallasTemperature instance with the OneWire instance

AsyncWebServer server(80); // Create an instance of AsyncWebServer on port 80

void setup() {
  Serial.begin(115200); // Start the serial communication with a baud rate of 115200

  sensors.begin(); // Initialize the DallasTemperature library

  if (!SPIFFS.begin()) { // Mount the SPIFFS file system
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password); // Connect to the Wi-Fi network using the SSID and password
  while (WiFi.status() != WL_CONNECTED) { // Wait for the connection to be established
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP()); // Print the local IP address of the ESP32 to the serial monitor

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle an HTTP GET request to the root URL
    request->send(SPIFFS, "/index.html"); // Send the index.html file from the SPIFFS file system to the client
  });

  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest * request) { 
    sensors.requestTemperatures(); 
    float temperature = sensors.getTempCByIndex(0); 
    request->send_P(200, "text/plain", String(temperature).c_str()); 
  });

  server.on("/tds", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle an HTTP GET request to the /tds URL
    int tdsValue = analogRead(TdsSensorPin); // Read the raw analog value from the TDS sensor pin
    float voltage = tdsValue / 1024.0 * 5.0; // Convert the analog value to voltage
    float tds = voltage * TDScalibrationFactor; // Convert the voltage to a TDS value using a calibration factor
    request->send_P(200, "text/plain", String(tds).c_str()); // Send the TDS value to the client
  });

  server.on("/ph", HTTP_GET, [](AsyncWebServerRequest * request) { // Handle an HTTP GET request to the /ph URL
    int phValue = 0;
    for (int i = 0; i < 10; i++) { // Take 10 readings from the pH sensor and average them
      phValue += analogRead(PH_PIN); // Read the raw analog value from the pH sensor pin
      delay(30);
    }
    phValue /= 10;
    float pHVol = (float)phValue * 5.0 / 1024.0; // Convert the analog value to voltage
    float pH = slope * pHVol + intercept; // Calculate the pH value using a calibration curve
    request->send_P(200, "text/plain", String(pH).c_str()); // Send the pH value to the client
  });

  server.begin(); // Start the server
}

void loop() {
  // This function is empty since we don't need to run any code repeatedly
}
