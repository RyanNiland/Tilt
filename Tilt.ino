//Includes
#include <ESP32Servo.h>

//defines
#define REACT_BUTTON_P1_PIN 19
#define REACT_BUTTON_P2_PIN 34

#define REACT_LED_P1_PIN 17
#define REACT_LED_P2_PIN 18

#define X_AXIS_UP_P1_PIN 25
#define X_AXIS_DOWN_P1_PIN 26
#define Y_AXIS_LEFT_P1_PIN 27
#define Y_AXIS_RIGHT_P1_PIN 32
#define X_AXIS_UP_P2_PIN 12
#define X_AXIS_DOWN_P2_PIN 13
#define Y_AXIS_LEFT_P2_PIN 14
#define Y_AXIS_RIGHT_P2_PIN 15

#define P1_GOAL_PIN 2
#define P2_GOAL_PIN 4

#define X_SERVO_PIN 21
#define Y_SERVO_PIN 22

#define SERVO_UPDATE_PERIOD_MS 100

#define GOALS_PER_MATCH 3

#define DEFAULT_X_ANGLE 45
#define DEFAULT_Y_ANGLE 55

#define DEG_PER_PRESS 2

#define DEBOUNCE_PERIOD_MS 20
#define GOAL_DEBOUNCE_MS 100
//Global Variables
bool inGame = false; // individual point
bool inMatch = false; // whole play

 bool reactP1State = false;
 bool reactP2State = false;
bool printP1ReadyOnce = true;
bool printP2ReadyOnce = true;

bool p1GoalScored = false;
bool p2GoalScored = false;


uint8_t goalCountP1 = 0;
uint8_t goalCountP2 = 0;

 int p1PressesX = 0;
 int p1PressesY = 0;
 int p2PressesX = 0;
 int p2PressesY = 0;

Servo xServo;
Servo yServo;

unsigned long lastServoUpdate = 0;

unsigned long lastDebounceP1React = 0;
unsigned long lastDebounceP2React = 0;

unsigned long lastDebounceP1Up = 0;
unsigned long lastDebounceP1Down = 0;
unsigned long lastDebounceP1Left = 0;
unsigned long lastDebounceP1Right = 0;
unsigned long lastDebounceP2Up = 0;
unsigned long lastDebounceP2Down = 0;
unsigned long lastDebounceP2Left = 0;
unsigned long lastDebounceP2Right = 0;

unsigned long lastDebounceGoalP1 = 0;
unsigned long lastDebounceGoalP2 = 0;

unsigned long currentTime = 0;


//Function Initiations
void IRAM_ATTR DetectGoalP1();
void IRAM_ATTR DetectGoalP2();

bool checkMatchStartCondition(void);
bool checkGameStartCondition(void);

int currentXAngle = DEFAULT_X_ANGLE;
int currentYAngle = DEFAULT_Y_ANGLE;

bool prevP1Up = HIGH, prevP1Down = HIGH, prevP1Left = HIGH, prevP1Right = HIGH;
bool prevP2Up = HIGH, prevP2Down = HIGH, prevP2Left = HIGH, prevP2Right = HIGH;

void IRAM_ATTR DetectGoalP1() {
  p1GoalScored = HIGH; // Set flag on button press
}

void IRAM_ATTR DetectGoalP2() {
  p2GoalScored = HIGH; // Set flag on button press
}
//Set-up of pins
void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Power up");
  
  //set pins
  pinMode(REACT_BUTTON_P1_PIN, INPUT_PULLUP);
  pinMode(REACT_BUTTON_P2_PIN, INPUT_PULLUP);
  pinMode(X_AXIS_UP_P1_PIN, INPUT_PULLUP);
  pinMode(X_AXIS_DOWN_P1_PIN, INPUT_PULLUP);
  pinMode(Y_AXIS_LEFT_P1_PIN, INPUT_PULLUP);
  pinMode(Y_AXIS_RIGHT_P1_PIN, INPUT_PULLUP);
  pinMode(X_AXIS_UP_P2_PIN, INPUT_PULLUP);
  pinMode(X_AXIS_DOWN_P2_PIN, INPUT_PULLUP);
  pinMode(Y_AXIS_LEFT_P2_PIN, INPUT_PULLUP);
  pinMode(Y_AXIS_RIGHT_P2_PIN, INPUT_PULLUP);
  pinMode(P1_GOAL_PIN, INPUT);
  pinMode(P2_GOAL_PIN, INPUT);

  attachInterrupt(P1_GOAL_PIN, DetectGoalP1, FALLING);
  attachInterrupt(P2_GOAL_PIN, DetectGoalP2, FALLING);
  


  xServo.attach(X_SERVO_PIN);  
  yServo.attach(Y_SERVO_PIN);

  xServo.write(DEFAULT_X_ANGLE);
  yServo.write(DEFAULT_Y_ANGLE);

}

