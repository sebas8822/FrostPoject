

//ESP32-38pins

//CONECTIONS Display
//OLED GND --> GND
//OLED VCC --> 3.3V
//OLED SCL -->G22
//OLED SDA -->G21

//CONNECTIONS Clock:
//DS1302 CLK/SCLK -->G13
//DS1302 DATA/IO -->G12
//DS1302 RST/CE -->G14
//DS1302 VCC --> 3.3V -5V
//DS1302 GND --> GND

//CONNECTION MICROSD
//SD 3V3  --> 3.3V
//SD CS   --> G5
//SD MOSI --> G23
//SD CLK  --> G18
//SD MISO --> G19
//SD GND  --> GND

//CONNECTIONS DHT Left to right:
//DHT Pin 1 --> G17
//DHT Pin 2 --> +5V
//DHT Pin 3 --> GND

//CONNECTION SOIL MOISTURE SENSORE
//SMS GND --> GND
//SMS VCC --> 3.3V
//SMS AO  --> G34

//CONNECtiON LED Status WIFI
//WIFI Led status -->G16

//CONNECtiON LED SD DATA TRASFER
//WIFI Led status -->G27


//LIBRARIES
#include "DHT.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ThreeWire.h> // Library to control the DS1302 Clock
#include <RtcDS1302.h> // Library to control the DS1302 Clock
#include <Adafruit_GFX.h>// Control Display
#include <Adafruit_SSD1306.h>// Control Display
#include "FS.h" // Control SD
#include "SD.h" // Control SD
#include "SPI.h"// Control SD


//PIN DEFINITION VARIABLES
#define DHTPIN 17     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // Select at the Library DHT 11
#define MOISTUREPin 34  // ESP32 pin analog feacture Moisture 
#define LED_STA_WIFI 16 // STATUS LED WIFI CONNECTION AND DATABASE RESPOND
#define LED_STA_SD 27 // STATUS LED WIFI CONNECTION AND DATABASE RESPOND
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//CONSTANTS AND CREDENCIALS

//Credentials to conect to the local wifi
const char* ssid = "BelongDC7164";
const char* password = "kymkdr3pqekj";

//Domain name with URL path and IP address with path to send the data
const char* serverName = "http://10.0.0.8/testproject/esp-post-data.php";

//Security feacture API Key value to be compatible with the PHP code have to match both (SEND data).
String apiKeyValue = "tPmAT5Ab3j7F9";

//Device identification can be change to improve identification in the network
String sensorName = "DHT11";
String sensorLocation = "Office";
//unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 30 seconds (30000)
//unsigned long timerDelay = 30000;


// Initialize DHT sensor.
DHT dht(DHTPIN, DHTTYPE);
// Initialize RTC
ThreeWire myWire(12, 13, 14);
RtcDS1302<ThreeWire> Rtc(myWire);
//Initialize Display
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


void setup() {
  Serial.begin(115200);

  initWiFi();
  iniSD();
  iniDisplay();
  iniClock();

  //WIFI Status led Pin G16 SETUP OUTPUT
  pinMode(LED_STA_WIFI, OUTPUT);

  //SD Status led Pin G27 SETUP OUTPUT
  pinMode(LED_STA_SD, OUTPUT);
  dht.begin();
  
  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");// opoen file 
  if(!file) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    //writeFile(SD, "/data.txt", "Epoch Time, Temperature, Humidity, Pressure \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();// close file

  // If the backup.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file2 = SD.open("/backup.txt");// opoen file 
  if(!file2) {
    Serial.println("File doesn't exist");
    Serial.println("Creating file...");
    //writeFile(SD, "/backup.txt", "Epoch Time, Temperature, Humidity, Pressure \r\n");
  }
  else {
    Serial.println("File already exists");  
  }
  file2.close();// close file








  
}

