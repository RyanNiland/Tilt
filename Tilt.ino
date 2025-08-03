//Includes
#include <ESP32Servo.h>

//defines
#define REACT_BUTTON_P1_PIN 19
#define REACT_BUTTON_P2_PIN 35

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
#define DEFAULT_Y_ANGLE 45

#define DEG_PER_PRESS 4

#define DEBOUNCE_PERIOD_MS 20
//Global Variables
bool inGame = false; // individual point
bool inMatch = false; // whole play

bool reactP1State = false;
bool reactP2State = false;

bool p1GoalScored = false;
bool p2GoalScored = false;

uint8_t goalCountP1 = 0;
uint8_t goalCountP2 = 0;

uint32_t p1PressesX = 0;
uint32_t p1PressesY = 0;
uint32_t p2PressesX = 0;
uint32_t p2PressesY = 0;

Servo xServo;
Servo yServo;

unsigned long lastServoUpdate = 0;

unsigned long lastP1UpPress = 0;
unsigned long lastP1DownPress = 0;
unsigned long lastP1LeftPress = 0;
unsigned long lastP1RightPress = 0;
unsigned long lastP1ReactPress = 0;

unsigned long lastP2UpPress = 0;
unsigned long lastP2DownPress = 0;
unsigned long lastP2LeftPress = 0;
unsigned long lastP2RightPress = 0;
unsigned long lastP2ReactPress = 0;


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
int currentYANgle = DEFAULT_Y_ANGLE;



//ISRs
void IRAM_ATTR reactButtonPressP1() {
  if(millis() - lastP1ReactPress > DEBOUNCE_PERIOD_MS){
    reactP1State = true;
    Serial.println("React P1");
    lastP1ReactPress = millis();
  }

}

void IRAM_ATTR reactButtonPressP2() {
  if(millis() - lastP2ReactPress > DEBOUNCE_PERIOD_MS){
    reactP2State = true;
    lastP2ReactPress = millis();
  }
  
}

void IRAM_ATTR upX_P1() {
  if(millis() - lastP1UpPress > DEBOUNCE_PERIOD_MS){
    p1PressesX += 1;
    Serial.println("Up P1");
    lastP1UpPress = millis();
  }
  
}
void IRAM_ATTR downX_P1() {
  if(millis() - lastP1DownPress > DEBOUNCE_PERIOD_MS){
    p1PressesX -= 1;
    Serial.println("Down P1");
    lastP1DownPress = millis();
  }
  

}
void IRAM_ATTR leftY_P1() {
  if(millis() - lastP1LeftPress > DEBOUNCE_PERIOD_MS){
    p1PressesY +=1;
    Serial.println("Left P1");
    lastP1LeftPress = millis();
  }
  

}
void IRAM_ATTR rightY_P1() {
  if(millis() - lastP1RightPress > DEBOUNCE_PERIOD_MS){
    p1PressesY -=1;
    Serial.println("Right P1");
    lastP1RightPress = millis();
  } 
}
void IRAM_ATTR upX_P2() {
  if(millis() - lastP2UpPress > DEBOUNCE_PERIOD_MS){
    p2PressesX += 1;
    Serial.println("Up P2");
    lastP2UpPress = millis();
  }
  
}
void IRAM_ATTR downX_P2() {
  if(millis() - lastP2DownPress > DEBOUNCE_PERIOD_MS){
    p2PressesX -= 1;
    Serial.println("Down P2");
    lastP2DownPress = millis();
  }
  
}
void IRAM_ATTR leftY_P2() {
  if(millis() - lastP2LeftPress > DEBOUNCE_PERIOD_MS){
    p2PressesY +=1;
    Serial.println("Left P2");
    lastP2LeftPress = millis();
  }
  
}
void IRAM_ATTR rightY_P2() {
  if(millis() - lastP2RightPress > DEBOUNCE_PERIOD_MS){
    p2PressesY -=1
    Serial.println("Right P2");
    lastP2RightPress = millis();
  }
}

//Set-up of pins
void setup() {
  Serial.begin(115200);
  delay(1000);
  
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
  

  //attatch ISRs to buttons
 attachInterrupt(digitalPinToInterrupt(REACT_BUTTON_P1_PIN), reactButtonPressP1, RISING);
  attachInterrupt(digitalPinToInterrupt(REACT_BUTTON_P2_PIN), reactButtonPressP2, RISING);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_UP_P1_PIN), upX_P1, RISING);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_DOWN_P1_PIN),downX_P1, RISING);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_LEFT_P1_PIN), leftY_P1, RISING);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_RIGHT_P1_PIN), rightY_P1, RISING);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_UP_P2_PIN), upX_P2, RISING);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_DOWN_P2_PIN), downX_P2, RISING);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_LEFT_P2_PIN), leftY_P2, RISING);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_RIGHT_P2_PIN), rightY_P2, RISING);

  xServo.attach(X_SERVO_PIN);  
  yServo.attatch(Y_SERVO_PIN);

  xServo.write(DEFAULT_X_ANGLE);
  yServo.write(DEFAULT_Y_ANGLE);

}

//Main Loop
void loop() {
  
  if(!inMatch && checkMatchStartCondition){ //checks if not in a match and players are holding down react
    //indicate match start - buzzer/led
    Serial.println("Start Match");
    delay(500);
    inMatch = true;
    
    reactP1State = false;
    reactP2State = false;
  }

  while(inMatch){

    if(!inGame && checkGameStartCondition){//checks if not in a game and players have pressed their react
      inGame = true;
      //Serial.println("In Game");
    } 

    while(inGame){
      //gameloop

      if(millis()-lastServoUpdate > SERVO_UPDATE_PERIOD_MS){
        currentXAngle = currentXAngle + DEG_PER_PRESS*(p1PressesX - p2PressesX);
        currentYANgle = currentYANgle + DEG_PER_PRESS*(p1PressesY - p2PressesY);

        // Print registered presses
        Serial.print("P1 X: ");
        Serial.print(p1PressesX);
        Serial.print(" | P1 Y: ");
        Serial.print(p1PressesY);
        Serial.print(" | P2 X: ");
        Serial.print(p2PressesX);
        Serial.print(" | P2 Y: ");
        Serial.println(p2PressesY);

        xServo.write(currentXAngle);
        yServo.write(currentYANgle;

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

  if(reactP1State && reactP2State){
    reactP1State = false;
    reactP2State = false;
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