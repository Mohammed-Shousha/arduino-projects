#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#define INCLUDE_NOTIFICATION_MODULE

#include <Arduino.h>
#include <DabbleESP32.h>
#include <HCSR04.h>
#include <ESP32Servo.h>

#define IN3 12  // right forward
#define IN2 14  // left forward
#define IN4 13  // right backward
#define IN1 27  // left backward

#define servoPin 33

#define trig 25
#define echo 26

Servo servo;

HCSR04 hc(trig, echo);

void setup() {
  Serial.begin(115200);
  Dabble.begin("MyEsp32");
  servo.setPeriodHertz(50);
  servo.attach(servoPin, 1000, 2000);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  Dabble.waitForAppConnection();
  Notification.clear();
  Notification.setTitle("Distance");
}

void loop() {
  Dabble.processInput();
  if (GamePad.isUpPressed()) {
    forward();
  }
  if (GamePad.isDownPressed()) {
    backward();
  }
  if (GamePad.isLeftPressed()) {
    rotateLeft();
  }
  if (GamePad.isRightPressed()) {
    rotateRight();
  }
  if (GamePad.isSquarePressed()) {
    servoLeft();
  }
  if (GamePad.isCirclePressed()) {
    servoRight();
  }
  if (GamePad.isTrianglePressed()) {
    servoCenter();
  }
  if (GamePad.isCrossPressed()) {
    Notification.notifyPhone(String("Distance is: ")
                             + hc.dist()
                             + String(" cm"));
  }
  stop();
}

void forward() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 0);
}

void backward() {
  analogWrite(IN1, 255);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 255);
}

void rotateLeft() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 255);
  analogWrite(IN4, 0);
}

void rotateRight() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 255);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

void stop() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}

void servoLeft() {
  servo.write(180);
}

void servoRight() {
  servo.write(0);
}

void servoCenter() {
  servo.write(90);
}
