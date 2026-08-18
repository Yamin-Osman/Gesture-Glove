#include <Arduino.h>
#include <BareI2C.h>
#include "imu.h"
#include "cursor.h"
#include <Mouse.h>

const int leftClick = 7;
const int rightClick = 4;
const int middleClick = 5;

unsigned long serialStartTime = 0;
unsigned long lastDisplayedTime = 0;
unsigned long lastPressedTime = 0;

bool lastLeftState = HIGH;
bool lastRightState = HIGH;
bool lastMiddleState = HIGH;

void setup()
{
  pinMode(leftClick, INPUT_PULLUP);
  pinMode(rightClick, INPUT_PULLUP);
  pinMode(middleClick, INPUT_PULLUP);

  Serial.begin(115200);
  Mouse.begin();

  while (!Serial && (millis() - serialStartTime < 3000))
  {
  }

  Serial.println(F("Starting IMU test"));

  const I2CStatus initStatus = I2CInit(100000);
  if (initStatus != I2CStatus::Ok)
  {
    Serial.print(F("I2C initialization failed: "));
    Serial.println(static_cast<uint8_t>(initStatus));
    while (true)
    {
    }
  }
}

void loop()
{
  unsigned long currentTime = millis();

  bool currentLeft = digitalRead(leftClick);
  bool currentRight = digitalRead(rightClick);
  bool currentMiddle = digitalRead(middleClick);

  if (currentTime - lastPressedTime > 50)
  {
    if (currentLeft == LOW && lastLeftState == HIGH)
    {
      Mouse.click(MOUSE_LEFT);
      lastPressedTime = currentTime;
    }
    else if (currentRight == LOW && lastRightState == HIGH)
    {
      Mouse.click(MOUSE_RIGHT);
      lastPressedTime = currentTime;
    }
    else if (currentMiddle == LOW && lastMiddleState == HIGH)
    {
      Mouse.click(MOUSE_MIDDLE);
      lastPressedTime = currentTime;
    }
  }

  lastLeftState = currentLeft;
  lastRightState = currentRight;
  lastMiddleState = currentMiddle;

  float *relativeValues = IMUTask();
  mouseMove(relativeValues[2], relativeValues[1]);

  if (currentTime - lastDisplayedTime > 200)
  {
    lastDisplayedTime = currentTime;
    Serial.print(F("GyroX: "));
    Serial.println(relativeValues[0]);
    Serial.print(F("GyroY: "));
    Serial.println(relativeValues[1]);
    Serial.print(F("GyroZ: "));
    Serial.println(relativeValues[2]);
  }
}
