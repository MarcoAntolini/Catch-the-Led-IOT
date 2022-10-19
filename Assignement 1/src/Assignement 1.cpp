#include <Arduino.h>
#include <EnableInterrupt.h>
#include <avr/sleep.h>
#include "Assignement 1.h"

char leds[4];
int ledsON[4];
char buttons[4];
int bouncing[4];
enum gameState state;
int intensity = 0;
int fadingFactor = 5;
float factor;
long t1;
long t2;
long t3;
int errors;
int score;
int i = 0;
long temp;

// void initialize() {
  
// }

// void executeLoop() {
  
// }

void startUp() {
  if (i == 0) {
    Serial.println("Welcome to the Catch the Led Pattern Game. Press Key T1 to Start.");
    enableInterrupt(buttons[0], setupDifficulty, RISING);
  }
  if (i <= 10000) {
    analogWrite(LED_RED, intensity);
    intensity = intensity + fadingFactor;
    if (intensity == 0 || intensity == 255) {
      fadingFactor = -fadingFactor;
    }
    i = i + START_DELAY;
    delay(START_DELAY);
  }
  if (i > 10000) {
    i = 0;
    digitalWrite(LED_RED, LOW);
    state = SLEEP;
    delay(START_DELAY);
  }
  t2 = T2;
  t3 = T3;
  errors = 0;
  score = 0;
}

void goSleep() {
  for (int i = 0; i < MAX_LEDS; i++) {
    enableInterrupt(buttons[i], wakeUp, RISING);
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void wakeUp() {
  sleep_disable();
  for (int i = 0; i < MAX_LEDS; i++) {
    disableInterrupt(buttons[i]);
  }
  state = START;
}

void setupDifficulty() {
  digitalWrite(LED_RED, LOW);
  int potLevel = analogRead(POT);
  switch (potLevel) {
    case 0 ... 256:
      factor = FACTOR1;
      Serial.println("Current difficulty: 1");
      break;
    case 257 ... 512:
      factor = FACTOR2;
      Serial.println("Current difficulty: 2");
      break;
    case 513 ... 768:
      factor = FACTOR3;
      Serial.println("Current difficulty: 3");
      break;
    case 769 ... 1024:
      factor = FACTOR4;
      Serial.println("Current difficulty: 4");
      break;
    default:
      break;
  }
  t1 = random(5000);
  state = WAIT;
  i = 0;
  disableInterrupt(buttons[0]);
}

bool sumCheck(int array[]) {
  int sum = 0;
  for (int i = 0; i < MAX_LEDS; i++) {
    sum += array[i];
  }
  if (sum == 0) {
    return false;
  }
  return true;
}

void randomizeLeds() {
  for (int i = 0; i < MAX_LEDS; i++) {
    ledsON[i] = random(2);
  }
  if (!sumCheck(ledsON)) {
    randomizeLeds();
  }
}

void showPattern() {
  if (i == 0) {
    delay(t1);
    randomizeLeds();
    for (int i = 0; i < MAX_LEDS; i++) {
      digitalWrite(leds[i], ledsON[i] == 1 ? HIGH : LOW);
    }
    i = 1;
    temp = millis();
  }
  for (int i = 0; i < MAX_LEDS; i++) {
    if (digitalRead(buttons[i]) == HIGH)
      error();
  }
  if (millis() - temp > t2) {
    for (int i = 0; i < MAX_LEDS; i++) {
      digitalWrite(leds[i], LOW);
      bouncing[i] = 0;
    }
    state = PLAY;
    delay(START_DELAY);
    temp = millis();
  }
}

void startGuessing() {
  for (int i = 0; i < MAX_LEDS; i++) {
    if (digitalRead(buttons[i]) == HIGH && bouncing[i] == 0) {
      if (ledsON[i] == 1) {
        ledsON[i] = 0;
        bouncing[i] = 1;
        digitalWrite(leds[i], HIGH);
        if (!sumCheck(ledsON)) {
          delay(1000);
          win();
        }
      } else {
        error();
      }
    }
  }
}

void error() {
  for (int i = 0; i < MAX_LEDS; i++) {
    disableInterrupt(buttons[i]);
  }
  state = ERROR;
}

void checkError() {
  for (int i = 0; i < MAX_LEDS; i++) {
    digitalWrite(leds[i], LOW);
  }
  errors++;
  if (errors < 3) {
    Serial.println("Penalty!!!");
    state = WAIT;
  } else {
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

void win() {
  for (int i = 0; i < MAX_LEDS; i++) {
    digitalWrite(leds[i], LOW);
  }
  score++;
  Serial.print("New point! Score: ");
  Serial.println(score);
  t2 = (t2 * factor < MIN_T) ? MIN_T : t2 * factor;
  t3 = (t3 * factor < MIN_T) ? MIN_T : t3 * factor;
  state = WAIT;
  i = 0;
  delay(START_DELAY);
}