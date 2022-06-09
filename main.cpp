#include <Arduino.h>

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(921600);
  // put your setup code here, to run once:
  Serial.println("Hellow Sebas from setup");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(100);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(10);
  Serial.println("Hellow from loop sebas");
  digitalWrite(LED_BUILTIN, LOW);



}