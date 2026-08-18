#include <Arduino.h>
#include "cursor.h"
#include <Mouse.h>

void mouseMove(float gyroHorizontal, float gyroVertical)
{
    constexpr float kCorrectionAngle = -0.15f;
    float cosA = cos(kCorrectionAngle);
    float sinA = sin(kCorrectionAngle);
    float correctedHorizontal = (gyroHorizontal * cosA) - (gyroVertical * sinA);
    float correctedVertical = (gyroHorizontal * sinA) + (gyroVertical * cosA);
    constexpr float kMouseSensitivityX = 0.15f;
    constexpr float kMouseSensitivityY = 0.12f;
    static float remainderX = 0.0f;
    static float remainderY = 0.0f;
    float inputSpeed = sqrt((correctedHorizontal * correctedHorizontal) + (correctedVertical * correctedVertical));

    float dynamicModifier = 0.4f + (inputSpeed * 0.1f);
    if (dynamicModifier > 1.5f)
        dynamicModifier = 1.5f;

    float targetX = (correctedHorizontal * kMouseSensitivityX * dynamicModifier) + remainderX;
    float targetY = (correctedVertical * kMouseSensitivityY * dynamicModifier) + remainderY;

    int moveX = (int)targetX;
    int moveY = (int)targetY;

    remainderX = targetX - moveX;
    remainderY = targetY - moveY;

    if (moveX != 0 || moveY != 0)
    {
        Mouse.move(moveX, moveY, 0);
    }
}
