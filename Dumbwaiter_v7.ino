#include <Wire.h>
#include <hd44780.h>  // Include HD44780 library for LCD
#include <hd44780ioClass/hd44780_I2Cexp.h> // Include I2C expander for LCD

// Define the pins for the limit switches
const int topLimitSwitch = 2;
const int floor3LimitSwitch1 = 3;
const int floor3LimitSwitch2 = 4;
const int floor2LimitSwitch1 = 5;
const int floor2LimitSwitch2 = 6;
const int bottomLimitSwitch = 7;

// Define the pins for the momentary buttons
const int floorButton1 = 10;
const int floorButton2 = A2;
const int floorButton3 = 12;
const int floorButton4 = A3;

// Motor control pins (L298 motor driver)
const int motorPin1 = 8;
const int motorPin2 = 9;

// LCD Setup
hd44780_I2Cexp lcd;  // Initialize the LCD using the HD44780 I2C library

// Variables for floor tracking and direction
int currentFloor = 1;  // Start with an arbitrary valid floor, will be updated by limit switches
String direction = "Idle";
int desiredFloor = 1;  // Initially, the desired floor will match the current floor

unsigned long lastUpdateTime = 0;  // For non-blocking LCD updates
const unsigned long updateInterval = 1000; // Update interval (1 second)

// Setup function
void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize the limit switches and buttons as inputs with pull-up resistors
  pinMode(topLimitSwitch, INPUT_PULLUP);
  pinMode(floor3LimitSwitch1, INPUT_PULLUP);
  pinMode(floor3LimitSwitch2, INPUT_PULLUP);
  pinMode(floor2LimitSwitch1, INPUT_PULLUP);
  pinMode(floor2LimitSwitch2, INPUT_PULLUP);
  pinMode(bottomLimitSwitch, INPUT_PULLUP);
  
  pinMode(floorButton1, INPUT_PULLUP);
  pinMode(floorButton2, INPUT_PULLUP);
  pinMode(floorButton3, INPUT_PULLUP);
  pinMode(floorButton4, INPUT_PULLUP);

  // Motor control pins as outputs
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);

  // Initialize the LCD
  lcd.begin(16, 2);
  lcd.setBacklight(1);
  
  // Initialize currentFloor based on limit switches
  currentFloor = getCurrentFloor();
  desiredFloor = currentFloor;  // Set the desired floor to current floor initially
  
  updateLCD();
}

// Main loop
void loop() {
  // Update the current floor from the limit switches
  currentFloor = getCurrentFloor();
  
  // Read the state of the floor selection buttons and set the desired floor
  if (!motorIsRunning()) {
    desiredFloor = readMomentarySwitches();
  }

  // Ensure desiredFloor is within valid range (1 to 4)
  if (desiredFloor < 1 || desiredFloor > 4) {
    desiredFloor = currentFloor;  // If invalid, reset desiredFloor to currentFloor
  }

  // Control motor based on current and desired floor
  if (currentFloor == desiredFloor) {
    stopMotor();
    direction = "Idle";
  } else if (currentFloor > desiredFloor) {
    moveDumbwaiterDown();
    direction = "Down";
  } else if (currentFloor < desiredFloor) {
    moveDumbwaiterUp();
    direction = "Up";
  }

  // Update the LCD at regular intervals
  if (millis() - lastUpdateTime >= updateInterval) {
    updateLCD();
    lastUpdateTime = millis();
  }
  //REMOVE THIS DELAY ===============================================================================
  delay(1000);
  Serial.print("top switch is on: ");
  Serial.println(digitalRead(topLimitSwitch) == LOW);
  Serial.print("second floor switch1 is on: ");
  Serial.println(digitalRead(floor2LimitSwitch1) == LOW);
  Serial.print("second floor switch2 is on: ");
  Serial.println(digitalRead(floor2LimitSwitch2) == LOW);
  Serial.print("current floor is: ");
  Serial.println(currentFloor);
  Serial.print("desired floor is: ");
  Serial.println(desiredFloor);
  Serial.println();
}

// Function to stop the motor
void stopMotor() {
  digitalWrite(motorPin1, LOW);
  digitalWrite(motorPin2, LOW);
}

// Function to move the dumbwaiter up
void moveDumbwaiterUp() {
  // Prevent motor from moving if already at top floor
  if (currentFloor < 4 && digitalRead(topLimitSwitch) == HIGH) {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  }
}

// Function to move the dumbwaiter down
void moveDumbwaiterDown() {
  // Prevent motor from moving if already at bottom floor
  if (currentFloor > 1 && digitalRead(bottomLimitSwitch) == HIGH) {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  }
}

// Check if the motor is currently running
bool motorIsRunning() {
  return (digitalRead(motorPin1) == HIGH || digitalRead(motorPin2) == HIGH);
}

// Function to read the current floor based on limit switches
int getCurrentFloor() {
  if (digitalRead(topLimitSwitch) == LOW) return 4;
  if (digitalRead(floor3LimitSwitch1) == LOW && digitalRead(floor3LimitSwitch2) == LOW) return 3;
  if (digitalRead(floor2LimitSwitch1) == LOW && digitalRead(floor2LimitSwitch2) == LOW) return 2;
  if (digitalRead(bottomLimitSwitch) == LOW) return 1;
  return currentFloor;  // Return current floor if no limit switches are triggered
}

// Function to read momentary buttons for floor selection
int readMomentarySwitches() {
  if (digitalRead(floorButton1) == LOW) return 1;
  if (digitalRead(floorButton2) == LOW) return 2;
  if (digitalRead(floorButton3) == LOW) return 3;
  if (digitalRead(floorButton4) == LOW) return 4;
  return desiredFloor;  // Return the last desired floor if no button is pressed
}

// Update the LCD with the current floor and direction
void updateLCD() {
  lcd.clear();  // Clear the display
  lcd.setCursor(0, 0);
  lcd.print("Floor: ");
  lcd.print(currentFloor);
  lcd.setCursor(0, 1);
  lcd.print("Direction: ");
  lcd.print(direction);
}