void loop() {
  // Read humity
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Read moisture analog entry requires calibration!!!
  int m = analogRead(MOISTUREPin);

  //La Unidades Calor (UC) son la forma como se mide la acumulación
  //del calor a través del tiempo. Son un indicador del tiempo que un
  //cultivo ha estado expuesto a temperaturas adecuadas para su desarrollo.
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  String dataMessage;
  
  RtcDateTime now = Rtc.GetDateTime();

  /*printDateTime(now);
    Serial.println();
    delay(1000);*/


  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F "));
  Serial.print(F(" Moisture: "));
  Serial.print(m);
  Serial.print("%");
  Serial.print(F(" Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.print(F("°F "));

  display.setCursor(50, 0);
  // Display static text
  display.println("AgroMac");
  display.println("");


  // Display static text
  display.print("H: ");
  display.print(h);
  display.println("%");
  display.print("T: ");
  display.print(t);
  display.print(" C ");
  display.print(f);
  display.println(" F ");
  display.print("M: ");
  display.print(m);
  display.println("%");
  display.print("HI: ");
  display.print(hic);
  display.print(" C ");
  display.print(hif);
  display.println("F ");
  display.println("");
  display.println(printDateTime(now));
  display.display();
  delay(1000);





















  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    //Domain name with URL path or IP address with path it was defined
    http.begin(client, serverName);
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor=" + sensorName
                             + "&location=" + sensorLocation + "&value1=" + String(h)
                             + "&value2=" + String(t) + "&value3=" + String(m) + "&value4=" + String(hic) + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData); // show what is going to be send

    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);
    // Wait a few seconds between measurements.
    digitalWrite(LED_STA_WIFI, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(LED_STA_WIFI, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      




      
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      storageDATA(h ,t, m, hic,printDateTime(now));
    }
    // Free resources
    http.end();
  }// post a line in the data base and start again
  else {
    Serial.println("WiFi Disconnected");// introduce SD code
  }

  /*
    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t) || isnan(f)) {
      Serial.println(F("Failed to read from DHT sensor!"));
      return;
    }




    if (!now.IsValid())
    {
        // Common Causes:
        //    1) the battery on the device is low or even missing and the power line was disconnected
        Serial.println("RTC lost confidence in the DateTime!");
    }

  */

  display.clearDisplay();
  delay(8000); // 8 seconds

















}

//FUCTIONS 


void storageDATA(float h,float t,float m,float hic, String datetime){
      Serial.println("Storage data on SD......");

      //Concatenate all info separated by commas
      dataMessage = String(h) + "," + String(t) + "," + String(m) + "," + String(hic) + "," + datetime + "\r\n";
      Serial.print("Saving data: ");
      Serial.println(dataMessage);

      //Append the data to file
      appendFile(SD, "/data.txt", dataMessage.c_str());
      digitalWrite(LED_STA_SD, HIGH);   // turn the LED on (HIGH is the voltage level)
      delay(1000);                       // wait for a second
      digitalWrite(LED_STA_SD, LOW);    // turn the LED off by making the voltage LOW
      delay(1000);  
  
  
}




//Initialize Clock
void iniClock(){

  

  //Initialize date and time 1302 CLK
  Serial.print("Compile:");
  Serial.print(__DATE__);
  Serial.print(__TIME__);
  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  printDateTime(compiled);
  Serial.println();


  // Check if is the current time it is configure during the compilation time
  // Can be modified to reduce code

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



  
}





//Initialize Display
void iniDisplay(){
//Display setup
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(100, 100);
  // Display static text
  display.println("AgroMac");
  display.display();
  delay(1000);  
  
}





// Initialize WiFi
void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

//Micro SD Setup
void iniSD(){
  if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}






//Functions CLK
#define countof(a) (sizeof(a) / sizeof(a[0]))
String printDateTime(const RtcDateTime& dt)
{
  char datestring[20];

  snprintf_P(datestring,
             countof(datestring),
             PSTR("%02u-%02u-%04u %02u:%02u:%02u"),
             dt.Month(),
             dt.Day(),
             dt.Year(),
             dt.Hour(),
             dt.Minute(),
             dt.Second() );
  Serial.print(datestring);
  //display.print(datestring);
  return datestring;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}
