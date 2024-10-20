#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "BluetoothSerial.h" // Bluetooth Serial library for ESP32
#include "sensorServo.cpp"
BluetoothSerial SerialBT;     // Create a Bluetooth Serial object

Adafruit_PWMServoDriver board = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125 // Minimum pulse length count (out of 4096)
#define SERVOMAX  575 // Maximum pulse length count (out of 4096)


#define pulseCycle 2000
#define shortPulse 900
#define longPulse 2100

String readBluetoothData() {
  String data = "";
  while (SerialBT.available()) {
    char c = SerialBT.read();
    if (c == '\n') break;  // Stop reading if newline is received
    data += c;
  }
  return data;
}

int servoArchMin = 90;

int servoArchMax = 120;

SensorHaptic servoHeel = SensorHaptic(15, 0, &board, 100, 150, 0, 40, -1);
SensorHaptic hapticHeel = SensorHaptic(15, 12, NULL, 135, 255, 40, 100, 1);
SensorHaptic servoToe = SensorHaptic(2, 1, &board, 90, 120, 0, 50, -1);
SensorHaptic servoArch = SensorHaptic(4, 2, &board, 90, 120, 0, 40, -1);
SensorHaptic hapticArch = SensorHaptic(4, 32, NULL, 135, 255, 40, 100, 1);

int testServo = 8;


void setup() {
  Serial.begin(115200);

  SerialBT.begin("ESP32_ServoControl"); // Bluetooth name you will see when connecting
  Serial.println("Bluetooth device is ready to pair");

  board.begin();
  board.setPWMFreq(60); // Set the frequency to 60 Hz for the servos


  // calibrate sensorServos
  servoHeel.calibrate();
  servoArch.calibrate();
  hapticHeel.calibrate();
  
}
 unsigned long previousMicros = 0;
void loop() {
  // Check if data is available from the Bluetooth client
  if (SerialBT.available()) {
    String received = readBluetoothData();  // Call function to read the data safely
    if (received.length() > 0) {
       Serial.println("Received: " + received);
       bool isNum = true;

      // received[received.length()-1] = '\0';

       for(int i = 0; i < received.length();){
        if(!isdigit(received[i])){
          received.remove(i,1);
//          isNum = false;
//          break;
        }
        else{++i;
        }
       }
     if(isNum){
      Serial.println("This is a number fam"+ received);
        if(received.length() > 0){
          Serial.println("This is the length" + received.length());
        int angle = received.toInt();
        constrain(angle,0,180);
        int pulse = map(angle,0,180,SERVOMIN,SERVOMAX);
        
       
       board.setPWM(testServo, 0, pulse);
        }
        
     }
    }
  }

 servoHeel.update();
 servoToe.update();
 servoArch.update();
 hapticHeel.update(true);
 hapticArch.update(true);
 
 delay(50);  // Short delay to avoid flooding the serial output
}
int angleToPulse(int ang) {
 return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
}
