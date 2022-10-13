#include <EnableInterrupt.h>
#include "TimerOne.h"
#include <avr/sleep.h>

#define LED1 12
#define LED2 11
#define LED3 10
#define LED4 8
#define LED_RED 9
#define BUTTON1 7
#define BUTTON2 6
#define BUTTON3 5
#define BUTTON4 4
#define POT A0

unsigned char leds[4];
unsigned int ledsON[4];
unsigned char buttons[4];
int maxLeds = 4;

#define START 0
#define SLEEP 1
#define WAIT 2
#define PLAY 3
int state;

#define START_DELAY 20
int intensity = 0;
int fadingFactor = 5;

#define FACTOR1 9/10
#define FACTOR2 8/10
#define FACTOR3 7/10
#define FACTOR4 6/10
float factor;

#define T2 5000
#define T3 10000
float t1;
float t2 = T2;
float t3 = T3;

#define MAX_ERROR 3
int errors = 0;
int score = 0;

#define BOUNCING_DELAY 25
int bouncing1 = -1;
int bouncing2 = -1;
int bouncing3 = -1;
int bouncing4 = -1;

void error(){
  errors++;
  if(errors<3){
    Serial.println("Penalty!!!");
    state = WAIT;
  }else{
    Serial.print("Game Over. Final score: ");
    Serial.println(score);
    state = START;
    score = 0;
    errors = 0;
  }
  digitalWrite(LED_RED, HIGH);
  delay(1000);
  digitalWrite(LED_RED, LOW);
}

void setupDifficulty(){
  digitalWrite(LED_RED, LOW);
  int potLevel = analogRead(POT);
  switch(potLevel){
    case 0 ... 256:
      factor = FACTOR1;
      break;
    case 257 ... 512:
      factor = FACTOR2;
      break;
    case 513 ... 768:
      factor = FACTOR3;
      break;
    case 769 ... 1024:
      factor = FACTOR4;
      break;
    default:
      break;
  }
  t1 = random(10000);
  state = WAIT;
  Serial.println(state);
  disableInterrupt(buttons[0]);
}

void startUp(){
  Serial.println("Welcome to the Catch the Led Pattern Game. Press Key T1 to Start.");
  enableInterrupt(buttons[0], setupDifficulty, RISING);
  int i = 0;
  while(i < 10000){
    analogWrite(LED_RED, intensity);
    intensity = intensity + fadingFactor;
    if (intensity == 0 || intensity == 255){ 
      fadingFactor = -fadingFactor ;
    }
    i = i + START_DELAY;
    delay(START_DELAY); 
  }
  digitalWrite(LED_RED, LOW);
  state = SLEEP;
}

void setup()
{
  leds[0] = LED1;
  leds[1] = LED2;
  leds[2] = LED3;
  leds[3] = LED4;
  buttons[0] = BUTTON1;
  buttons[1] = BUTTON2;
  buttons[2] = BUTTON3;
  buttons[3] = BUTTON4;
  for (int i = 0; i < maxLeds; i++){
    pinMode(leds[i], OUTPUT);  
  }
  for (int i = 0; i < maxLeds; i++){
    pinMode(buttons[i], INPUT);  
  }
  pinMode(LED_RED, OUTPUT);
  Serial.begin(9600);
  state = START;
}

bool sumCheck(int array[])
{
  int sum = 0;
  for (int i = 0; i < maxLeds; i++) {
    sum += array[i];
  }
  if (sum == 0) {
    return false;
  }
  return true;
}

void randomizeLeds()
{
  for (int i = 0; i < maxLeds; i++){
    ledsON[i] = random(2);
    Serial.println(ledsON[i]);
  }
  if (!sumCheck(ledsON)) {
    randomizeLeds();
  }
}

void showPattern(){
  for (int i = 0; i < maxLeds; i++){
    enableInterrupt(buttons[i], error, RISING);
  }
  //delay(t1);
  randomizeLeds();
  for (int i = 0; i < maxLeds; i++){
    digitalWrite(leds[i], ledsON[i] == 1 ? HIGH : LOW);
  }
  delay(t2);
  for (int i = 0; i < maxLeds; i++){
    digitalWrite(leds[i], LOW);  
  }
  for (int i = 0; i < maxLeds; i++){
    disableInterrupt(buttons[i]);
  }
  state = PLAY;
  Timer1.initialize(t3*1000);
  Timer1.attachInterrupt(error);
}

void wakeUp(){
  sleep_disable();
  for (int i = 0; i < maxLeds; i++){
    disableInterrupt(buttons[i]);
  }
  state = START;
}

void goSleep(){
  for (int i = 0; i < maxLeds; i++){
    enableInterrupt(buttons[i], wakeUp, RISING);
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void win(){
  for (int i = 0; i < maxLeds; i++){
    digitalWrite(leds[i], LOW);
  }
  score++;
  Serial.print("New point! Score: ");
  Serial.println(score);
  t2 = t2 * factor;
  t3 = t3 * factor;
  state = WAIT;
}

bool bouncingCheck(int button){
  switch(button) {
    case 0:
      if(bouncing1 < BOUNCING_DELAY){
        bouncing1 = millis();
        return true;
      }else{
        return false;
      }
      break;
    case 1:
      if(bouncing2 < BOUNCING_DELAY){
        bouncing2 = millis();
        return true;
      }else{
        return false;
      }
      break;
    case 2:
      if(bouncing3 < BOUNCING_DELAY){
        bouncing3 = millis();
        return true;
      }else{
        return false;
      }
      break;
    case 3:
      if(bouncing4 < BOUNCING_DELAY){
        bouncing4 = millis();
        return true;
      }else{
        return false;
      }
      break;
    default:
      break;
  }
}

void startGuessing(){
  for (int i = 0; i < maxLeds; i++){
    if(digitalRead(buttons[i]) == HIGH){
      if(bouncingCheck(i)){
        if(ledsON[i] == 1){
          ledsON[i] = 0;
          digitalWrite(leds[i], HIGH);
          if(!sumCheck(ledsON)){
            win();
          }
        }else{
          error();
        }
      }
      
    }
  }
}

void loop()
{
  switch(state){
    case START:
      startUp();
      break;
    case SLEEP:
      goSleep();
      break;
    case WAIT:
      showPattern();
      break;
    case PLAY:
      startGuessing();
      break;
    default:
      break;
  }
}