#include <Wire.h>
#include <hd44780.h>  // Include HD44780 library for LCD
#include <hd44780ioClass/hd44780_I2Cexp.h> // Include i/o class header

// Define the pins for the limit switches
const int topLimitSwitch = 2;  // Top floor limit switch
const int floor3LimitSwitch = 3; // 3rd floor limit switch
const int floor2LimitSwitch = 5; // 2nd floor limit switch
const int bottomLimitSwitch = 7;  // Bottom floor limit switch

// Define the pins for the momentary buttons
const int floorButton1 = 10;  // Button to select Floor 1
const int floorButton2 = 11;  // Button to select Floor 2
const int floorButton3 = 12;  // Button to select Floor 3
const int floorButton4 = 13;  // Button to select Floor 4

// Motor control pins (connected to L298 motor driver)
const int motorPin1 = 8;  // IN1 (for motor direction)
const int motorPin2 = 9;  // IN2 (for motor direction)

// Setup LCD (use HD44780 I2C library, the address 0x3F is common)
hd44780_I2Cexp lcd;  // Initialize the LCD using the HD44780 I2C library

// Variables for floor tracking and direction
int currentFloor = 1;  // Start at floor 1
String direction = "Idle"; // Default direction
bool motorRunning = false;  // Flag to track if the motor is running

// Setup
void setup() {
  // Set limit switch pins as input
  pinMode(topLimitSwitch, INPUT_PULLUP);
  pinMode(floor3LimitSwitch, INPUT_PULLUP);
  pinMode(floor2LimitSwitch, INPUT_PULLUP);
  pinMode(bottomLimitSwitch, INPUT_PULLUP);

  // Set button pins as input
  pinMode(floorButton1, INPUT_PULLUP);
  pinMode(floorButton2, INPUT_PULLUP);
  pinMode(floorButton3, INPUT_PULLUP);
  pinMode(floorButton4, INPUT_PULLUP);

  // Set motor control pins as output
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);

  // Initialize motor off
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);

  // Initialize the LCD
  lcd.begin(16, 2);  // 16 columns, 2 rows
  lcd.backlight();
  lcd.setCursor(0, 0);  // Set cursor to the first line, first column
  lcd.print("Floor: ");
  lcd.print(currentFloor);
  lcd.setCursor(0, 1);  // Move to the second line
  lcd.print("Direction: ");
  lcd.print(direction);
}

// Main loop
void loop() {
  // Read the state of the limit switches
  bool atTop = digitalRead(topLimitSwitch) == LOW;
  bool atFloor3 = digitalRead(floor3LimitSwitch) == LOW;
  bool atFloor2 = digitalRead(floor2LimitSwitch) == LOW;
  bool atBottom = digitalRead(bottomLimitSwitch) == LOW;

  // Read the state of the floor selection buttons
  bool goToFloor1 = digitalRead(floorButton1) == LOW;
  bool goToFloor2 = digitalRead(floorButton2) == LOW;
  bool goToFloor3 = digitalRead(floorButton3) == LOW;
  bool goToFloor4 = digitalRead(floorButton4) == LOW;

  // If a button is pressed and motor is not moving to the same floor
  if (goToFloor1 && currentFloor != 1 && !motorRunning) {
    direction = "Down";
    currentFloor = 1;
    motorRunning = true;
    moveDumbwaiterDown();
  } 
  else if (goToFloor2 && currentFloor != 2 && !motorRunning) {
    direction = "Up";
    currentFloor = 2;
    motorRunning = true;
    moveDumbwaiterUp();
  } 
  else if (goToFloor3 && currentFloor != 3 && !motorRunning) {
    direction = "Up";
    currentFloor = 3;
    motorRunning = true;
    moveDumbwaiterUp();
  } 
  else if (goToFloor4 && currentFloor != 4 && !motorRunning) {
    direction = "Up";
    currentFloor = 4;
    motorRunning = true;
    moveDumbwaiterUp();
  }

  // Stop the motor if the current floor's limit switch is triggered
  if ((atTop && currentFloor == 4) || 
      (atBottom && currentFloor == 1) || 
      (atFloor2 && currentFloor == 2) || 
      (atFloor3 && currentFloor == 3)) {
    stopMotor();
    motorRunning = false;  // Motor is no longer running
    direction = "Idle";  // Set direction to idle when the motor stops
  }

  // Update the LCD with current floor and direction
  lcd.setCursor(7, 0);
  lcd.print(currentFloor);

  lcd.setCursor(11, 1);
  lcd.print(direction);
}

// Function to stop the motor
void stopMotor() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}

// Function to move the dumbwaiter up
void moveDumbwaiterUp() {
  digitalWrite(motorPin1, HIGH);
  digitalWrite(motorPin2, LOW);
}

// Function to move the dumbwaiter down
void moveDumbwaiterDown() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, HIGH);
}
