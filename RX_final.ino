#include <SPI.h>
#include <RF24.h>

// NRF24L01 Pin Definitions
#define CE_PIN 7
#define CSN_PIN 8
/// connect sck-13 mosi-11 miso - 12
// Front Motor Driver Pin Definitions
#define FRONT_MOTOR_EN1 9   // Enable/PWM for front left motor
#define FRONT_MOTOR_EN2 5   // Enable/PWM for front right motor
#define FRONT_MOTOR_IN1 A0   // Direction control for front right motor
#define FRONT_MOTOR_IN2 4   // Direction control for front right motor
#define FRONT_MOTOR_IN3 2  // Direction control for front left motor
#define FRONT_MOTOR_IN4 A1  // Direction control for front left motor

// Back Motor Driver Pin Definitions
#define BACK_MOTOR_EN1 3    // Enable/PWM for back left motor
#define BACK_MOTOR_EN2 6    // Enable/PWM for back right motor
#define BACK_MOTOR_IN1 A2   // Direction control for back left motor
#define BACK_MOTOR_IN2 A3   // Direction control for back left motor
#define BACK_MOTOR_IN3 A4   // Direction control for back right motor
#define BACK_MOTOR_IN4 A5   // Direction control for back right motor

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

struct JoystickData {
  int x;
  int y;
  bool button;
};

JoystickData joystickData;

void setup() {
  Serial.begin(9600);
  initMotorPins();
  
  if (!radio.begin()) {
    Serial.println("Radio hardware not responding!");
    while (1) {}
  }
  
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(76);
  radio.startListening();
  
  Serial.println("Robot Car Receiver initialized");
}

void initMotorPins() {
  pinMode(FRONT_MOTOR_EN1, OUTPUT);
  pinMode(FRONT_MOTOR_EN2, OUTPUT);
  pinMode(FRONT_MOTOR_IN1, OUTPUT);
  pinMode(FRONT_MOTOR_IN2, OUTPUT);
  pinMode(FRONT_MOTOR_IN3, OUTPUT);
  pinMode(FRONT_MOTOR_IN4, OUTPUT);
  
  pinMode(BACK_MOTOR_EN1, OUTPUT);
  pinMode(BACK_MOTOR_EN2, OUTPUT);
  pinMode(BACK_MOTOR_IN1, OUTPUT);
  pinMode(BACK_MOTOR_IN2, OUTPUT);
  pinMode(BACK_MOTOR_IN3, OUTPUT);
  pinMode(BACK_MOTOR_IN4, OUTPUT);
}

void loop() {
  if (radio.available()) {
    radio.read(&joystickData, sizeof(joystickData));
    
    int mappedX = map(joystickData.x, 0, 1023, -100, 100);
    int mappedY = map(joystickData.y, 0, 1023, -100, 100);
    
    if (abs(mappedX) < 10) mappedX = 0;
    if (abs(mappedY) < 10) mappedY = 0;
    
    controlRobotMovement(mappedX, mappedY);
  }
}

void controlRobotMovement(int x, int y) {
  // Calculate wheel speeds for differential steering
  int leftSpeed = y + x;
  int rightSpeed = y - x;
  
  leftSpeed = constrain(leftSpeed, -100, 100);
  rightSpeed = constrain(rightSpeed, -100, 100);
  
  // Front Motors Control
  controlMotorPair(
    FRONT_MOTOR_EN1, FRONT_MOTOR_EN2,
    FRONT_MOTOR_IN1, FRONT_MOTOR_IN2, 
    FRONT_MOTOR_IN3, FRONT_MOTOR_IN4, 
    leftSpeed, rightSpeed
  );
  
  // Back Motors Control
  controlMotorPair(
    BACK_MOTOR_EN1, BACK_MOTOR_EN2,
    BACK_MOTOR_IN1, BACK_MOTOR_IN2, 
    BACK_MOTOR_IN3, BACK_MOTOR_IN4, 
    leftSpeed, rightSpeed
  );
}

void controlMotorPair(int enablePin1, int enablePin2, int in1, int in2, int in3, int in4, int leftSpeed, int rightSpeed) {
  // Left motor control
  if (leftSpeed > 0) {
    digitalWrite(FRONT_MOTOR_IN3, LOW);
    digitalWrite(FRONT_MOTOR_IN4, HIGH);
    digitalWrite(BACK_MOTOR_IN1, LOW);
    digitalWrite(BACK_MOTOR_IN2, HIGH);
  } else if (leftSpeed < 0) {
    digitalWrite(FRONT_MOTOR_IN3, HIGH);
    digitalWrite(FRONT_MOTOR_IN4, LOW);
    digitalWrite(BACK_MOTOR_IN1, HIGH);
    digitalWrite(BACK_MOTOR_IN2, LOW);
  } else {
    digitalWrite(FRONT_MOTOR_IN3, LOW);
    digitalWrite(FRONT_MOTOR_IN4, LOW);
    digitalWrite(BACK_MOTOR_IN1, LOW);
    digitalWrite(BACK_MOTOR_IN2, LOW);
  }
  analogWrite(enablePin1, map(abs(leftSpeed), -100, 100, 0, 255));
  analogWrite(enablePin2, map(abs(leftSpeed), -100, 100, 0, 255));
  // Right motor control
  if (rightSpeed > 0) {
    digitalWrite(FRONT_MOTOR_IN1, LOW);
    digitalWrite(FRONT_MOTOR_IN2, HIGH);
    digitalWrite(BACK_MOTOR_IN3, LOW);
    digitalWrite(BACK_MOTOR_IN4, HIGH);
  } else if (rightSpeed < 0) {
    digitalWrite(FRONT_MOTOR_IN1, HIGH);
    digitalWrite(FRONT_MOTOR_IN2, LOW);
    digitalWrite(BACK_MOTOR_IN3, HIGH);
    digitalWrite(BACK_MOTOR_IN4, LOW);
    
  } else {
    digitalWrite(FRONT_MOTOR_IN1, LOW);
    digitalWrite(FRONT_MOTOR_IN2, LOW);
    digitalWrite(BACK_MOTOR_IN3, LOW);
    digitalWrite(BACK_MOTOR_IN4, LOW);
  }
  analogWrite(enablePin2, map(abs(rightSpeed), -100, 100, 0, 255));
  analogWrite(enablePin1 , map(abs(rightSpeed), -100, 100, 0, 255));
}

void stopMotors() {
  // Front Motors
  digitalWrite(FRONT_MOTOR_IN1, LOW);
  digitalWrite(FRONT_MOTOR_IN2, LOW);
  digitalWrite(FRONT_MOTOR_IN3, LOW);
  digitalWrite(FRONT_MOTOR_IN4, LOW);
  analogWrite(FRONT_MOTOR_EN1, 0);
  analogWrite(FRONT_MOTOR_EN2, 0);
  
  // Back Motors
  digitalWrite(BACK_MOTOR_IN1, LOW);
  digitalWrite(BACK_MOTOR_IN2, LOW);
  digitalWrite(BACK_MOTOR_IN3, LOW);
  digitalWrite(BACK_MOTOR_IN4, LOW);
  analogWrite(BACK_MOTOR_EN1, 0);
  analogWrite(BACK_MOTOR_EN2, 0);
}
