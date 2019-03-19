#include <M5Stack.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "time.h"

uint8_t mm = 0, ss = 0;
uint32_t startedAt = 0, targetAt = 0;

const char* ssid = "your-ssid";
const char* password = "your-password";
const char* pixelaPostEndPoint = "https://pixe.la/v1/users/your-pixela-username/graphs/your-graphid";
const char* pixelaToken = "your-pixela-token";

void setup() {
  M5.begin();

  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    M5.Lcd.printf(".");
  }
  M5.Lcd.println("\nconnected");

  delay(1000);

  // Reset LCD
  M5.Lcd.fillScreen(TFT_BLACK);

  // NTP
  configTime(9 * 3600, 0, "time.google.com");

  // Print Current Date
  struct tm timeInfo;
  getLocalTime(&timeInfo);
  M5.Lcd.printf("%4d-%02d-%02d", timeInfo.tm_year + 1900, timeInfo.tm_mon + 1, timeInfo.tm_mday);

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
  } else if (M5.BtnC.wasReleased()) {
    int responseCode = sendToPixela();
    M5.Lcd.println("\nsend to pixe.la. status: " + String(responseCode));
  }

  if (isStarted()) {
    tickTock();
  }
}

int sendToPixela() {
  char yyyymmdd[9] = "";

  struct tm timeInfo;
  getLocalTime(&timeInfo);
  strftime(yyyymmdd, 9, "%Y%m%d", &timeInfo);

  HTTPClient http;
  http.begin(pixelaPostEndPoint);
  http.addHeader("X-USER-TOKEN", pixelaToken);
  return http.POST("{\"date\":\"" + String(yyyymmdd) + "\",\"quantity\":\"" + String(mm) + "\"}");
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
