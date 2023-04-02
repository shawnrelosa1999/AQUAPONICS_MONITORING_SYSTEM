#ifdef ESP32
  #include <WiFi.h>
  #include <HTTPClient.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESP8266HTTPClient.h>
  #include <WiFiClient.h>
#endif
#include <Wire.h>
#include <DallasTemperature.h>
#include <OneWire.h>

// Replace with your network credentials
const char* ssid = "RelosaWifi";
const char* password = "PLDTWIFI47qPv";

float slope = 0.20;  // change this value to match your calibration curve
float intercept = 2.5;  // change this value to match your calibration curve

// Data wire is plugged into pin 27 on the ESP32
#define ONE_WIRE_BUS 27

// TDS sensor pin
#define TdsSensorPin 35

const float TDScalibrationFactor = 32.5; // Calibration factor for TDS sensor

// pH sensor pin
#define PH_PIN 34

OneWire oneWire(ONE_WIRE_BUS); // Create a OneWire instance with the pin number where the data wire is connected
DallasTemperature sensors(&oneWire); // Create a DallasTemperature instance with the OneWire instance

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://192.168.1.9/sensordata/post-esp-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page.
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key
String apiKeyValue = "tPmAT5Ab3j7F9";

String sensorName = "Water Quality Sensor";
String sensorLocation = "Aquarium";

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;

    // Your Domain name with URL path or IP address with path
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    int tdsValue = analogRead(TdsSensorPin);
    float tds = (tdsValue / 1024.0) * 50;

    sensors.requestTemperatures(); // Request temperature reading
    float temperature = sensors.getTempCByIndex(0); // Get temperature reading

    int phValue = 0;
    for (int i = 0; i < 10; i++) {
      phValue += analogRead(PH_PIN);
      delay(30);
    }
    phValue /= 10;

    float pHVol = (float)phValue * 5.0 / 1024.0;
    float pH = slope * pHVol + intercept;

     String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName +
                             "&value2=" + String(pH) +
                             "&value1=" + String(temperature) + "&value3=" + String(tds) + "";

        Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
     
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 15 seconds
  delay(1000);  

}
