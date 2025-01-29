#include "Util.h"
#include <Axios.h>
#include <EEPROMHelper.h>
#include <LcdHelper.h>
#include <Arduino_Secrets.h>

String getUuid() {    
    String uuid = readFromEEPROM(UUID_OFFSET);
    if (uuid == "") {
        uuid = generateUUID();
        saveToEEPROM(UUID_OFFSET, uuid);

        String uuidLine1 = uuid.substring(0, 16);
        String uuidLine2 = uuid.substring(16);

        writeToLCD(uuidLine1, uuidLine2);

        while (digitalRead(BUTTON_PIN) == LOW) {
            delay(100);
        }

        getApiKey(uuid);
    }
    return uuid;
}

String generateUUID() {
    String randomUUID = UUID;
    
    String uuidWithoutDashes = "";
    for (int i = 0; i < randomUUID.length(); i++) {
        if (randomUUID[i] != '-') {
            uuidWithoutDashes += randomUUID[i];
        }
    }

    Serial.println("Generated UUID: " + uuidWithoutDashes);

    return uuidWithoutDashes;
}

String getApiKey(String uuid) {
    Axios axiosWithoutApiKey(BACKEND_IP);

    String apikey = readFromEEPROM(API_KEY_OFFSET);

    if (apikey == "") {
        Serial.println("No API Key found. Fetching api key from server...");
        StaticJsonDocument<JSON_BUFFER_SIZE> dataBody = axiosWithoutApiKey.get("/api/device");

        const char* token = dataBody["data"][0]["token"];
        apikey = String(token);

        if (apikey == "") {
            Serial.println("Couldn't fetch api key. Exiting...");
            serializeJsonPretty(dataBody, Serial);
            writeToLCD("Apikey failure", "Restart device");
            while (true) {
                delay(100);
            }
        }

        saveToEEPROM(API_KEY_OFFSET, apikey);

        Serial.println("Fetched new api key: " + apikey);
    }

    return apikey;
}