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
#include <ESPAsyncWebServer.h>

const char* ssid = "RelosaWifi";
const char* password = "PLDTWIFI47qPv";

float slope = 0.20;
float intercept = 2.5;

#define ONE_WIRE_BUS 27
#define TdsSensorPin 35

const float TDScalibrationFactor = 32.5;

#define PH_PIN 34

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);


const char* serverName = "http://192.168.1.9/sensordata/post-esp-data.php";
String apiKeyValue = "tPmAT5Ab3j7F9";
String sensorName = "Water Quality Sensor";
String sensorLocation = "Aquarium";

AsyncWebServer server(80);

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

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request){
    int params = request->params();
    String sensorValue1, sensorValue2, sensorValue3;

    for(int i = 0; i < params; i++){
      AsyncWebParameter* p = request->getParam(i);
      if(p->name() == "value1"){
        sensorValue1 = p->value();
      }
      else if(p->name() == "value2"){
        sensorValue2 = p->value();
      }
      else if(p->name() == "value3"){
        sensorValue3 = p->value();
      }
    }

    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName +
                             "&value1=" + sensorValue1 + "&value2=" + sensorValue2 + "&value3=" + sensorValue3;

    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);

    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }

    http.end();
    request->send(200);
  });
}

void loop() {
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
    http.begin(serverName);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");


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

    // Construct HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName +
                             "&value2=" + String(pH) +
                             "&value1=" + String(temperature) + "&value3=" + String(tds) + "";

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    // Check for successful HTTP response
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
  delay(15000);  
}
