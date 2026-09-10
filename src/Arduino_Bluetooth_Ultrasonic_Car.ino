/*
  Arduino Bluetooth & Ultrasonic Car
  ----------------------------------
  Features:
  - Serial/Bluetooth motion control
  - Forward / backward movement
  - Forward and reverse steering
  - Two ultrasonic sensors for front/rear obstacle protection
  - Independent PWM speed control for the two motors

  Board:
  - Arduino Mega or any compatible board providing the same pins

  Commands:
  F = Forward
  B = Backward
  R = Right Forward
  L = Left Forward
  I = Right Backward
  E = Left Backward
  S = Stop
*/

// ========================
// Motor Driver Pins
// ========================

const uint8_t MOTOR_A_IN1 = 22;
const uint8_t MOTOR_A_IN2 = 23;
const uint8_t MOTOR_B_IN1 = 24;
const uint8_t MOTOR_B_IN2 = 25;

const uint8_t MOTOR_RIGHT_PWM = 12;
const uint8_t MOTOR_LEFT_PWM  = 11;

const uint8_t MOTION_LED = 26;


// ========================
// Ultrasonic Sensor Pins
// ========================

const uint8_t FRONT_TRIG = 27;
const uint8_t FRONT_ECHO = 28;

const uint8_t REAR_TRIG = 29;
const uint8_t REAR_ECHO = 30;


// ========================
// Configuration
// ========================

const float OBSTACLE_DISTANCE_CM = 10.0;

const uint8_t NORMAL_SPEED = 128;
const uint8_t TURN_SPEED   = 64;


// ========================
// State
// ========================

char command = '\0';

uint8_t rightMotorSpeed = 0;
uint8_t leftMotorSpeed  = 0;

bool movingForward  = false;
bool movingBackward = false;


// ========================
// Ultrasonic Distance
// ========================

float readDistanceCM(uint8_t trigPin, uint8_t echoPin)
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  unsigned long duration = pulseIn(echoPin, HIGH, 30000UL);

  // No echo received
  if (duration == 0)
  {
    return -1.0;
  }

  return duration * 0.0343 / 2.0;
}


// ========================
// Motor Direction
// ========================

void setForwardDirection()
{
  digitalWrite(MOTOR_A_IN1, HIGH);
  digitalWrite(MOTOR_A_IN2, LOW);

  digitalWrite(MOTOR_B_IN1, HIGH);
  digitalWrite(MOTOR_B_IN2, LOW);
}


void setBackwardDirection()
{
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, HIGH);

  digitalWrite(MOTOR_B_IN1, LOW);
  digitalWrite(MOTOR_B_IN2, HIGH);
}


// ========================
// Motion Functions
// ========================

void stopCar()
{
  digitalWrite(MOTOR_A_IN1, LOW);
  digitalWrite(MOTOR_A_IN2, LOW);
  digitalWrite(MOTOR_B_IN1, LOW);
  digitalWrite(MOTOR_B_IN2, LOW);

  rightMotorSpeed = 0;
  leftMotorSpeed = 0;

  movingForward = false;
  movingBackward = false;

  analogWrite(MOTOR_RIGHT_PWM, 0);
  analogWrite(MOTOR_LEFT_PWM, 0);

  digitalWrite(MOTION_LED, LOW);
}


void moveForward()
{
  setForwardDirection();

  rightMotorSpeed = NORMAL_SPEED;
  leftMotorSpeed  = NORMAL_SPEED;

  movingForward = true;
  movingBackward = false;
}


void moveBackward()
{
  setBackwardDirection();

  rightMotorSpeed = NORMAL_SPEED;
  leftMotorSpeed  = NORMAL_SPEED;

  movingForward = false;
  movingBackward = true;
}


void turnRightForward()
{
  setForwardDirection();

  rightMotorSpeed = NORMAL_SPEED;
  leftMotorSpeed  = TURN_SPEED;

  movingForward = true;
  movingBackward = false;
}


void turnLeftForward()
{
  setForwardDirection();

  rightMotorSpeed = TURN_SPEED;
  leftMotorSpeed  = NORMAL_SPEED;

  movingForward = true;
  movingBackward = false;
}


void turnRightBackward()
{
  setBackwardDirection();

  rightMotorSpeed = TURN_SPEED;
  leftMotorSpeed  = NORMAL_SPEED;

  movingForward = false;
  movingBackward = true;
}


void turnLeftBackward()
{
  setBackwardDirection();

  rightMotorSpeed = NORMAL_SPEED;
  leftMotorSpeed  = TURN_SPEED;

  movingForward = false;
  movingBackward = true;
}


// ========================
// Apply Motor Outputs
// ========================

