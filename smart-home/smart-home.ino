#include <Servo.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define humSensor A0
#define gasSensor 2

#define motor 4
#define buzzer 3

#define led1 11
#define led2 12
#define led3 13

Servo servo;

const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 9, 8, 25, 7 };
byte colPins[COLS] = { 6, 24, 23, 22 };

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String password = "";
String unlock = "1414";
String lock = "4141";

char command;

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

void setup() {
  Serial.begin(9600);
  servo.attach(10);
  servo.write(100);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  printLCD("Welcome!", 0 , "Enter Password", 0); 
  pinMode(motor, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  digitalWrite(led1,LOW);
  digitalWrite(led2,LOW);
  digitalWrite(led3,LOW);
}

void loop() {
  char key = keypad.getKey();
  int humValue = analogRead(humSensor);
  int gasValue = digitalRead(gasSensor);

  if (key != '*' && key != NO_KEY) {
    password += key;
    Serial.println(password);
  }
  if (key == '*' && password.length() == 4) {
    checkPassword();
    password = "";
  } else if (key == '*' && password.length() < 4) {
    password = "";
  } else if (key == '*' && password.length() > 4) {
    password = "";
  }
  
  bluetoothControl();
  checkGas(gasValue);
  checkHum(humValue); 
}

void bluetoothControl (){
  if(Serial.available()!=0)
  {
    command = Serial.read();
  }
  if(command == 'a')
  {
    digitalWrite(led1, HIGH);
    printLCD("Room 1 On");    
  }
  if(command == 'b')
  {
    digitalWrite(led1,LOW);
  }
  if(command == 'c'){
    digitalWrite(led2, HIGH);
    printLCD("Room 2 On");    
  }
  if(command == 'd'){
    digitalWrite(led2, LOW);
  }
  if(command == 'e'){
    digitalWrite(led3, HIGH);
    printLCD("Room 3 On");    
  }
  if(command == 'f'){
    digitalWrite(led3, LOW);
  }
}

void checkGas(int gasValue) {
   if (gasValue == 0) {
    digitalWrite(buzzer, HIGH);
    printLCD("Gas Leakage");
    delay(1500);
    lcd.clear();
  }
  if (gasValue == 1) {
    digitalWrite(buzzer, LOW);
  }
}

void checkHum(int humValue){
  if(humValue > 800 ){
    digitalWrite(motor, HIGH);
    printLCD("Grass is Dry"); 
    delay(1500);
    lcd.clear();   
  }
  if(humValue < 800){
    digitalWrite(motor, LOW);
  }
}

void checkPassword() {
  if (unlock == password) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Unlocked");
    servo.write(0);
  }
  if (lock == password) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Locked");
    servo.write(100);
  }
}