//Main Loop
void loop() {
  pollReactButtons();
  inMatch = true;
  if(!inMatch && checkMatchStartCondition()){ //checks if not in a match and players are holding down react
    //indicate match start - buzzer/led
    Serial.println("Start Match");
    delay(500);
    inMatch = true;
    
    reactP1State = false;
    reactP2State = false;
  }

  while(inMatch){
    pollReactButtons();
    inGame = true;

    if(!inGame && checkGameStartCondition()){//checks if not in a game and players have pressed their react
      inGame = true;
      Serial.println("Start Game");
      //Serial.println("In Game");
    } 

    while(inGame){
      //gameloop
      currentTime = millis();
      pollMoveButtons();
      pollReactButtons();
      if(currentTime-lastServoUpdate > SERVO_UPDATE_PERIOD_MS){
        currentXAngle = currentXAngle + DEG_PER_PRESS*(p1PressesX - p2PressesX);
        currentYAngle = currentYAngle + DEG_PER_PRESS*(p1PressesY - p2PressesY);

        // Print registered presses
        Serial.print("P1 X: ");
        Serial.print(p1PressesX);
        Serial.print(" | P1 Y: ");
        Serial.print(p1PressesY);
        Serial.print(" | P2 X: ");
        Serial.print(p2PressesX);
        Serial.print(" | P2 Y: ");
        Serial.println(p2PressesY);

        if(currentXAngle > DEFAULT_X_ANGLE +20){
          currentXAngle = DEFAULT_X_ANGLE +20;
        }else if(currentXAngle < DEFAULT_X_ANGLE -20){
          currentXAngle = DEFAULT_X_ANGLE -20;
        }

        if(currentYAngle > DEFAULT_Y_ANGLE +20){
          currentYAngle = DEFAULT_Y_ANGLE +20;
        }else if(currentYAngle < DEFAULT_Y_ANGLE -20){
          currentYAngle = DEFAULT_Y_ANGLE -20;
        }

        xServo.write(currentXAngle);
        yServo.write(currentYAngle);

        p1PressesX = 0;
        p1PressesY = 0;
        p2PressesX = 0;
        p2PressesY = 0;

        lastServoUpdate = currentTime;
      }

        // Goal 1 is scored in
      if (p1GoalScored && (currentTime- lastDebounceGoalP1 > GOAL_DEBOUNCE_MS)) {

        Serial.println("Goal 1 ");
        goalCountP1++;
        lastDebounceGoalP1 = currentTime; // Update debounce time
      }

      // Goal 2 is scored in
      if (p2GoalScored && (currentTime - lastDebounceGoalP2 > GOAL_DEBOUNCE_MS)) {

        Serial.println("Goal 2 ");
        goalCountP2 ++;
        lastDebounceGoalP2 = currentTime; // Update debounce time
      }

      
      if(p1GoalScored || p2GoalScored){

        inGame = false;
        p2GoalScored = false;
        p1GoalScored = false;

        while(currentXAngle != DEFAULT_X_ANGLE || currentYAngle != DEFAULT_Y_ANGLE){
          if(currentXAngle != DEFAULT_X_ANGLE)
          { 
            currentXAngle =  currentXAngle + (DEFAULT_X_ANGLE - currentXAngle) / abs((DEFAULT_X_ANGLE - currentXAngle));
          }
          if(currentYAngle != DEFAULT_Y_ANGLE){
            currentYAngle =  currentYAngle + (DEFAULT_Y_ANGLE - currentYAngle) / abs((DEFAULT_Y_ANGLE - currentYAngle));
          }
          delay(20);
        }

        if(goalCountP1 == GOALS_PER_MATCH || goalCountP2 == GOALS_PER_MATCH){
          Serial.println("End Game");
          xServo.write(DEFAULT_X_ANGLE);
          yServo.write(DEFAULT_Y_ANGLE);
          //initiate game end condition - can be blocking code
          goalCountP1 = 0;
          goalCountP2 = 0;
          
          inMatch = false;
        }
      }
    }
    
  }

}

