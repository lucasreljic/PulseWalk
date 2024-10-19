#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define SERVOMIN  125 // Minimum pulse length count (out of 4096)
#define SERVOMAX  575 // Maximum pulse length count (out of 4096)
#define WINDOW_SIZE 10
#define UPPER_LIMIT 800
class SensorHaptic {
    private:
        int sensorPin;
        int hapticPin;
        float minValue;
        float maxValue;
        float minDistance;
        float maxDistance;
        int direction;
        std::array<int, WINDOW_SIZE> movingAvg = {};  // Initialized to zero

        Adafruit_PWMServoDriver* board;

        //float deltaDistance;
      int getMovingAvg(int newDistance) {
          int avgDistance = 0;
          if (newDistance > UPPER_LIMIT){ // stopping extraneous values from getting into moving avg
            newDistance = movingAvg[WINDOW_SIZE - 1];
          }
          for(int i = 0; i < WINDOW_SIZE - 1; i++) {
            movingAvg[i] = movingAvg[i+1];
            avgDistance += movingAvg[i];
          }
          movingAvg[WINDOW_SIZE - 1] = newDistance;
          avgDistance += newDistance;  // Add the new distance to the sum

          return (avgDistance / WINDOW_SIZE);  // Return the average
        }

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
            int distance = getMovingAvg(readSensor());
            Serial.print("distance:");
            Serial.print(distance);
            Serial.print("pin:");
            int  value = map(distance, minDistance, maxDistance, (direction ==1) ? minValue: maxValue, (direction ==1) ? maxValue: minValue);
            value = (value < minValue) ? minValue: (value > maxValue) ? maxValue: value;
            Serial.print(this-> sensorPin);

            Serial.print("value:");
            Serial.println(value);
            if (isVibration){
                Serial.print("hapticvalue:");
                Serial.println(value);
                if (value < minValue + 1) {
                  value = 0;
                }
                analogWrite(hapticPin, value);

                return;
            }
            int pulse = angleToPulse(value);
            board->setPWM(hapticPin, 0, pulse);





        }



};