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

#define minWalkHeight 1
#define maxWalkHeight 17

#define minStepHeight 22
#define maxStepHeight 30

#define minForward  6
#define maxForward  60

#define leftServo 15
 #define rightServo 8

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
SensorHaptic servoToe = SensorHaptic(2, 1, &board, 90, 135, minForward, maxForward, 1);

SensorHaptic servoArch = SensorHaptic(4, 0, &board, 85, 120, minWalkHeight, maxWalkHeight, -1);
SensorHaptic servoHeel = SensorHaptic(15, 2, &board, 86, 120, minWalkHeight, maxWalkHeight, -1);

SensorHaptic hapticArch = SensorHaptic(4, 32, NULL, 130, 255, minStepHeight, maxStepHeight, 1);
SensorHaptic hapticHeel = SensorHaptic(15, 12, NULL, 135, 255, minStepHeight, maxStepHeight, 1);

//int testServo = 8;

int angleToPulse(int ang) {
 return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
}

String parseDirections(String directions){
  String answer = "";
  String number = "";
  directions.toLowerCase();

  if(directions.indexOf("right") != -1){
    answer += "r";
  }
  else if(directions.indexOf("left") != -1){
    answer += "l";
  }
  bool foundNumber = false;
  for(int i = 0; i< directions.length();++i){
    if(isDigit(directions[i])){
      foundNumber = true;
      number += directions[i];
    }
    else if (foundNumber){
      break;
    }
  }
  return answer + number;
}

  void parseGps(String value){
    int servo = rightServo;
    int angle;
    char direction = value[0];
    int distance  = value.substring(1, value.length()).toInt();
    //int flip = 1;
 Serial.print("distance:");
            Serial.print(distance);
            Serial.print("direction:");
            Serial.println(direction);
    if(direction == 'r'){
      angle = 145;
    }
    else{
      servo = leftServo;
      angle = 20;
    }

    for(int i = 0; i < distance; ++i){
      int pulse = map(angle,0,180,SERVOMIN,SERVOMAX);
          
        board.setPWM(servo, 0, pulse);
        delay(200);
        board.setPWM(servo, 0, map(90,0,180,SERVOMIN,SERVOMAX));
        delay(200);
    }

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

  parseGps("r2");
  
}
 
void loop() {
  // Check if data is available from the Bluetooth client
  if (SerialBT.available()) {
    String received = readBluetoothData();  // Call function to read the data safely
    if (received.length() > 0) {
       //Serial.println("Received: " + received);
        Serial.println(parseDirections(received));
        parseGps(parseDirections(received));
        
        received = "";
        
       
      // board.setPWM(testServo, 0, pulse);
        
    
    }
  }
  // int distance_cm = servoToe.readSensor();
  // if (distance_cm < 90){
  //   int ang = 0;
  //   ang = map(ang, 0, 180, servoArchMin, servoArchMax);
  //   int pulse = angleToPulse(ang);
  //   board.setPWM(2, 0, pulse);
  // }

 servoToe.update();
 servoArch.update();
 hapticArch.update(true);
 servoHeel.update();
 hapticHeel.update(true);
 
 delay(50);  // Short delay to avoid flooding the serial output
}
