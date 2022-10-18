#ifndef __ASSIGNEMENT_1_H__
#define __ASSIGNEMENT_1_H__

#include <Arduino.h>

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
#define START 0
#define SLEEP 1
#define WAIT 2
#define PLAY 3
#define ERROR 4
#define MAX_LEDS 4
#define MAX_ERROR 3
#define FACTOR1 0.9
#define FACTOR2 0.8
#define FACTOR3 0.7
#define FACTOR4 0.6
#define START_DELAY 20
#define T2 5000
#define T3 10000
#define MIN_T 100

void initialize();

void executeLoop();

void startUp();

void goSleep();

void wakeUp();

void setupDifficulty();

bool sumCheck(int array[]);

void randomizeLeds();

void showPattern();

void startGuessing();

void error();

void checkError();

void win();

#endif