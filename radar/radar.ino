#include <LiquidCrystal_I2C.h>
#include <HCSR04.h>
#include <Servo.h>

HCSR04 hc(5,6); //initialisation class HCSR04 (trig pin , echo pin)
LiquidCrystal_I2C lcd(0x27, 16, 2);

int potpin = A0;  // analog pin used to connect the potentiometer
int val; 
Servo myservo;    // create servo object to control a servo

void setup()
{
  myservo.attach(9);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  delay(2000);
  lcd.setCursor(0,0);             // Set LCD cursor to upper left corner, column 0, row 0
  lcd.print("Target Distance:");  // Print Message on First Row
}

void loop()
{
  val = analogRead(potpin);          // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 1023, 0, 180);   // scale it for use with the servo (value between 0 and 180)
  myservo.write(val);                // sets the servo position according to the scaled value
  delay(15);
  printLCD();
}

void printLCD(){
  lcd.setCursor(0,1);   // Set Cursor again to first column of second row
  lcd.print(hc.dist()); // Print measured distance
  lcd.print(" cm");     // Print your units.
  delay(250);           // Pause to let things settle
}