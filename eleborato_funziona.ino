#include <EnableInterrupt.h>
#include <avr/sleep.h>

#define LED1 13
#define LED2 12
#define LED3 11
#define LED4 8
#define LED_RED 3
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
#define ERROR 4
int state;

#define START_DELAY 20
int intensity = 0;
int fadingFactor = 5;

#define FACTOR1 0.9
#define FACTOR2 0.8
#define FACTOR3 0.7
#define FACTOR4 0.6
float factor;

#define T2 5000
#define T3 10000
long t1;
long t2 = T2;
long t3 = T3;

#define MAX_ERROR 3
int errors = 0;
int score = 0;

int bouncing[4];

int i = 0;
long a;

void error(){
  for (int i = 0; i < maxLeds; i++){
    disableInterrupt(buttons[i]);
  }
  state = ERROR;
}

void checkError(){
  for (int i = 0; i < maxLeds; i++){
    digitalWrite(leds[i], LOW);
  }
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
  i = 0;
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
  t1 = random(5000);
  state = WAIT;
  i = 0;
  disableInterrupt(buttons[0]);
}

void startUp(){
  if(i == 0){
    Serial.println("Welcome to the Catch the Led Pattern Game. Press Key T1 to Start.");
    enableInterrupt(buttons[0], setupDifficulty, RISING);
  }
  if(i <= 10000){
    analogWrite(LED_RED, intensity);
    intensity = intensity + fadingFactor;
    if (intensity == 0 || intensity == 255){ 
      fadingFactor = -fadingFactor ;
    }
    i = i + START_DELAY;
    delay(START_DELAY); 
  }
  if(i > 10000){
    i = 0;
    digitalWrite(LED_RED, LOW);
    state = SLEEP;
    delay(START_DELAY);
  }
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
  if(i == 0){
    delay(t1);
    randomizeLeds();
    for (int i = 0; i < maxLeds; i++){
      digitalWrite(leds[i], ledsON[i] == 1 ? HIGH : LOW);
    }
    i = 1;
    a = millis();
  }
  for(int i = 0; i < maxLeds; i++){
    if(digitalRead(buttons[i]) == HIGH) error();
  }
  if(millis()-a > t2){
    for (int i = 0; i < maxLeds; i++){
      digitalWrite(leds[i], LOW); 
      bouncing[i] = 0;
    }
    state = PLAY;
    delay(START_DELAY);
    a = millis();
  }
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
  i = 0;
  delay(START_DELAY);
}

void startGuessing(){
  for (int i = 0; i < maxLeds; i++){
    if(digitalRead(buttons[i]) == HIGH && bouncing[i]==0){
      if(ledsON[i] == 1){
        ledsON[i] = 0;
        bouncing[i] = 1;
        digitalWrite(leds[i], HIGH);
        if(!sumCheck(ledsON)){
          win();
        }
      }else{
        for(int i = 0; i < maxLeds; i++){
        Serial.println(ledsON[i]);}
        error();
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
      if(millis()-a < t3){
        startGuessing();
      }else{
        error();
      }
      break;
    case ERROR:
      checkError();
      break;
    default:
      break;
  }
}
