#include <Arduino.h>
#include <mqtt-auth.h>
#include <user-variables.h>
#include <arduino-timer.h>
#include <uptime_formatter.h>
#include <PubSubClient.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <ArduinoJson.h>

WiFiClient espClient;
PubSubClient client(espClient);
AsyncWebServer server(80);
DNSServer dns;

auto timer = timer_create_default();

bool relayCheck(void*) 
{
    checkTemp();
    checkLight();
    checkSoilMoisture();
    return true;
}

void checkTemp() 
{
    if (setTemp <= Temperature && setTemp != 0)
    {
        digitalWrite(relayPin, LOW);
        client.publish("home/NodeMCUTTGO/buttonState/temp", "true");
    }
    else if (btnTemp != true && setTemp - 1 >= Temperature)
    {
        digitalWrite(relayPin, HIGH);
        client.publish("home/NodeMCUTTGO/buttonState/temp", "false");
    }

    if (setHumidity > Humidity && setHumidity != 0)
    {
        digitalWrite(relayPin4, LOW);
        client.publish("home/NodeMCUTTGO/buttonState/humidity", "true");
    }
    else if (btnHumidity != true && setHumidity + 15 <= Humidity && setHumidity != 0)
    {
        digitalWrite(relayPin4, HIGH);
        client.publish("home/NodeMCUTTGO/buttonState/humidity", "false");
    }
}

void checkLight() 
{
    if (lux <= setLight && setLight != 0)
    {
        digitalWrite(relayPin3, LOW);
        client.publish("home/NodeMCUTTGO/buttonState/light", "true");
    }
    else if (btnLight != true && lux + 10 > setLight && setLight != 0)
    {
        digitalWrite(relayPin3, HIGH);
        client.publish("home/NodeMCUTTGO/buttonState/light", "false");
    }
}

void checkSoilMoisture() 
{
    if (soilmoisturepercent <= setSoil && setSoil)
    {
        digitalWrite(relayPin2, LOW);
        client.publish("home/NodeMCUTTGO/buttonState/soil", "true");
    }
    else if (btnSoil != true && soilmoisturepercent > setSoil + 10 && setSoil != 0)
    {
        digitalWrite(relayPin2, HIGH);
        client.publish("home/NodeMCUTTGO/buttonState/soil", "false");
    }
}

void checkConnection() 
{
    if (!client.connected()) {
        Serial.println("MQTT Broker disconected!");
        Serial.println("Reconnect...");
        ESP.restart();
    }
}