void updateMotorOutputs()
{
  analogWrite(MOTOR_RIGHT_PWM, rightMotorSpeed);
  analogWrite(MOTOR_LEFT_PWM, leftMotorSpeed);

  if (movingForward || movingBackward)
  {
    digitalWrite(MOTION_LED, HIGH);
  }
  else
  {
    digitalWrite(MOTION_LED, LOW);
  }
}


// ========================
// Command Handling
// ========================

void handleCommand(char cmd, bool frontClear, bool rearClear)
{
  switch (cmd)
  {
    case 'F':
    case 'f':
      if (frontClear)
      {
        moveForward();
        Serial.println("MOTION: FORWARD");
      }
      else
      {
        stopCar();
        Serial.println("FRONT OBSTACLE: FORWARD BLOCKED");
      }
      break;

    case 'B':
    case 'b':
      if (rearClear)
      {
        moveBackward();
        Serial.println("MOTION: BACKWARD");
      }
      else
      {
        stopCar();
        Serial.println("REAR OBSTACLE: BACKWARD BLOCKED");
      }
      break;

    case 'R':
    case 'r':
      if (frontClear)
      {
        turnRightForward();
        Serial.println("MOTION: RIGHT FORWARD");
      }
      else
      {
        stopCar();
        Serial.println("FRONT OBSTACLE: RIGHT FORWARD BLOCKED");
      }
      break;

    case 'L':
    case 'l':
      if (frontClear)
      {
        turnLeftForward();
        Serial.println("MOTION: LEFT FORWARD");
      }
      else
      {
        stopCar();
        Serial.println("FRONT OBSTACLE: LEFT FORWARD BLOCKED");
      }
      break;

    case 'I':
    case 'i':
      if (rearClear)
      {
        turnRightBackward();
        Serial.println("MOTION: RIGHT BACKWARD");
      }
      else
      {
        stopCar();
        Serial.println("REAR OBSTACLE: RIGHT BACKWARD BLOCKED");
      }
      break;

    case 'E':
    case 'e':
      if (rearClear)
      {
        turnLeftBackward();
        Serial.println("MOTION: LEFT BACKWARD");
      }
      else
      {
        stopCar();
        Serial.println("REAR OBSTACLE: LEFT BACKWARD BLOCKED");
      }
      break;

    case 'S':
    case 's':
      stopCar();
      Serial.println("MOTION: STOPPED");
      break;

    default:
      break;
  }
}


// ========================
// Setup
// ========================

void setup()
{
  Serial.begin(9600);

  pinMode(MOTOR_A_IN1, OUTPUT);
  pinMode(MOTOR_A_IN2, OUTPUT);
  pinMode(MOTOR_B_IN1, OUTPUT);
  pinMode(MOTOR_B_IN2, OUTPUT);

  pinMode(MOTOR_RIGHT_PWM, OUTPUT);
  pinMode(MOTOR_LEFT_PWM, OUTPUT);

  pinMode(MOTION_LED, OUTPUT);

  pinMode(FRONT_TRIG, OUTPUT);
  pinMode(FRONT_ECHO, INPUT);

  pinMode(REAR_TRIG, OUTPUT);
  pinMode(REAR_ECHO, INPUT);

  digitalWrite(FRONT_TRIG, LOW);
  digitalWrite(REAR_TRIG, LOW);

  stopCar();

  Serial.println("Arduino Bluetooth & Ultrasonic Car");
  Serial.println("Commands: F B R L I E S");
  Serial.println("MOTION: STOPPED");
}


// ========================
// Main Loop
// ========================

void loop()
{
  float frontDistance = readDistanceCM(FRONT_TRIG, FRONT_ECHO);
  float rearDistance  = readDistanceCM(REAR_TRIG, REAR_ECHO);

  // A negative value means no valid echo was received.
  // In that case movement is blocked for safety.
  bool frontClear =
      (frontDistance >= OBSTACLE_DISTANCE_CM);

  bool rearClear =
      (rearDistance >= OBSTACLE_DISTANCE_CM);

  // Immediately stop forward motion if a front obstacle appears.
  if (movingForward && !frontClear)
  {
    stopCar();
    Serial.println("SAFETY STOP: FRONT OBSTACLE");
  }

  // Immediately stop reverse motion if a rear obstacle appears.
  if (movingBackward && !rearClear)
  {
    stopCar();
    Serial.println("SAFETY STOP: REAR OBSTACLE");
  }

  // Read a new Bluetooth/Serial command only when available.
  if (Serial.available() > 0)
  {
    command = Serial.read();
    handleCommand(command, frontClear, rearClear);
  }

  updateMotorOutputs();

  delay(20);
}
