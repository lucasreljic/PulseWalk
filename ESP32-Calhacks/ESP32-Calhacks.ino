#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "BluetoothSerial.h" // Bluetooth Serial library for ESP32
#include "sensorServo.cpp"
BluetoothSerial SerialBT;     // Create a Bluetooth Serial object

Adafruit_PWMServoDriver board = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125 // Minimum pulse length count (out of 4096)
#define SERVOMAX  575 // Maximum pulse length count (out of 4096)



int servoArchMin = 90;

int servoArchMax = 120;

SensorHaptic servoHeel = SensorHaptic(15, 0, &board, 90, 120, 0, 100, 1);
SensorHaptic servoToe = SensorHaptic(2, 1, &board, 90, 120, 0, 50, -1);
SensorHaptic servoArch = SensorHaptic(4, 2, &board, 90, 120, 0, 100, 1);


void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_ServoControl"); // Bluetooth name you will see when connecting
  Serial.println("Bluetooth device is ready to pair");

  board.begin();
  board.setPWMFreq(60); // Set the frequency to 60 Hz for the servos


  // calibrate sensorServos
  servoHeel.calibrate();
  servoArch.calibrate();
  
}
void loop() {
  // Check if data is available from the Bluetooth client
  if (SerialBT.available()) {
    String received = readBluetoothData();  // Call function to read the data safely
    if (received.length() > 0) {
      // Serial.println("Received: " + received);
    }
  }
  // float volts = analogRead(2) * 0.0008056640625; // value from sensor * (3.3/4096)
  // int distance_cm = 29.988 * pow( volts, -1.173);
  // if (distance_cm < 90){
  //   int ang = 0;
  //   ang = map(ang, 0, 180, servoArchMin, servoArchMax);
  //   int pulse = angleToPulse(ang);
  //   board.setPWM(servoToe, 0, pulse);
  // }
  servoHeel.update();
  servoToe.update();
  servoArch.update();
  
  delay(100);  // Short delay to avoid flooding the serial output
}
int angleToPulse(int ang) {
  return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
}

String readBluetoothData() {
  String data = "";
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') break;  // Stop reading if newline is received
    data += c;
  }
  return data;
}

