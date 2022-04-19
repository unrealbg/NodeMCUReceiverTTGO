#include <Arduino.h>

const int relayPin = D1;
const int relayPin2 = D2;
const int relayPin3 = D3;
const int relayPin4 = D4;
const int relayPin5 = D5;
const int relayPin6 = D6;
const int relayPin7 = D7;
const int relayPin8 = D8;
const int SensorPin = A0;
float soilmoisturepercent = 0;
float Temperature;
float Humidity;
float lux;
float setTemp = 0;
float setLight = 0;
float setSoil = 0;
float setHumidity = 0;
bool btnTemp = false;
bool btnSoil = false;
bool btnHumidity = false;
bool btnLight = false;
void sendUptime();
void checkTemp();
void checkLight();
void checkSoilMoisture();
void callback(char* topic, byte* payload, unsigned int length);