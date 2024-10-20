#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "sensorServo.cpp"


#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>


#define SERVICE_UUID        "cb4d1429-62b4-4e83-940e-77499e409770"
#define CHARACTERISTIC_UUID "119177b1-3c4d-4256-afd3-9f06ff01e1c6"

Adafruit_PWMServoDriver board = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  125 // Minimum pulse length count (out of 4096)
#define SERVOMAX  575 // Maximum pulse length count (out of 4096)


#define pulseCycle 2000
#define shortPulse 900
#define longPulse 2100


String message;
// Create a callback class to handle the write event
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      Serial.println("*********");
      Serial.print("Received Value: ");
      for (int i = 0; i < value.length(); i++)
        Serial.print(value[i]);
      Serial.println();
      message = value;
      Serial.println("*********");
    }
  }
};

// String readBluetoothData() {
//   String data = "";
//   while (SerialBT.available()) {
//     char c = SerialBT.read();
//     if (c == '\n') break;  // Stop reading if newline is received
//     data += c;
//   }
//   return data;
// }

int servoArchMin = 90;

int servoArchMax = 120;

SensorHaptic servoHeel = SensorHaptic(15, 0, &board, 90, 120, 0, 40, 1);
SensorHaptic hapticHeel = SensorHaptic(15, 12, NULL, 135, 255, 40, 100, 1);
SensorHaptic servoToe = SensorHaptic(2, 1, &board, 90, 120, 0, 50, -1);
SensorHaptic servoArch = SensorHaptic(4, 2, &board, 90, 120, 0, 40, 1);
SensorHaptic hapticArch = SensorHaptic(4, 32, NULL, 135, 255, 40, 100, 1);

int testServo = 8;


void setup() {
  Serial.begin(115200);

  board.begin();
  board.setPWMFreq(60); // Set the frequency to 60 Hz for the servos


  // calibrate sensorServos
  servoHeel.calibrate();
  servoArch.calibrate();
  hapticHeel.calibrate();

  Serial.println("Starting BLE work!");

  BLEDevice::init("ESP32-wroom");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

  pCharacteristic->setValue("Hello World says the foot thingy");

    // Attach the callback to handle data received from the phone
  pCharacteristic->setCallbacks(new MyCallbacks());


  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  pAdvertising->setMinInterval(32); // Minimum value for advertising interval (20ms)
  pAdvertising->setMaxInterval(64); // Maximum value for advertising interval (40ms)

  BLEDevice::startAdvertising();
  Serial.println("Characteristic defined! Now you can read it in your phone!");
}


unsigned long previousMicros = 0;
void loop() {
//   // Check if data is available from the Bluetooth client
//   if (SerialBT.available()) {
//     String received = readBluetoothData();  // Call function to read the data safely
//     if (received.length() > 0) {
//        Serial.println("Received: " + received);
//        bool isNum = true;

//       // received[received.length()-1] = '\0';

//        for(int i = 0; i < received.length();){
//         if(!isdigit(received[i])){
//           received.remove(i,1);
// //          isNum = false;
// //          break;
//         }
//         else{++i;
//         }
//        }
//      if(isNum){
//       Serial.println("This is a number fam"+ received);
//         if(received.length() > 0){
//           Serial.println("This is the length" + received.length());
//         int angle = received.toInt();
//         constrain(angle,0,180);
//         int pulse = map(angle,0,180,SERVOMIN,SERVOMAX);
        
       
//        board.setPWM(testServo, 0, pulse);
//         }
        
//      }
//     }
//   }

 servoHeel.update();
 servoToe.update();
 servoArch.update();
 hapticHeel.update(true);
 hapticArch.update(true);
 
 delay(150);  // Short delay to avoid flooding the serial output
}
int angleToPulse(int ang) {
 return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
}
