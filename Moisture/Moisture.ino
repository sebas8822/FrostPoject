#define SensorPin 4  // ESP32 pin analog feacture
int output_sensor;
void setup() { 
  Serial.begin(115200);
  Serial.println("Reading Moisture Sensor....");
  delay(2000);
}

void loop() {
  float sensorValue = analogRead(SensorPin);
  int output_sensor = sensorValue;
  Serial.print("Moisture: ");
  Serial.print(output_sensor);
  Serial.println("%");
  delay(1000);
} 
