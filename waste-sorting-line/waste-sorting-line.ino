#include <LiquidCrystal_I2C.h>

#define irSensor A6
#define proximitySensor A7

#define dirPin 2
#define stepPin 3
#define steps 45 // 81 degrees "360 degrees = 200 steps"
#define speed 1000

#define motorPin 10

LiquidCrystal_I2C lcd(0x27, 16, 2); // I2C address 0x27, 16 column and 2 rows

int irValue;
int proximityValue;

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(irSensor, INPUT);
  pinMode(proximitySensor, INPUT);
  pinMode(motorPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
}

void loop() {
  irValue = digitalRead(irSensor); // LOW "Object Detected" || HIGH "No Object Detected"

  if (irValue == LOW) {
    digitalWrite(motorPin, HIGH);  // start the motor
    printLCD("Motor Started", 1);
    delay(2000); // wait 2 seconds until the object reaches the proximity sensor
    proximityAndStepper();
  } else {
    digitalWrite(motorPin, LOW);
  }
}

void proximityAndStepper() {
  /*
  function controlling the stepper motor based on the proximity read 
  */

  proximityValue = digitalRead(proximitySensor); // LOW "Metal" || HIGH "Non-Metal"

  if (proximityValue == LOW) { // if the sensor detects metal open the gate
    printLCD("Material:", 0, "Metal", 5);
    rotateStepper(1); // rotate clockwise
    delay(6000); // keep the gate open for 6 seconds
    rotateStepper(0); // back to the initial position "rotate counterclockwise"
    delay(500); 
    lcd.clear();
  } else {
    printLCD("Material:", 0, "Non-Metal", 3);
    delay(7000); // keep the motor on for 7 seconds
    lcd.clear();
  }
}

void rotateStepper(int direction) {
  /*
  function rotating the stepper a constant number of steps
  either clockwise or counterclockwise 
  based on the direction provided 
  */

  if (direction == 1) {
    digitalWrite(dirPin, HIGH); // clockwise
  } else if (direction == 0) {
    digitalWrite(dirPin, LOW); // counterclockwise
  }

  for (int x = 0; x < steps; x++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(speed);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(speed);
  }
}

void printLCD(String text1= "", int pos1= 0, String text2= "", int pos2= 0) {
  /* 
  function to display text on lcd
    text1: text to be written in the first line default to ""
    pos1: the column position of text1 default to 0
    text2: text to be written in the second line default to ""
    pos2 the column position of text2 default to 0
  */

  lcd.clear();             // clear the lcd
  lcd.setCursor(pos1, 0);  // move cursor to column pos1 and line 0
  lcd.print(text1);        // print text1 at column pos1 and line 0
  lcd.setCursor(pos2, 1);  // move cursor to column pos2 and line 1
  lcd.print(text2);        // print text2 at column pos2 and line 1
}