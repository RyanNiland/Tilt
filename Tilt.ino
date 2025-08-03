#include <Arduino.h> // Basic Arduino library for ESP32

// Global Variables
bool GoalFlag1 = LOW;
bool GoalFlag2 = LOW;
unsigned long GoalDebounceTime1 = 0;
unsigned long GoalDebounceTime2 = 0;
const unsigned long debounceDelay = 100; // 100 ms debounce

// Function Declarations
void IRAM_ATTR DetectGoal1();
void IRAM_ATTR DetectGoal2();

void setup() {
  Serial.begin(115200); // Initialize Serial for debugging
  pinMode(0, INPUT); // LED pin
  // digitalWrite(18, LOW); // Initialize LED off
  pinMode(2, INPUT); // Button 1 with internal pull-up
  attachInterrupt(digitalPinToInterrupt(0), DetectGoal1, FALLING); // Trigger on FALLING edge
  attachInterrupt(digitalPinToInterrupt(2), DetectGoal1, FALLING); // Trigger on FALLING edge

}

void IRAM_ATTR DetectGoal1() {
  GoalFlag1 = HIGH; // Set flag on button press
}

void IRAM_ATTR DetectGoal1() {
  GoalFlag2 = true; // Set flag on button press
}

void loop() {
  unsigned long currentTime = millis();
  
  // Goal 1 is scored in
  if (GoalFlag1 && (currentTime - GoalDebounceTime1 > debounceDelay)) {

    Serial.println("Goal 1 ");
    GoalFlag = LOW; // Reset flag
    lastDebounceTime1 = currentTime; // Update debounce time
  }

  // Goal 2 is scored in
   if (GoalFlag2 && (currentTime - GoalDebounceTime2 > debounceDelay)) {

    Serial.println("Goal 2 ");
    GoalFlag2 = LOW; // Reset flag
    lastDebounceTime2 = currentTime; // Update debounce time
  }
}