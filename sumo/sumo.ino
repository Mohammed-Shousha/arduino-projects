#include <HCSR04.h>

HCSR04 ultrasonic(2, new int[2]{ 3, 5 }, 2);  //(trig pin , echo pin, number of sensor)

#define IN1 6   //right forwad
#define IN2 9   // right backward
#define IN3 10  // left forward
#define IN4 11  // left backward

#define IR_sensor_front A0  // front sensor
#define IR_sensor_back A1   // rear sensor

int front_distance;
int back_distance;
int IR_front;
int IR_back;

void setup() {
  Serial.begin(9600);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  delay(5000);
}

void loop() {
  ROTATE(200);
  delay(500);
  STOP();
  delay(100);

  checkIR();
  checkDist();
}

void checkDist() {
  front_distance = ultrasonic.dist(0);
  back_distance = ultrasonic.dist(1);

  IR_front = digitalRead(IR_sensor_front);
  IR_back = digitalRead(IR_sensor_back);

  while (front_distance < 20)  // move backward if (front_distance < 20)
  {
    BACKWARD(255);
    delay(200);
    ROTATE(200);
    delay(200);
    FORWARD(255);
    delay(100);
    STOP();

    IR_front = digitalRead(IR_sensor_front);
    IR_back = digitalRead(IR_sensor_back);

    if (IR_front == 0 || IR_back == 0) {
      break;
    }
    delay(10);
  }

  while (back_distance < 20) {
    FORWARD(255);
    delay(200);
    ROTATE(200);
    delay(200);
    FORWARD(255);
    delay(100);
    STOP();

    IR_front = digitalRead(IR_sensor_front);
    IR_back = digitalRead(IR_sensor_back);
    if (IR_front == 0 || IR_back == 0) {
      break;
    }
    delay(10);
  }
}

void checkIR() {
  IR_front = digitalRead(IR_sensor_front);
  IR_back = digitalRead(IR_sensor_back);

  if (IR_front == 0)  // 0 means white line
  {
    STOP();
    delay(50);
    BACKWARD(255);
    delay(500);
  }

  if (IR_back == 0) {
    STOP();
    delay(50);
    FORWARD(255);
    delay(500);
  }
}

void FORWARD(int Speed) {
  analogWrite(IN1, Speed);
  analogWrite(IN2, 0);
  analogWrite(IN3, Speed);
  analogWrite(IN4, 0);
}

void BACKWARD(int Speed) {
  analogWrite(IN1, 0);
  analogWrite(IN2, Speed);
  analogWrite(IN3, 0);
  analogWrite(IN4, Speed);
}

void ROTATE(int Speed) {
  analogWrite(IN1, Speed);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, Speed);
}

void STOP() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}
