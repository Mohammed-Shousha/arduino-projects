#include <SPI.h>
#include <Wire.h>
#include <PID_v1.h>
#include <max6675.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define SCREEN_ADDRESS 0x3D
#define OLED_RESET -1
#define CURSOR_WIDTH 3
#define CURSOR_HEIGHT 16
#define CURSOR_OFFSET 68

#define MAXDO 10
#define MAXCS 11
#define MAXCLK 12

#define SSR 9

#define OK_BUTTON 3
#define UP_BUTTON 4
#define DOWN_BUTTON 5
#define DECREASE_BUTTON 2
#define INCREASE_BUTTON 6
#define RUN_BUTTON 7

#define RED_LED 13
#define GREEN_LED 8

#define Kp 7
#define Ki 10
#define Kd 1
#define threshold 220

double input, setpoint, output;

struct TempTime {
  int temp;
  int time;
};

TempTime values[5] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

int selectedNumber = 0;
int valuesLength = sizeof(values) / sizeof(values[0]);
int numScreens = valuesLength + 1;
bool updateTemp = true;
bool updateValues = false;

unsigned long previousMillis = 0;
unsigned int timePassedMinutes = 0;
unsigned long lastReading = 0;

const unsigned int minute = 60000;      // 1 minute
const unsigned int second = 1000;       // 1 second
const unsigned int messageTime = 5000;  // 5 seconds

double startTemp;
double temp;


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
MAX6675 thermocouple(MAXCLK, MAXCS, MAXDO);
PID myPID(&input, &output, &setpoint, Kp, Ki, Kd, DIRECT);

void (*reset)(void) = 0;

void displayMessage(String str1, String str2) {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(str1);
  display.println(str2);
  display.display();
}

void checkValuesOnIncrease(int &value, int step, int upper = 1250) {
  if (value + step >= upper) {
    value = upper;
  } else {
    value += step;
  }
}

void checkValuesOnDecrease(int &value, int step, int lower = 0) {
  if (value - step <= lower) {
    value = lower;
  } else {
    value -= step;
  }
}

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setRotation(2);  // rotate 180
  display.setTextSize(2);
  display.setTextColor(WHITE);

  startTemp = thermocouple.readCelsius();
  setpoint = thermocouple.readCelsius();
  temp = thermocouple.readCelsius();
  output = 0;
  myPID.SetSampleTime(second);
  myPID.SetMode(AUTOMATIC);

  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(INCREASE_BUTTON, INPUT_PULLUP);
  pinMode(DECREASE_BUTTON, INPUT_PULLUP);
  pinMode(OK_BUTTON, INPUT_PULLUP);
  pinMode(RUN_BUTTON, INPUT_PULLUP);

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(RED_LED, LOW);


  displayMessage("Kiln Ctrl", "Program");
  delay(messageTime);
}

void loop() {
  checkRun();
  checkOK();
  checkDown();
  checkUp();
  checkIncrease();
  checkDecrease();
  displayOnScreen();
  if (updateValues) {
    drawCursor();
  }
}


void pidControl(int temp, int time, double startTemp) {
  double tempRate;
  unsigned int lastReading = 0;

  while (thermocouple.readCelsius() < temp || setpoint < temp || time == 0 || output > threshold || isnan(output)) {

    if(isnan(output)){
      displayMessage("ERORR!", "Press Stop");
      while(1){}
    }

    if (millis() - lastReading > second) {
      lastReading = millis();
      displayMessage("Temp:", String(thermocouple.readCelsius()) + " C");
    }

    if (startTemp > temp || temp == 0) {
      break;
    }

    input = thermocouple.readCelsius();

    if (time == 0) {
      setpoint = temp;
    }

    if (temp > startTemp && time > 0) {
      tempRate = (temp - startTemp) / time;
    }

    if ((millis() - previousMillis) >= minute && time > 0) {

      previousMillis = millis();

      if ((setpoint + tempRate) >= temp) {
        setpoint = temp;
      } else {
        setpoint += tempRate;
      }
    }

    pidOutput();
  }
}

