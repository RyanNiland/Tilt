//Includes
#include <ESP32Servo.h>


//Global Variables
Servo myServo;  // create servo object
volatile bool button1Flag = false;
volatile bool button2Flag = false;
int count = 40;
unsigned long lastDebounceTime1 = 0;
unsigned long lastDebounceTime2 = 0;
const unsigned long debounceDelay = 25; // 100 ms debounce
//Function Initiations
void IRAM_ATTR handleButton1();
void IRAM_ATTR handleButton2();

//Set-up of pins
void setup() {
  // put your setup code here, to run once:n
  Serial.begin(115200); // Initialize Serial for debugging
  myServo.attach(21);  // attach servo signal wire to pin 9
  pinMode(26, INPUT_PULLUP); //Button initialization
  attachInterrupt(digitalPinToInterrupt(26), handleButton1, FALLING); //interrupt link to button
  pinMode(25, INPUT_PULLUP); //Button initialization
  attachInterrupt(digitalPinToInterrupt(25), handleButton2, FALLING); //interrupt link to butto
  pinMode(18, OUTPUT);
  myServo.write(count);
}

void IRAM_ATTR handleButton1() {  
  button1Flag = true;
}

void IRAM_ATTR handleButton2() {  
  button2Flag = true;
}
//Main Loop
void loop() {
  // put your main code here, to run repeatedly:
  unsigned long currentTime1 = millis();
  if(button1Flag&&(currentTime1 - lastDebounceTime1 > debounceDelay)){
  count+=2;
          if(count>60){
      count = 60;
    }
    myServo.write(count);
    digitalWrite(18, HIGH);
    button2Flag = false;
    button1Flag = false;
    Serial.println("Button 1 pressed: LED ON");
    lastDebounceTime1 = currentTime1;
  }
 unsigned long currentTime2 = millis();
if(button2Flag&&(currentTime2 - lastDebounceTime2 > debounceDelay)){
  count-=2;
  if(count<20){
    count = 20;
  }
  myServo.write(count);
  digitalWrite(18,LOW);
  button1Flag = false;
  button2Flag = false;
  Serial.println("Button 2 pressed: LED OFF");
  lastDebounceTime2 = currentTime2;
}
  
  // if(button1Flag&&(currentTime - lastDebounceTime > debounceDelay)){
  //   if(count>=90){
  //     count = 0;
  //   }
  //   count+=5;
  //   myServo.write(count);
  //   digitalWrite(18, HIGH);
  //   button1Flag = false;
  //   lastDebounceTime = currentTime;
  // }else if(!button1Flag && (currentTime - lastDebounceTime > 1000)){
  //   digitalWrite(18, LOW);
  // }
}
