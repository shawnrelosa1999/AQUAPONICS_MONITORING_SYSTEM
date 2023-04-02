 

#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 27

// TDS sensor pin
#define TDS_PIN 35

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

//pH calibration
float calibration = 2.60;  //change this value to calibrate
const int analogInPin = 34;
int sensorValue = 0;
unsigned long int avgValue;
float b;
int buf[10], temp;

void setup() {
  // start the serial communication
  Serial.begin(9600);

  // Start up the library
  sensors.begin();
}

void loop() {
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  sensors.requestTemperatures();

  //Read TDS sensor
  int tdsValue = analogRead(TDS_PIN);
  float tds = (tdsValue / 1024.0) * 500;

  // read temperature
  float temperature = sensors.getTempCByIndex(0);

  // read pH sensor
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(analogInPin);
    delay(30);
  }
  for (int i = 0; i < 9; i++) {
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)
    avgValue += buf[i];
  float pHVol = (float)avgValue * 3.3 / 1023;
  float phValue = -5.70 * pHVol + calibration;

  // print the temperature in Celsius
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" C, ");

  // print the TDS value
  Serial.print("TDS: ");
  Serial.print(tds);
  Serial.println(" ppm");

  //print pH level in voltage
  Serial.print("pH Level = ");
  Serial.println(phValue);





  delay(1000);
}
