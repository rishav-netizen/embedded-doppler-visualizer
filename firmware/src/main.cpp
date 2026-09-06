#include <Arduino.h>

const int switchPin = 2;
const int xPin = A0;
const int yPin = A1;

void setup() 
{
  // put your setup code here, to run once:
  Serial.begin(115200); //this is industry standard baudrate for low latency

  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(switchPin, INPUT_PULLUP); //cuz the switch is high when not pressed

}

void loop() 
{
  // put your main code here, to run repeatedly:
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  int switchValue = digitalRead(switchPin);

  //send value in terms of csv
  Serial.print(xValue);
  Serial.print(",");
  Serial.print(yValue);
  Serial.print(",");
  Serial.println(switchValue);

  // because the simulator runs at 60fps so we send data with that frequency
  delay(1000/60);
}
