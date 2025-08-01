bool goalScored = false;  //Global boolean to determine if a goal has been scored
//line above needs to go in code before calling gameplay()

int y_p1, y_p2;  //y button presses for player 1(p1) and player 2 (p2)
int x_p1, x_p2;  //x button presses for player 1(p1) and player 2 (p2)

unsigned long powerUpStartTime = 0;  //Global variable that sets the time when powerUp starts
unsigned long powerUpStopTime = 0;   //GLobal variable that sets the time when powerUp ended

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  
  while (!goalScored) {                //While Game is running
    if (millis() > angleUpdateTime) {  //Checking if there has been sufficient time since the angle was updated
      //Placeholder: Check states of Power ups
      int x = x_p1 + x_p2;
      int y = y_p1 + y_p2;
      //Placeholder: Update Angle for the servos using variable x and y
      x_p1 = 0;
      x_p2 = 0;
      y_p1 = 0;
      y_p2 = 0;

      unsigned long angleUpdateTime = 0;  //Variable setting time for angle updates
    }
  }
}
