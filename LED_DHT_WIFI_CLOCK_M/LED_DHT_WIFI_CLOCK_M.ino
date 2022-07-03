// Testing sketch for various DHT humidity/temperature sensors/clock1302
// Written by ladyada, public domain

// REQUIRES the following Arduino libraries:
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library
// - Adafruit Unified Sensor Lib: https://github.com/adafruit/Adafruit_Sensor


//ESP32-38pins

//CONNECTIONS Clock:
//DS1302 CLK/SCLK -->G13
//DS1302 DATA/IO -->G12 
//DS1302 RST/CE -->G14 
//DS1302 VCC --> 3.3V -5V
//DS1302 GND --> GND

//CONNECTIONS DHT Left to right:
//DHT Pin 1 --> G17
//DHT Pin 2 --> +5V
//DHT Pin 3 --> GND

//CONNECtiON LED Status
//WIFI Led status -->G16



#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#define DHTPIN 17     // Digital pin connected to the DHT sensor



#define DHTTYPE DHT11   // Select at the Library DHT 11
#define SensorPin 34  // ESP32 pin analog feacture
// Library to contro the DS1302
#include <ThreeWire.h>
#include <RtcDS1302.h>

ThreeWire myWire(12,13,14);
RtcDS1302<ThreeWire> Rtc(myWire);


//Credentials to conect to the local wifi
const char* ssid = "BelongDC7164";
const char* password = "kymkdr3pqekj";

//Domain name with URL path and IP address with path to send the data
const char* serverName = "http://10.0.0.8/testproject/esp-post-data.php";

//Security feacture API Key value to be compatible with the PHP code have to match both (post data).
String apiKeyValue = "tPmAT5Ab3j7F9";
//Device identification can be change to improve identification in the network
String sensorName = "DHT11";
String sensorLocation = "Office";
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 30 seconds (30000)
unsigned long timerDelay = 30000;
//int Moisture;
#define LED_STA 16 
// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);



void setup() {
  
  //WIFI Status led Pin G16
  pinMode(LED_STA, OUTPUT);
  Serial.begin(115200);
  // Moisture sensor
  Serial.println("Reading Moisture Sensor....");
  delay(2000);
  //Clock
  Serial.print("Compile:");
  Serial.print(__DATE__);
  Serial.print(__TIME__);
  Rtc.Begin();

  RtcDateTime compiled = RtcDateTime(__DATE__,__TIME__);
  printDateTime(compiled);
  Serial.println();


  if (!Rtc.IsDateTimeValid()) 
  {
      // Common Causes:
      //    1) first time you ran and the device wasn't running yet
      //    2) the battery on the device is low or even missing

      Serial.println("RTC lost confidence in the DateTime!");
      Rtc.SetDateTime(compiled);
  }

  if (Rtc.GetIsWriteProtected())
  {
      Serial.println("RTC was write protected, enabling writing now");
      Rtc.SetIsWriteProtected(false);
  }

  if (!Rtc.GetIsRunning())
  {
      Serial.println("RTC was not actively running, starting now");
      Rtc.SetIsRunning(true);
  }

  RtcDateTime now = Rtc.GetDateTime();
  if (now < compiled) 
  {
      Serial.println("RTC is older than compile time!  (Updating DateTime)");
      Rtc.SetDateTime(compiled);
  }
  else if (now > compiled) 
  {
      Serial.println("RTC is newer than compile time. (this is expected)");
  }
  else if (now == compiled) 
  {
      Serial.println("RTC is the same as compile time! (not expected but all is fine)");
  }


  // Setup WIFI  
  Serial.println(F("DHT11 test!"));
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  float sensorValue = analogRead(SensorPin);
  int output_sensor = sensorValue;
  
 
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    //Domain name with URL path or IP address with path it was defined
    http.begin(client, serverName);
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                            + "&location=" + sensorLocation + "&value1=" + String(t)
                            + "&value2=" + String(h) + "&value3=" + String(t) + "";
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



      
    // Wait a few seconds between measurements.
    digitalWrite(LED_STA_WIFI, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_STA_WIFI, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second
  
    // Reading temperature or humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
    //float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    //float t = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    //float f = dht.readTemperature(true);
  
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }
  
    // Compute heat index in Fahrenheit (the default)
    float hif = dht.computeHeatIndex(f, h);
    // Compute heat index in Celsius (isFahreheit = false)
    float hic = dht.computeHeatIndex(t, h, false);
  
    Serial.print(F("Humidity: "));
    Serial.print(h);
    Serial.print(F("%  Temperature: "));
    Serial.print(t);
    Serial.print(F("°C "));
    Serial.print(f);
    Serial.print(F("°F" ))
    Serial.print(F("Moisture: "));
    Serial.print(m);
    Serial.println("%");
    // Heat index: "));
    //Serial.print(hic);
    //Serial.print(F("°C "));
    //Serial.print(hif);
    //Serial.println(F("°F"));

    delay(1000);
    


    RtcDateTime now = Rtc.GetDateTime();

    printDateTime(now);
    Serial.println();

    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }


    delay(8000); // 8 seconds

    
}


#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const RtcDateTime& dt)
{
    char datestring[20];

    snprintf_P(datestring, 
            countof(datestring),
            PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
            dt.Month(),
            dt.Day(),
            dt.Year(),
            dt.Hour(),
            dt.Minute(),
            dt.Second() );
    Serial.print(datestring);
}
