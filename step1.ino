#include <M5Stack.h>

uint8_t mm = 0, ss = 0;
uint32_t startedAt = 0, targetAt = 0;

void setup() {
  M5.begin();

  resetTimer();
}

void loop() {
  M5.update();

  if (M5.BtnA.wasReleased()) {
    if (isStarted()) {
      stopTimer();
    } else {
      startTimer();
    }
  } else if (M5.BtnB.wasReleased()) {
    resetTimer();
  }

  if (isStarted()) {
    tickTock();
  }
}

void resetTimer() {
  ss = 0;
  mm = 0;
  drawTimer();
}

void drawTimer() {
  int xpos = 0;
  int ypos = 85;
  if (mm < 10) {
    xpos += M5.Lcd.drawChar('0', xpos, ypos, 8);
  }
  xpos += M5.Lcd.drawNumber(mm, xpos, ypos, 8);
  xpos += M5.Lcd.drawChar(':', xpos, ypos - 8, 8);
  if (ss < 10) {
    xpos += M5.Lcd.drawChar('0', xpos, ypos, 8);
  }
  M5.Lcd.drawNumber(ss, xpos, ypos, 8);
}

void startTimer() {
  startedAt = millis();
  targetAt = startedAt + 1000;
}

void stopTimer() {
  startedAt = 0;
  targetAt = 0;
}

void tickTock() {
  if (targetAt < millis()) {
    targetAt = targetAt + 1000;

    ss++;
    if (ss == 60) {
      ss = 0;
      mm += 1;
    }
    drawTimer();
  }
}

boolean isStarted() {
  return startedAt != 0;
}