void pollReactButtons() {
  bool p1State = digitalRead(REACT_BUTTON_P1_PIN) == LOW;
  bool p2State = digitalRead(REACT_BUTTON_P2_PIN) == LOW;

  if (p1State && (millis() - lastDebounceP1React > DEBOUNCE_PERIOD_MS)) {
    reactP1State = true;
    lastDebounceP1React = millis();
    //Serial.println("React P1");
  }

  if (p2State && (millis() - lastDebounceP2React > 500)) {
    reactP2State = true;
    lastDebounceP2React = millis();
    //Serial.println("React P2");
  }
}

void pollMoveButtons() {
  bool currP1Up = digitalRead(X_AXIS_UP_P1_PIN);
  bool currP1Down = digitalRead(X_AXIS_DOWN_P1_PIN);
  bool currP1Left = digitalRead(Y_AXIS_LEFT_P1_PIN);
  bool currP1Right = digitalRead(Y_AXIS_RIGHT_P1_PIN);
  bool currP2Up = digitalRead(X_AXIS_UP_P2_PIN);
  bool currP2Down = digitalRead(X_AXIS_DOWN_P2_PIN);
  bool currP2Left = digitalRead(Y_AXIS_LEFT_P2_PIN);
  bool currP2Right = digitalRead(Y_AXIS_RIGHT_P2_PIN);

  unsigned long now = millis();

  if (prevP1Up == HIGH && currP1Up == LOW && now - lastDebounceP1Up > DEBOUNCE_PERIOD_MS) {
    p1PressesX += 1;
    lastDebounceP1Up = now;
  }
  if (prevP1Down == HIGH && currP1Down == LOW && now - lastDebounceP1Down > DEBOUNCE_PERIOD_MS) {
    p1PressesX -= 1;
    lastDebounceP1Down = now;
  }
  if (prevP1Left == HIGH && currP1Left == LOW && now - lastDebounceP1Left > DEBOUNCE_PERIOD_MS) {
    p1PressesY += 1;
    lastDebounceP1Left = now;
  }
  if (prevP1Right == HIGH && currP1Right == LOW && now - lastDebounceP1Right > DEBOUNCE_PERIOD_MS) {
    p1PressesY -= 1;
    lastDebounceP1Right = now;
  }

  if (prevP2Up == HIGH && currP2Up == LOW && now - lastDebounceP2Up > DEBOUNCE_PERIOD_MS) {
    p2PressesX += 1;
    lastDebounceP2Up = now;
  }
  if (prevP2Down == HIGH && currP2Down == LOW && now - lastDebounceP2Down > DEBOUNCE_PERIOD_MS) {
    p2PressesX -= 1;
    lastDebounceP2Down = now;
  }
  if (prevP2Left == HIGH && currP2Left == LOW && now - lastDebounceP2Left > DEBOUNCE_PERIOD_MS) {
    p2PressesY += 1;
    lastDebounceP2Left = now;
  }
  if (prevP2Right == HIGH && currP2Right == LOW && now - lastDebounceP2Right > DEBOUNCE_PERIOD_MS) {
    p2PressesY -= 1;
    lastDebounceP2Right = now;
  }

  // Update previous states
  prevP1Up = currP1Up;
  prevP1Down = currP1Down;
  prevP1Left = currP1Left;
  prevP1Right = currP1Right;

  prevP2Up = currP2Up;
  prevP2Down = currP2Down;
  prevP2Left = currP2Left;
  prevP2Right = currP2Right;
}



bool checkMatchStartCondition(void){
  bool startGame = false; 

  bool p1ReactHigh = digitalRead(REACT_BUTTON_P1_PIN) == LOW;
  bool p2ReactHigh = digitalRead(REACT_BUTTON_P2_PIN) == LOW;

  if(p2ReactHigh == p1ReactHigh){
    startGame = true;
  }
  
  return startGame;
}

bool checkGameStartCondition(void){
  bool startGame = false; 

  if(reactP1State && printP1ReadyOnce){
    Serial.println("P1 ready");
    printP1ReadyOnce = false;
  }
  if(reactP2State && printP2ReadyOnce){
    Serial.println("P2 ready");
    printP2ReadyOnce = false;
  }

  if(reactP1State && reactP2State){
    reactP1State = false;
    reactP2State = false;
    printP1ReadyOnce = true;
    printP2ReadyOnce = true;

    return true;
  }else{
    return false;
  }


  return startGame;
}


void ledControl(void){
  //if react high and Game but not match set power led high
  //if powerup active set powerup led high
}