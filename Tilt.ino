//Includes

//defines
#define REACT_BUTTON_P1_PIN 34
#define REACT_BUTTON_P2_PIN 35

#define X_AXIS_UP_P1_PIN 12
#define X_AXIS_DOWN_P1_PIN 13
#define Y_AXIS_LEFT_P1_PIN 14
#define Y_AXIS_RIGHT_P1_PIN 15
#define X_AXIS_UP_P2_PIN 25
#define X_AXIS_DOWN_P2_PIN 26
#define Y_AXIS_LEFT_P2_PIN 27
#define Y_AXIS_RIGHT_P2_PIN 32



#define GOALS_PER_MATCH 3
//Global Variables
bool inGame = false; // individual point
bool inMatch = false; // whole play

bool reactP1State = false;
bool reactP2State = false;

bool goalScored = false;
uint8_t goalCountP1 = 0;
uint8_t goalCountP2 = 0;

uint32_t p1PressesX = 0;
uint32_t p1PressesY = 0;
uint32_t p2PressesX = 0;
uint32_t p2PressesY = 0;

//Function Initiations

//ISRs
void IRAM_ATTR reactButtonPressP1() {
  reactP1State = true;
}

void IRAM_ATTR reactButtonPressP2() {
  reactP2State = true;
}

void IRAM_ATTR upX_P1() {
  p1PressesX += 1;
}
void IRAM_ATTR downX_P1() {
  p1PressesX -= 1;
}
void IRAM_ATTR leftY_P1() {
  p1PressesY +=1;
}
void IRAM_ATTR rightY_P1() {
  p1PressesY -=1;
}
void IRAM_ATTR upX_P2() {
  p2PressesX += 1;
}
void IRAM_ATTR downX_P2() {
  p2PressesX -= 1;
}
void IRAM_ATTR leftY_P2() {
  p2PressesY +=1;
}
void IRAM_ATTR rightY_P2() {
  p2PressesY -=1;
}

//Set-up of pins
void setup() {
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
  attachInterrupt(digitalPinToInterrupt(REACT_BUTTON_P1_PIN), reactButtonPressP1, Rising);
  attachInterrupt(digitalPinToInterrupt(REACT_BUTTON_P2_PIN), reactButtonPressP2, Rising);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_UP_P1_PIN), upX_P1, Rising);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_DOWN_P1_PIN),downX_P1, Rising);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_LEFT_P1_PIN), leftY_P1, Rising);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_RIGHT_P1_PIN), rightY_P1, Rising);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_UP_P2_PIN), upX_P2, Rising);
  attachInterrupt(digitalPinToInterrupt(X_AXIS_DOWN_P2_PIN), downX_P2, Rising);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_LEFT_P2_PIN), leftY_P2, Rising);
  attachInterrupt(digitalPinToInterrupt(Y_AXIS_RIGHT_P2_PIN), rightY_P2, Rising);

}

//Main Loop
void loop() {
  
  if(!inMatch && checkMatchStartCondition){ //checks if not in a match and players are holding down react
    //indicate match start - buzzer/led
    delay(1000);
    inMatch = true;

    reactP1State = false;
    reactP2State = false;
  }

  while(inMatch){

    if(!inGame && checkGameStartCondition){//checks if not in a game and players have pressed their react
      inGame = true;
    } 

    while(inGame){
      //gameloop

      if(goalScored){

        inGame = false;

        if(goalCountP1 == GOALS_PER_MATCH || goalCountP2 == GOALS_PER_MATCH){
          
          //initiate game end condition - can be blocking code
          inMatch = false;
        }
      }
    }
    
  }

}



bool checkMatchStartCondition{
  bool startGame = false; 

  bool p1ReactHigh = digitalRead(REACT_BUTTON_P1_PIN) == HIGH;
  bool p2ReactHigh = digitalRead(REACT_BUTTON_P2_PIN) == HIGH;

  if(p2ReactHigh == p1ReactHigh){
    startGame = true;
  }
  
  return startGame;
}

bool checkGameStartCondition{
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


void ledControl(){
  //if react high and Game but not match set power led high
  //if powerup active set powerup led high
}