void lastValue(int temp, int time) {
  bool startTime = false;
  unsigned long previousTime = 0;
  unsigned long lastReading = 0;

  while (thermocouple.readCelsius() < temp || time != 0 || output > threshold || isnan(output)) {

    if(isnan(output)){
      displayMessage("ERORR!", "Press Stop");
      while(1){}
    }

    if (millis() - lastReading > second) {
      lastReading = millis();
      displayMessage("Temp:", String(thermocouple.readCelsius()) + " C");
    }

    input = thermocouple.readCelsius();
    setpoint = temp;
    pidOutput();

    if (thermocouple.readCelsius() >= temp && time > 0) {
      startTime = true;
    }

    if (startTime && (millis() - previousTime >= minute)) {

      previousTime = millis();

      timePassedMinutes++;

      if (timePassedMinutes >= time && output < threshold) {
        break;
      }
    }
  }
}

void pidOutput() {
  myPID.Compute();

  Serial.print("output: ");
  Serial.println(output);

  if (output > threshold) {
    analogWrite(SSR, 255);
  } else {
    analogWrite(SSR, 0);
  }
}

void checkRun() {
  if (digitalRead(RUN_BUTTON) == LOW) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    for (int i = 0; i < valuesLength; i++) {
      if (i < valuesLength - 1) {
        startTemp = i == 0 ? startTemp : values[i - 1].temp;
        pidControl(values[i].temp, values[i].time, startTemp);
      } else {
        lastValue(values[i].temp, values[i].time);
      }
    }
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    displayMessage("Program", "Finished");
    while (1) {}
  }
}

void checkOK() {
  if (digitalRead(OK_BUTTON) == LOW && selectedNumber < 5) {
    updateValues = !updateValues;  // toggle
    updateTemp = true;

    while (digitalRead(OK_BUTTON) == LOW) {}
  }
}

void displayOnScreen() {
  display.clearDisplay();
  display.setCursor(0, 0);

  for (int i = 0; i < numScreens; i++) {
    if (i == 5) {
      display.println("Temp:");
      display.println(String(temp) + " C");
      if (millis() - lastReading > second) {
        lastReading = millis();
        temp = thermocouple.readCelsius();
      }
    } else if (selectedNumber == i) {
      display.print("Temp" + String(i + 1) + ":");
      display.println(values[i].temp);
      display.print("Time" + String(i + 1) + ":");
      display.println(values[i].time);
    }
  }

  display.display();
}

void drawCursor() {
  int cursorX = CURSOR_OFFSET;
  int cursorY = updateTemp ? 0 : 16;
  display.fillRect(cursorX, cursorY, CURSOR_WIDTH, CURSOR_HEIGHT, INVERSE);
  display.display();
  delay(1);
  display.clearDisplay();
}

void checkDown() {
  if (digitalRead(DOWN_BUTTON) == LOW) {
    if (updateValues) {
      updateTemp = !updateTemp;
    } else {
      selectedNumber = (selectedNumber + 1) % numScreens;
    }

    while (digitalRead(DOWN_BUTTON) == LOW) {}
  }
}

void checkUp() {
  if (digitalRead(UP_BUTTON) == LOW) {
    if (updateValues) {
      updateTemp = !updateTemp;
    } else {
      selectedNumber = (selectedNumber + numScreens - 1) % numScreens;
    }

    while (digitalRead(UP_BUTTON) == LOW) {}
  }
}

void checkIncrease() {
  if (digitalRead(INCREASE_BUTTON) == LOW && updateValues) {
    if (updateTemp) {
      checkValuesOnIncrease(values[selectedNumber].temp, 5);
    } else {
      checkValuesOnIncrease(values[selectedNumber].time, 1, 1000);
    }
  }
  delay(30);  // to test again
}

void checkDecrease() {
  if (digitalRead(DECREASE_BUTTON) == LOW && updateValues) {
    if (updateTemp) {
      checkValuesOnDecrease(values[selectedNumber].temp, 5);
    } else {
      checkValuesOnDecrease(values[selectedNumber].time, 1);
    }
  }
  delay(30);
}
