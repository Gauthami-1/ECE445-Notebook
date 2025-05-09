#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "ESP32C3_AP";
const char* password = "password123";

// Create a web server on port 80
WebServer server(80);

bool motorsRunning = false;


// Motor control pins
const int motorPin1 = 2; // left motor
const int motorPin2 = 3; // left motor
const int motorPin3 = 4; // right motor
const int motorPin4 = 5; // right motor
const int motorPin5 = 6; // tombstone
const int motorPin6 = 7; // tombstone

// Motor speed (0-255)
int motorSpeed = 255;      // Full speed for motors 1 and 2
int motorSpeedSlower = 245; // Slightly slower speed for motors 3 and 4
int tombstoneSpeed = 80;


void stopAllMotors() {
  analogWrite(motorPin1, 0);
  analogWrite(motorPin2, 0);
  analogWrite(motorPin3, 0);
  analogWrite(motorPin4, 0);
  analogWrite(motorPin5, 0);
  analogWrite(motorPin6, 0);
}

void setup() {
  Serial.begin(115200);

  // Initialize motor pins
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(motorPin5, OUTPUT);
  pinMode(motorPin6, OUTPUT);

  WiFi.setSleep(false);
  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.println(WiFi.softAPIP());

  // Forward command
  server.on("/forward", []() {
    analogWrite(motorPin1, motorSpeed);  // Motor 1 forward (left)
    analogWrite(motorPin2, 0);           // Motor 2 forward (left)
    analogWrite(motorPin3, motorSpeedSlower);  // Motor 3 forward (right)
    analogWrite(motorPin4, 0);           // Motor 4 forward (right)
    motorsRunning = true;

    server.send(200, "text/plain", "Moving forward");
  });

  // Backward command
  server.on("/backward", []() {
    analogWrite(motorPin1, 0);          // Motor 1 backward (left)
    analogWrite(motorPin2, motorSpeed); // Motor 2 backward (left)
    analogWrite(motorPin3, 0);          // Motor 3 backward (right)
    analogWrite(motorPin4, motorSpeedSlower);  // Motor 4 backward (right)
    motorsRunning = true;

    server.send(200, "text/plain", "Moving backward");
  });

  // LEFT command
  server.on("/left", []() {
    analogWrite(motorPin1, 0);          // Motor 2 backward (left)
    analogWrite(motorPin2, motorSpeed); // Motor 3 backward (left)
    analogWrite(motorPin3, motorSpeedSlower); // Motor 4 forward (right)
    analogWrite(motorPin4, 0);          // Motor 5 forward (right)
    motorsRunning = true;
    server.send(200, "text/plain", "Moving left");
  });

  // RIGHT command
  server.on("/right", []() {
    analogWrite(motorPin1, motorSpeed); // Motor 2 forward (left)
    analogWrite(motorPin2, 0);          // Motor 3 forward (left)
    analogWrite(motorPin3, 0);          // Motor 4 backward (right)
    analogWrite(motorPin4, motorSpeedSlower); // Motor 5 backward (right)
    motorsRunning = true;
    server.send(200, "text/plain", "Moving right");
  });

  // STOP command
  server.on("/stop", []() {
    analogWrite(motorPin1, 0);
    analogWrite(motorPin2, 0);
    analogWrite(motorPin3, 0);
    analogWrite(motorPin4, 0);
    motorsRunning = false;

    server.send(200, "text/plain", "Stopped all motors");
  });

  server.on("/tombstone_on", []() {
    analogWrite(motorPin5, tombstoneSpeed); // Tombstone motor forward
    analogWrite(motorPin6, 0);
    motorsRunning = true;
    server.send(200, "text/plain", "Tombstone spinning slowly");
  });

  server.on("/tombstone_off", []() {
    analogWrite(motorPin5, 0);
    analogWrite(motorPin6, 0);
    motorsRunning = false;

    server.send(200, "text/plain", "Tombstone stopped");
  });

  server.begin();
}




 //added stop functioanlity by checking if bool var is ever changed. 
void loop() {
  server.handleClient();

  static unsigned long lastCheck = 0;
  if (millis() - lastCheck > 1000) { // check every 1s
    lastCheck = millis();

    int numClients = WiFi.softAPgetStationNum();
    if (numClients == 0 && motorsRunning) {
      Serial.println("No clients connected. Stopping all motors.");
      stopAllMotors();
      motorsRunning = false;
    }
  }
}