void setup_mqtt() 
{
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    while (!client.connected()) {
        Serial.println("Connecting to MQTT...");

        if (client.connect("nodeMCU2", mqttUser, mqttPassword)) {

            Serial.println("connected");

        }
        else {

            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }
}

bool sendUp(void*) 
{
    sendUptime();
    return true;
}

void sendUptime() 
{
    client.publish("home/NodeMCUTTGO/uptime", String(uptime_formatter::getUptime()).c_str(), true);
}

void callback(char* topic, byte* payload, unsigned int length) 
{
    String message;

    for (int i = 0; i < length; i++) 
    {
        message += (char)payload[i];
    }

    String myTopic = String(topic);

    if (myTopic == "home/NodeMCUTTGO/set/temp")
    {
        setTemp = message.toFloat();
    }
    else if (myTopic == "home/NodeMCUTTGO/set/light")
    {
        setLight = message.toFloat();
    }
    else if (myTopic == "home/NodeMCUTTGO/set/soil")
    {
        setSoil = message.toInt();
    }
    else if (myTopic == "home/NodeMCUTTGO/set/humidity")
    {
        setHumidity = message.toInt();
    }
    else if (myTopic == "home/NodeMCUTTGO/switch/tempRelay")
    {
        if (message == "true")
        {
            digitalWrite(relayPin, LOW);
        }
        else
        {
            digitalWrite(relayPin, HIGH);
        }
    }
    else if (myTopic == "home/NodeMCUTTGO/switch/lightRelay")
    {
        if (message == "true")
        {
            digitalWrite(relayPin3, LOW);
        }
        else
        {
            digitalWrite(relayPin3, HIGH);
        }
    }
    else if (myTopic == "home/NodeMCUTTGO/switch/humidityRelay")
    {
        if (message == "true")
        {
            digitalWrite(relayPin4, LOW);
        }
        else
        {
            digitalWrite(relayPin4, HIGH);
        }
    }
    else if (myTopic == "home/NodeMCUTTGO/switch/soilRelay")
    {
        if (message == "true")
        {
            digitalWrite(relayPin2, LOW);
        }
        else
        {
            digitalWrite(relayPin2, HIGH);
        }
    }
    else if (myTopic == "LilyGo/083af2667048")
    {
        StaticJsonDocument<768> doc;
        deserializeJson(doc, payload, length);

        JsonObject plant = doc["plant"];
    // const char* plant_LilyGo = plant["LilyGo"]; // "083af2667048"
    // const char* plant_date = plant["date"]; // "06-04"
    // const char* plant_sensorname = plant["sensorname"]; // nullptr
    // const char* plant_time = plant["time"]; // "17:54"
    // int plant_sleep5Count = plant["sleep5Count"]; // 0
    // int plant_bootCount = plant["bootCount"]; // 2057
        float plant_lux = plant["lux"]; // 49.16666
        float plant_temp = plant["temp"]; // 25.24
        float plant_humid = plant["humid"]; // 42.00879
        int plant_soil = plant["soil"]; // 59
    // float plant_soilTemp = plant["soilTemp"]; // 4095.938
    // int plant_salt = plant["salt"]; // 2339
    // const char* plant_saltadvice = plant["saltadvice"]; // "too high"
    // int plant_bat = plant["bat"]; // 81
    // const char* plant_batcharge = plant["batcharge"]; // nullptr
    // const char* plant_batchargeDate = plant["batchargeDate"]; // "06-04"
    // float plant_daysOnBattery = plant["daysOnBattery"]; // 0.2
    // int plant_battvolt = plant["battvolt"]; // 2210
    // float plant_battvoltage = plant["battvoltage"]; // 3.918095
    // float plant_pressure = plant["pressure"]; // 1010.864
    // int plant_plantValveNo = plant["plantValveNo"]; // 1
    // const char* plant_wifissid = plant["wifissid"]; // "Skynet"
    // const char* plant_rel = plant["rel"]; // "4.3.2"

        Temperature = plant_temp;
        Humidity = plant_humid;
        soilmoisturepercent = plant_soil;
        lux = plant_lux;
    }
    else if (myTopic == "home/lilygo/valves/valve1")
    {
        if (message == "true")
        {
            digitalWrite(relayPin, LOW);
        }
        else
        {
            digitalWrite(relayPin, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve2")
    {
        if (message == "true")
        {
            digitalWrite(relayPin2, LOW);
        }
        else
        {
            digitalWrite(relayPin2, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve3")
    {
        if (message == "true")
        {
            digitalWrite(relayPin3, LOW);
        }
        else
        {
            digitalWrite(relayPin3, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve4")
    {
        if (message == "true")
        {
            digitalWrite(relayPin4, LOW);
        }
        else
        {
            digitalWrite(relayPin4, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve5")
    {
        if (message == "true")
        {
            digitalWrite(relayPin5, LOW);
        }
        else
        {
            digitalWrite(relayPin5, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve6")
    {
        if (message == "true")
        {
            digitalWrite(relayPin6, LOW);
        }
        else
        {
            digitalWrite(relayPin6, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve7")
    {
        if (message == "true")
        {
            digitalWrite(relayPin7, LOW);
        }
        else
        {
            digitalWrite(relayPin7, HIGH);
        }
    }
    else if (myTopic == "home/lilygo/valves/valve8")
    {
        if (message == "true")
        {
            digitalWrite(relayPin8, LOW);
        }
        else
        {
            digitalWrite(relayPin8, HIGH);
        }
    }
}



void setup() 
{
    // Debug console
    Serial.begin(115200);

    AsyncWiFiManager wifiManager(&server, &dns);
    wifiManager.autoConnect();
    setup_mqtt();

    pinMode(relayPin, OUTPUT);
    pinMode(relayPin2, OUTPUT);
    pinMode(relayPin3, OUTPUT);
    pinMode(relayPin4, OUTPUT);
    pinMode(relayPin5, OUTPUT);
    pinMode(relayPin6, OUTPUT);
    pinMode(relayPin7, OUTPUT);
    pinMode(relayPin8, OUTPUT);
    digitalWrite(relayPin, HIGH);
    digitalWrite(relayPin2, HIGH);
    digitalWrite(relayPin3, HIGH);
    digitalWrite(relayPin4, HIGH);
    digitalWrite(relayPin5, HIGH);
    digitalWrite(relayPin6, HIGH);
    digitalWrite(relayPin7, HIGH);
    digitalWrite(relayPin8, HIGH);
    timer.every(2000, relayCheck);
    timer.every(1000, sendUp);
    client.setBufferSize(768);
    client.subscribe("#");
}

void loop() 
{
    client.loop();
    checkConnection();
    timer.tick();
}