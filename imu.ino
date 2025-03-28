#include "LSM6DS3.h"
#include "Wire.h"
#include <math.h>
#include <ArduinoBLE.h>

LSM6DS3 myIMU(I2C_MODE, 0x6A);  // IMU at I2C address 0x6A
BLEService postureService("265a765f-d391-4cf1-bc09-25b1db879925"); 
BLEByteCharacteristic slouchStatusChar("2A59", BLERead | BLENotify);

float angleY = 0;  // Estimated angle
unsigned long prevTime;

#define BUTTON_PIN 4
#define SLOUCH_THRESHOLD 15.0  
#define SLOUCH_TIME 3000       
#define RECALIBRATION_TIME 5000  

unsigned long slouchStartTime = 0, recalibrationStartTime = 0;
float baselineAngleY = 90;  
bool slouching = false;
bool buttonPressed = false;

// Kalman Filter Variables
float Q_angle = 0.01, Q_bias = 0.003, R_measure = 0.03;
float angleKalman = 90, bias = 0, rate = 0; 
float P[2][2] = {{0, 0}, {0, 0}};

void setup() {
    Serial.begin(9600);
    while (!Serial);

    // Initialize IMU
    if (myIMU.begin() != 0) {
        Serial.println("Device error");
        while (1);
    } else {
        Serial.println("Device OK!");
    }

    // Initialize BLE
    if (!BLE.begin()) {
        Serial.println("failed to initialize BLE!");
        while (1);
    }

    BLE.setLocalName("PostureMonitor");
    BLE.setAdvertisedService(postureService);
    postureService.addCharacteristic(slouchStatusChar);
    BLE.addService(postureService);
    BLE.advertise();
    Serial.println("advertising ...");

    pinMode(BUTTON_PIN, INPUT);
    pinMode(LED_BUILTIN, OUTPUT);

    prevTime = millis();
    delay(2000);  // Initial delay for stabilization
    calibrateUpright();
}

void loop() {
    unsigned long currentTime = millis();
    float dt = (currentTime - prevTime) / 1000.0; 
    prevTime = currentTime;

    // IMU posture detection
    float accX = myIMU.readFloatAccelX();
    float accY = myIMU.readFloatAccelY();
    float accZ = myIMU.readFloatAccelZ();
    float gyroY = myIMU.readFloatGyroY();

    float accAngleY = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * (180.0 / M_PI) + 90.0;
    angleY = kalmanFilter(accAngleY, gyroY, dt);

    // Slouch detection and recalibration logic
    bool newSlouchState = detectSlouch(angleY);
    if (!newSlouchState && slouching) {
        if (recalibrationStartTime == 0) {
            recalibrationStartTime = millis();
        } else if (millis() - recalibrationStartTime > RECALIBRATION_TIME) {
            calibrateUpright();
            recalibrationStartTime = 0;
        }
    } else {
        recalibrationStartTime = 0;
    }
    slouching = newSlouchState;

    // Update LED
    digitalWrite(LED_BUILTIN, slouching ? HIGH : LOW);

    // BLE handling
    BLEDevice central = BLE.central();
    if (central) {
        Serial.print("Connected to central: ");
        Serial.println(central.address());
        
        while (central.connected()) {
            // Update BLE characteristic with current slouch status
            slouchStatusChar.writeValue(slouching ? 1 : 0);

            // Serial output for debugging
            Serial.print("AngleY: ");
            Serial.print(angleY, 2);
            Serial.print(" | Baseline: ");
            Serial.print(baselineAngleY, 2);
            Serial.print(" | Slouching: ");
            Serial.println(slouching ? "YES" : "NO");

            // Button handling for recalibration
            if (digitalRead(BUTTON_PIN) == HIGH && !buttonPressed) {
                calibrateUpright();
                buttonPressed = true; 
            } else if (digitalRead(BUTTON_PIN) == LOW) {
                buttonPressed = false; 
            }

            // Short delay to prevent overwhelming BLE updates
            delay(50);

            // Recalculate posture within BLE loop to keep data fresh
            currentTime = millis();
            dt = (currentTime - prevTime) / 1000.0;
            prevTime = currentTime;

            accX = myIMU.readFloatAccelX();
            accY = myIMU.readFloatAccelY();
            accZ = myIMU.readFloatAccelZ();
            gyroY = myIMU.readFloatGyroY();

            accAngleY = atan2(-accX, sqrt(accY * accY + accZ * accZ)) * (180.0 / M_PI) + 90.0;
            angleY = kalmanFilter(accAngleY, gyroY, dt);

            newSlouchState = detectSlouch(angleY);
            slouching = newSlouchState;
            digitalWrite(LED_BUILTIN, slouching ? HIGH : LOW);
        }
        Serial.println("Disconnected from central.");
    }

    // Short delay when not connected to BLE to maintain responsiveness
    delay(50);
}

void calibrateUpright() {
    baselineAngleY = angleY;
    Serial.println("Angle reset to zero!");
}

bool detectSlouch(float angle) {
    if (abs(angle - baselineAngleY) > SLOUCH_THRESHOLD) {
        if (slouchStartTime == 0) slouchStartTime = millis();
        if (millis() - slouchStartTime > SLOUCH_TIME) {
            return true;
        }
    } else {
        slouchStartTime = 0;
    }
    return false;
}

float kalmanFilter(float newAngle, float newRate, float dt) {
    rate = newRate - bias;
    angleKalman += dt * rate;

    P[0][0] += dt * (dt * P[1][1] - P[0][1] - P[1][0] + Q_angle);
    P[0][1] -= dt * P[1][1];
    P[1][0] -= dt * P[1][1];
    P[1][1] += Q_bias * dt;

    float S = P[0][0] + R_measure;
    float K[2] = {P[0][0] / S, P[1][0] / S};

    float y = newAngle - angleKalman;
    angleKalman += K[0] * y;
    bias += K[1] * y;

    float P00_temp = P[0][0];
    float P01_temp = P[0][1];
    P[0][0] -= K[0] * P00_temp;
    P[0][1] -= K[0] * P01_temp;
    P[1][0] -= K[1] * P00_temp;
    P[1][1] -= K[1] * P01_temp;

    return angleKalman;
}
