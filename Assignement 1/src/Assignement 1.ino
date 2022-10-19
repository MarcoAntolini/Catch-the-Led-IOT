/*
  Marco Antolini 0000977047
  Daniel Capannini 0000971194
  Roberto Sopranzetti 0000993972
*/

#include "Assignement 1.h"

extern char leds[4];
extern int ledsON[4];
extern char buttons[4];
extern int bouncing[4];
extern enum gameState state;
extern int intensity = 0;
extern int fadingFactor = 5;
extern float factor;
extern long t1;
extern long t2;
extern long t3;
extern int errors;
extern int score;
extern int i = 0;
extern long temp;

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
  for (int i = 0; i < MAX_LEDS; i++) {
    pinMode(leds[i], OUTPUT);
  }
  for (int i = 0; i < MAX_LEDS; i++) {
    pinMode(buttons[i], INPUT);
  }
  pinMode(LED_RED, OUTPUT);
  Serial.begin(9600);
  state = START;
}

void loop()
{
  switch (state) {
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
      if (millis() - temp < t3) {
        startGuessing();
      } else {
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
