#include <Wire.h>
#include "rgb_lcd.h"
#include <DHT.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <Axios.h>
#include <LcdHelper.h>
#include <EEPROMHelper.h>
#include <Util.h>
#include <Arduino_Secrets.h>
#include <avr/wdt.h>

#define DHTPIN A0    
#define DHTTYPE DHT11

#define debug Serial

DHT dht(DHTPIN, DHTTYPE);

void sendPostRequest();
void connectToWifi(int retries = 0);
int* timer();

int seconds = 0;
int minutes = 0;
int hours = 0;

int time = 0;
int previosNumber = 0;

int frequency = 0;

StaticJsonDocument<JSON_BUFFER_SIZE> deviceSettings;

Axios* axios = nullptr;

WiFiClient client;
const char* websocketUrl = BACKEND_IP "/api/device";

void setup() {
    debug.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    Wire.begin();
    dht.begin();
    lcd.begin(16, 2);
    lcd.clear();

    Serial.println("Starting...");

    connectToWifi();

    String ApiKey = getApiKey(getUuid());

    axios = new Axios(BACKEND_IP, ApiKey);

    StaticJsonDocument<JSON_BUFFER_SIZE> deviceResponse = axios->get("/api/device");

    if (deviceResponse["status"] == "Unauthorized") {
        Serial.println("Unauthorized. Exiting...");

        writeToLCD("Unauthorized", "Restart device");

        clearEEPROM();

        wdt_enable(WDTO_15MS);
        while (1) {}
        return;
    }

    deviceSettings = deviceResponse["data"][0];

    frequency = deviceSettings["frequency"];
    const char* status = deviceSettings["status"];

    Serial.print("Frequency: ");
    Serial.println(frequency);

    Serial.print("Device Status: ");
    Serial.println(status);
}

void loop() {
    timer();

    float hum = dht.readHumidity();
    float temp = dht.readTemperature();

    writeToLCD("Temp: " + String(temp) + "C", "Humidity: " + String(hum) + "%");

    if (temp > 25) { lcd.setRGB(255, 0, 0); }
    else if (temp > 20) { lcd.setRGB(255, 255, 0); }
    else { lcd.setRGB(0, 255, 0); }

        if (minutes % 3 == 0 && seconds == 0 && time != previosNumber) {
        deviceSettings = axios->get("/api/device")["data"][0];

        if (deviceSettings["frequency"].as<int>() != frequency) {
            Serial.print("Updated Frequency: ");
            Serial.println(frequency);

            frequency = deviceSettings["frequency"];
        }
    }

    if (frequency > 0 && time % (frequency / 1000) == 0 && time != previosNumber) {
        if (deviceSettings["status"] == "ACTIVE") sendPostRequest();
    }
}

class Data {
    public:
        float value;
        String identifier;
        String name;

        Data(float v, String id, String n)
            : value(v), identifier(id), name(n) {}

        void toJsonObject(JsonObject& json) {
            json["value"] = value;
            json["identifier"] = identifier;
            json["name"] = name;
        }
};

void sendPostRequest() {
    if (!axios) return;

    Data dataArray[] = {
        Data( dht.readTemperature(), "celsius", "RoomTemp"),
        Data( dht.readHumidity(), "percentage", "RoomHumidity")
    };

    StaticJsonDocument<512> data;
    JsonArray dataArrayJson = data.to<JsonArray>();

    for (size_t i = 0; i < 2; i++) {
        JsonObject obj = dataArrayJson.createNestedObject();
        dataArray[i].toJsonObject(obj);
    }

    String jsonPayload;
    serializeJson(dataArrayJson, jsonPayload);

    unsigned long startTime = millis();
    StaticJsonDocument<JSON_BUFFER_SIZE> response = axios->post("/api/data", jsonPayload);
    unsigned long endTime = millis();

    unsigned long duration = endTime - startTime;
    Serial.print("Function execution time: ");
    Serial.print(duration);
    Serial.println(" ms");

    const char* status = response["status"];
    const char* message = response["message"];

    Serial.println("Status: " + String(status));
    Serial.println("Message: " + String(message));

    Serial.print("\n\n");
}

void connectToWifi(int retries = 0) {
    writeToLCD("Connecting WiFi", "Retries: " + String(retries));
    if (retries == 0) Serial.println("Connecting to WiFi...");

    int status = WiFi.begin(WIFI_SSID);
    if (status != WL_CONNECTED) return connectToWifi(retries + 1);

    Serial.println("Connected to WiFi");
}

int* timer() {
    previosNumber = time;
    time = millis() / 1000;

    if (time != previosNumber) {
        seconds++;
    }

    int countsystem = 60;

    if (seconds >= countsystem) {
        seconds -= countsystem;
        minutes++;
    }

    if (minutes >= countsystem) {
        minutes -= countsystem;
        hours++;
    }

    static int timers[3] = { seconds, minutes, hours }; 

    return timers;
}
