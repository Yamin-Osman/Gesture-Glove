#include <Arduino.h>
#include <BareI2C.h>
#include "imu.h"

float *IMUTask()
{
    constexpr uint8_t kImuAddress = 0x68;
    constexpr uint8_t kGyroStartRegister = 0x43;
    constexpr int16_t kGyroxOffset = 86;
    constexpr int16_t kGyroyOffset = 325;
    constexpr int16_t kGyrozOffset = 0;
    constexpr float kNeutralGyrox = -0.13;
    constexpr float kNeutralGyroy = 0.03;
    constexpr float kNeutralGyroz = -0.10;
    constexpr float kAlphaGyro = 0.20f;
    constexpr float kGyroThreshold = 0.25f;
    static float relativevalues[3] = {0.0f, 0.0f, 0.0f};
    static float filteredGyrox = 0.0f;
    static float filteredGyroy = 0.0f;
    static float filteredGyroz = 0.0f;


    uint8_t rawGyroValues[6];
    const I2CStatus gyroStatus = I2CWriteRead(kImuAddress, &kGyroStartRegister, 1, rawGyroValues, 6);

    if (gyroStatus != I2CStatus::Ok)
    {
        Serial.print(F("Gyro Read failed: "));
        Serial.println(static_cast<uint8_t>(gyroStatus));
        return relativevalues;
    }

    int16_t combinedGyroValues[3];
    for (int i = 0; i < 6; i += 2)
    {
        combinedGyroValues[i / 2] = (int16_t)((rawGyroValues[i] << 8) | rawGyroValues[i + 1]);
    }

    float rawRelativeGyrox = (((float)combinedGyroValues[0] - (float)kGyroxOffset) / 131.0f) - kNeutralGyrox;
    float rawRelativeGyroy = (((float)combinedGyroValues[1] - (float)kGyroyOffset) / 131.0f) - kNeutralGyroy;
    float rawRelativeGyroz = (((float)combinedGyroValues[2] - (float)kGyrozOffset) / 131.0f) - kNeutralGyroz;

    filteredGyrox = (kAlphaGyro * rawRelativeGyrox) + ((1.0f - kAlphaGyro) * filteredGyrox);
    filteredGyroy = (kAlphaGyro * rawRelativeGyroy) + ((1.0f - kAlphaGyro) * filteredGyroy);
    filteredGyroz = (kAlphaGyro * rawRelativeGyroz) + ((1.0f - kAlphaGyro) * filteredGyroz);

    float finalGyrox = (abs(filteredGyrox) < kGyroThreshold) ? 0.0f : filteredGyrox;
    float finalGyroy = (abs(filteredGyroy) < kGyroThreshold) ? 0.0f : filteredGyroy;
    float finalGyroz = (abs(filteredGyroz) < kGyroThreshold) ? 0.0f : filteredGyroz;

    relativevalues[0] = finalGyrox;
    relativevalues[1] = finalGyroy;
    relativevalues[2] = finalGyroz;

    return relativevalues;
}
