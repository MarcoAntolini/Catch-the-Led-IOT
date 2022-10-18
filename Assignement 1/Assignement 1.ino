#include "Assignement 1.h"

char leds[4];
int ledsON[4];
char buttons[4];
int bouncing[4];
int state;
int intensity = 0;
int fadingFactor = 5;
float factor;
long t1;
long t2 = T2;
long t3 = T3;
int errors = 0;
int score = 0;
int i = 0;
long temp;

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
  for (int i = 0; i < MAX_LEDS; i++)
  {
    pinMode(leds[i], OUTPUT);
  }
  for (int i = 0; i < MAX_LEDS; i++)
  {
    pinMode(buttons[i], INPUT);
  }
  pinMode(LED_RED, OUTPUT);
  Serial.begin(9600);
  state = START;
}

void loop()
{
  switch (state)
  {
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
    if (millis() - temp < t3)
    {
      startGuessing();
    }
    else
    {
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