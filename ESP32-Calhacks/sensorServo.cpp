#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN  125 // Minimum pulse length count (out of 4096)
#define SERVOMAX  575 // Maximum pulse length count (out of 4096)
class SensorHaptic {
    private:
        int sensorPin;
        int hapticPin;
        float minValue;
        float maxValue;
        float minDistance;
        float maxDistance;
        int direction;

        Adafruit_PWMServoDriver* board;

        //float deltaDistance;

    public:
        SensorHaptic(int sensorPin, int hapticPin, Adafruit_PWMServoDriver* boardPassed, float minValue = 0 , float maxValue = 90, float minDistance = 0, float maxDistance = 20, int direction = 1):board(boardPassed){

            this-> sensorPin = sensorPin;
            this-> hapticPin = hapticPin;
            this-> minValue = minValue;
            this-> maxValue = maxValue;
            this-> minDistance = minDistance;
            this-> maxDistance = maxDistance;
            this-> direction = direction;

        }

        float readSensor(){
            float volts = analogRead(sensorPin) * 0.0008056640625; // value from sensor * (3.3/4096)
            return  29.988 * pow( volts, -1.173);
        }

        void calibrate(int iterations = 10){
            float average = 0;
            for(int i = 0; i < iterations; ++i){
                average += readSensor();
                delay(100);
            }
            average /= iterations;
            //deltaDistance = average;

            minDistance += average;
            maxDistance += average;

        }
        int angleToPulse(int ang) {
          return map(ang, 0, 180, SERVOMIN, SERVOMAX); // Map angle (0-180) to pulse (SERVOMIN-SERVOMAX)
        }
        void update(bool isVibration = 0){
            int distance = readSensor();
            Serial.print("distance:");
            Serial.println(distance);
            Serial.print("pin:");
            int  value = map((distance < minDistance) ? minDistance: (distance > maxDistance) ? maxDistance: distance, minDistance, maxDistance, (direction ==1) ? minValue: maxValue, (direction ==1) ? maxValue: minValue);
            Serial.print(this-> sensorPin);

            Serial.print("value:");
            Serial.println(value);
            if (isVibration){
                analogWrite(hapticPin,value);
                return;
            }
            int pulse = angleToPulse(value);
            board->setPWM(hapticPin, 0, pulse);





        }



};