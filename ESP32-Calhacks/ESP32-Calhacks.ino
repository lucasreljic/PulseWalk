#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "BluetoothSerial.h" // Bluetooth Serial library for ESP32
#include "sensorServo.cpp"
#include <Pairs.h>
BluetoothSerial SerialBT;     // Create a Bluetooth Serial object

Adafruit_PWMServoDriver board = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125 // Minimum pulse length count (out of 4096)
#define SERVOMAX  575 // Maximum pulse length count (out of 4096)


#define pulseCycle 2000
#define shortPulse 900
#define longPulse 2100

#define minWalkHeight 1
#define maxWalkHeight 12

#define minStepHeight 12
#define maxStepHeight 20

#define minForward  6
#define maxForward  30

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
//sensorPin, hapticPin, board, minValue, maxValue, minDistance, maxDistance, direction
SensorHaptic servoToe = SensorHaptic(2, 1, &board, 90, 120, minForward, maxForward, -1);

<<<<<<< HEAD
SensorHaptic servoArch = SensorHaptic(4, 2, &board, 90, 120, minWalkHeight, maxWalkHeight, 1);
SensorHaptic servoHeel = SensorHaptic(15, 0, &board, 90, 120, minWalkHeight, maxWalkHeight, 1);

SensorHaptic hapticArch = SensorHaptic(4, 33, NULL, 130, 255, minStepHeight, maxStepHeight, 1);
SensorHaptic hapticHeel = SensorHaptic(15, 32, NULL, 135, 255, minStepHeight, maxStepHeight, 1);
=======
SensorHaptic servoHeel = SensorHaptic(15, 0, &board, 100, 150, 0, 40, -1);
SensorHaptic hapticHeel = SensorHaptic(15, 12, NULL, 135, 255, 40, 100, 1);
SensorHaptic servoToe = SensorHaptic(2, 1, &board, 90, 120, 0, 50, -1);
SensorHaptic servoArch = SensorHaptic(4, 2, &board, 90, 120, 0, 40, -1);
SensorHaptic hapticArch = SensorHaptic(4, 32, NULL, 135, 255, 40, 100, 1);
>>>>>>> 49bb8dbd93dadfd9ff0dd54314cc822510d64ed4

int testServo = 8;

string parseDirections(string directions){
  string answer = "";
  string number = "";
  bool findingDirection = true;
  bool appendingNumber = false;
  directions = directions.toLowerCase();
  for(int i = 0; i < directions.length(); ++i){
    if(findingDirection){
      if(directions[i]  == 'r' && i+ 5 < directions.length()){
          if(directions.subString(i,i+5) == "right"){
            answer += 'r';
            findingDirection = false;
          }
      }
      else if(directions[i]  == 'l' && i+ 4 < directions.length()){
          if(directions.subString(i,i+4) == "left"){
              answer += 'l';
              findingDirection = false;
          }
        }
      }
      if(isDigit(directions[i])){
        do{
          number += directions[i++]
        }while(isDigit(directions[i]) && i < directions.length())
      }
    }
    return  answer + number;
  }
    

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
      //Serial.println("This is a number fam"+ received);
        if(received.length() > 0){
          //Serial.println("This is the length" + received.length());
        int angle = received.toInt();
        constrain(angle,0,180);
        int pulse = map(angle,0,180,SERVOMIN,SERVOMAX);
        
       
       board.setPWM(testServo, 0, pulse);
        }
        
     }
    }
  }
<<<<<<< HEAD
  distance_cm = servoToe.readSensor();
  if (distance_cm < 90){
    int ang = 0;
    ang = map(ang, 0, 180, servoArchMin, servoArchMax);
    int pulse = angleToPulse(ang);
    board.setPWM(servoToe, 0, pulse);
  }

 servoToe.update();
 servoArch.update();
 hapticArch.update(true);
 servoHeel.update();
 hapticHeel.update(true);
 
 delay(100);  // Short delay to avoid flooding the serial output
=======

 servoHeel.update();
 servoToe.update();
 servoArch.update();
 hapticHeel.update(true);
 hapticArch.update(true);
 
 delay(50);  // Short delay to avoid flooding the serial output
}
int angleToPulse(int ang) {
 return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
>>>>>>> 49bb8dbd93dadfd9ff0dd54314cc822510d64ed4
}
int angleToPulse(int ang) {
 return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
}