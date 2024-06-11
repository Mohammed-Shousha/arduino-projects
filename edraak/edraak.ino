// https://chatgpt.com/share/8ac74246-76fe-4eb9-86f7-0f430f5cdc52

// Define the LCD address and size

const int DIR_PIN = 18;    // GPIO pin connected to DIR on A4988
const int STEP_PIN = 5;    // GPIO pin connected to STEP on A4988
const int MS1_PIN = 25;    // GPIO pin connected to MS1 on A4988
const int MS2_PIN = 26;    // GPIO pin connected to MS2 on A4988
const int MS3_PIN = 27;    // GPIO pin connected to MS3 on A4988
const int endStopPin = 32; // GPIO pin connected to the end stop switch
const int trigPin = 33;    // Trigger pin for the ultrasonic sensor 
const int echoPin = 34;    // Echo pin for the ultrasonic sensor
const int steps_per_rev = 400; // Number of steps per revolution in full-step mode

bool motorRunning = false;
int motorDirection = 0; // 0: stopped, 1: forward, -1: backward


void setup() {
  Serial.begin(115200);

  pinMode(DIR_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(MS1_PIN, OUTPUT);
  pinMode(MS2_PIN, OUTPUT);
  pinMode(MS3_PIN, OUTPUT);
  pinMode(endStopPin, INPUT_PULLUP); // Set the end stop pin as input with pull-up resistor
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // Set microstepping mode (e.g., half-step mode)
  digitalWrite(MS1_PIN, HIGH);
  digitalWrite(MS2_PIN, HIGH);
  digitalWrite(MS3_PIN, HIGH);

  // Initialize the I2C bus with custom SDA and SCL pins
  
}

void loop() {
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    handleSerialInput(data);
  }

  if (motorRunning) {
    moveMotor();
  }
}

void handleSerialInput(String data) {
  if (data == "E") {
    long distance = calculateDistance();

    Serial.print("Distance:");
    Serial.println(distance);
    
  } else if (data == "F") {
    motorDirection = 1;
    motorRunning = true;
  } else if (data == "B") {
    motorDirection = -1;
    motorRunning = true;
  } else if (data == "S") {
    motorRunning = false;
  }
}

void moveMotor() {
  if (digitalRead(endStopPin) == LOW) {
    motorRunning = false;
    return;
  }

  digitalWrite(DIR_PIN, motorDirection == 1 ? HIGH : LOW);

  digitalWrite(STEP_PIN, HIGH);
  delayMicroseconds(1000); // Adjusted delay for half-step mode
  digitalWrite(STEP_PIN, LOW);
  delayMicroseconds(1000); // Adjusted delay for half-step mode
}

// Function to calculate distance using the ultrasonic sensor
long calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  long distance = duration * 0.034 / 2; // Calculate the distance in cm

  return distance;
}
