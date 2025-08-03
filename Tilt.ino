//Includes
#include <ESP32Servo.h>

//defines
#define REACT_BUTTON_P1_PIN 19
#define REACT_BUTTON_P2_PIN 35

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

#define X_SERVO_PIN 21
#define Y_SERVO_PIN 22

#define SERVO_UPDATE_PERIOD_MS 500

#define GOALS_PER_MATCH 3

#define DEFAULT_X_ANGLE 45
#define DEFAULT_Y_ANGLE 55

#define DEG_PER_PRESS 4

#define DEBOUNCE_PERIOD_MS 20
//Global Variables
bool inGame = false; // individual point
bool inMatch = false; // whole play

volatile bool reactP1State = false;
volatile bool reactP2State = false;
bool printP1ReadyOnce = true;
bool printP2ReadyOnce = true;

bool p1GoalScored = false;
bool p2GoalScored = false;

uint8_t goalCountP1 = 0;
uint8_t goalCountP2 = 0;

volatile int p1PressesX = 0;
volatile int p1PressesY = 0;
volatile int p2PressesX = 0;
volatile int p2PressesY = 0;

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


//Function Initiations
void IRAM_ATTR reactButtonPressP1(void);
void IRAM_ATTR reactButtonPressP2(void);
void IRAM_ATTR upX_P1(void);
void IRAM_ATTR downX_P1(void);
void IRAM_ATTR leftY_P1(void);
void IRAM_ATTR rightY_P1(void);
void IRAM_ATTR upX_P2(void);
void IRAM_ATTR downX_P2(void);
void IRAM_ATTR leftY_P2(void);
void IRAM_ATTR rightY_P2(void);

bool checkMatchStartCondition(void);
bool checkGameStartCondition(void);

int currentXAngle = DEFAULT_X_ANGLE;
int currentYAngle = DEFAULT_Y_ANGLE;


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
  


  xServo.attach(X_SERVO_PIN);  
  yServo.attach(Y_SERVO_PIN);

  xServo.write(DEFAULT_X_ANGLE);
  yServo.write(DEFAULT_Y_ANGLE);

}

//Main Loop
void loop() {
  pollReactButtons();

  if(!inMatch && checkMatchStartCondition()){ //checks if not in a match and players are holding down react
    //indicate match start - buzzer/led
    Serial.println("Start Match");
    delay(500);
    inMatch = true;
    
    reactP1State = false;
    reactP2State = false;
  }

  while(inMatch){

    if(!inGame && checkGameStartCondition()){//checks if not in a game and players have pressed their react
      inGame = true;
      Serial.println("Start Game");
      //Serial.println("In Game");
    } 

    while(inGame){
      //gameloop
      pollMoveButtons();
      pollReactButtons();
      if(millis()-lastServoUpdate > SERVO_UPDATE_PERIOD_MS){
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

        lastServoUpdate = millis();
      }

      
      if(p1GoalScored || p2GoalScored){

        inGame = false;
        p2GoalScored = false;
        p1GoalScored = false;

        if(goalCountP1 == GOALS_PER_MATCH || goalCountP2 == GOALS_PER_MATCH){
          Serial.println("End Game");
          xServo.write(DEFAULT_X_ANGLE);
          yServo.write(DEFAULT_Y_ANGLE);
          //initiate game end condition - can be blocking code
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
  }

  if (p2State && (millis() - lastDebounceP2React > DEBOUNCE_PERIOD_MS)) {
    reactP2State = true;
    lastDebounceP2React = millis();
  }
}

void pollMoveButtons() {
  if (digitalRead(X_AXIS_UP_P1_PIN) == LOW && millis() - lastDebounceP1Up > DEBOUNCE_PERIOD_MS) {
    p1PressesX += 1;
    lastDebounceP1Up = millis();
  }

  if (digitalRead(X_AXIS_DOWN_P1_PIN) == LOW && millis() - lastDebounceP1Down > DEBOUNCE_PERIOD_MS) {
    p1PressesX -= 1;
    lastDebounceP1Down = millis();
  }

  if (digitalRead(Y_AXIS_LEFT_P1_PIN) == LOW && millis() - lastDebounceP1Left > DEBOUNCE_PERIOD_MS) {
    p1PressesY += 1;
    lastDebounceP1Left = millis();
  }

  if (digitalRead(Y_AXIS_RIGHT_P1_PIN) == LOW && millis() - lastDebounceP1Right > DEBOUNCE_PERIOD_MS) {
    p1PressesY -= 1;
    lastDebounceP1Right = millis();
  }

  if (digitalRead(X_AXIS_UP_P2_PIN) == LOW && millis() - lastDebounceP2Up > DEBOUNCE_PERIOD_MS) {
    p2PressesX += 1;
    lastDebounceP2Up = millis();
  }

  if (digitalRead(X_AXIS_DOWN_P2_PIN) == LOW && millis() - lastDebounceP2Down > DEBOUNCE_PERIOD_MS) {
    p2PressesX -= 1;
    lastDebounceP2Down = millis();
  }

  if (digitalRead(Y_AXIS_LEFT_P2_PIN) == LOW && millis() - lastDebounceP2Left > DEBOUNCE_PERIOD_MS) {
    p2PressesY += 1;
    lastDebounceP2Left = millis();
  }

  if (digitalRead(Y_AXIS_RIGHT_P2_PIN) == LOW && millis() - lastDebounceP2Right > DEBOUNCE_PERIOD_MS) {
    p2PressesY -= 1;
    lastDebounceP2Right = millis();
  }
}



bool checkMatchStartCondition(void){
  bool startGame = false; 

  bool p1ReactHigh = digitalRead(REACT_BUTTON_P1_PIN) == HIGH;
  bool p2ReactHigh = digitalRead(REACT_BUTTON_P2_PIN) == HIGH;

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