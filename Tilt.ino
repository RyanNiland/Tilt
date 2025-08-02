//Includes
#include <ESP32Servo.h>


//Global Variables
Servo myServo;  // create servo object
volatile bool button1Flag = false;
int count = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 100; // 100 ms debounce
//Function Initiations
void IRAM_ATTR handleButton1();

//Set-up of pins
void setup() {
  // put your setup code here, to run once:n
  myServo.attach(21);  // attach servo signal wire to pin 9
  pinMode(34, INPUT_PULLDOWN); //Button initialization
  attachInterrupt(digitalPinToInterrupt(34), handleButton1, RISING); //interrupt link to button
  pinMode(18, OUTPUT);
}

void IRAM_ATTR handleButton1() {
  button1Flag = true;
}

//Main Loop
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime = millis();
  if(button1Flag&&(currentTime - lastDebounceTime > debounceDelay)){
    if(count>=90){
      count = 0;
    }
    count+=5;
    myServo.write(count);
    digitalWrite(18, HIGH);
    button1Flag = false;
    lastDebounceTime = currentTime;
  }else if(!button1Flag && (currentTime - lastDebounceTime > 1000)){
    digitalWrite(18, LOW);
  }
}
