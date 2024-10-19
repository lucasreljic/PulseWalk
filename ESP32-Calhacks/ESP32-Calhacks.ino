#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#ifndef APSSID
#define APSSID "ESPap"
#define APPSK "thereisnospoon"
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;

ESP8266WebServer server(80);
Adafruit_PWMServoDriver board = Adafruit_PWMServoDriver(0x40);
#define SERVOMIN  125 // this is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  575 // this is the 'maximum' pulse length count (out of 4096)

int servoNum = 0;

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected. Use /servo?angle=<value> to set the servo angle.</h1>");
}

// Handler to set the servo angle based on input from the HTTP request
void handleServo() {
  if (server.hasArg("angle")) {
    String angleStr = server.arg("angle");
    int angle = angleStr.toInt();
    
    if (angle >= 0 && angle <= 180) {
      int pulse = angleToPulse(angle);
      board.setPWM(servoNum, 0, pulse);
      String response = "Servo angle set to: " + String(angle);
      server.send(200, "text/html", response);
    } else {
      server.send(400, "text/html", "Invalid angle. Please provide a value between 0 and 180.");
    }
  } else {
    server.send(400, "text/html", "Please provide an angle using /servo?angle=<value>");
  }
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  
  server.on("/", handleRoot);
  server.on("/servo", handleServo); // Add endpoint to set the servo angle
  
  server.begin();
  Serial.println("HTTP server started");

  board.begin();
  board.setPWMFreq(60); 
}

void loop() {
  server.handleClient();
}

int angleToPulse(int ang) {
  return map(ang, 0, 180, SERVOMIN, SERVOMAX);
}