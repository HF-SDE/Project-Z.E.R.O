#include "Util.h"
#include <Axios.h>
#include <EEPROMHelper.h>
#include <LcdHelper.h>
#include <UUID.h>
#include <Arduino_Secrets.h>

String getUuid(String defaultString = "") {
    if (defaultString != "") return defaultString;
    
    String uuid = readFromEEPROM(UUID_OFFSET);
    if (uuid == "") {
        Serial.println("No UUID found. Generating a new one...");
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
    UUID uuid;

    String uuidWithDashes = uuid.toCharArray();
    
    String uuidWithoutDashes = "";
    for (int i = 0; i < uuidWithDashes.length(); i++) {
        if (uuidWithDashes[i] != '-') {
            uuidWithoutDashes += uuidWithDashes[i];
        }
    }

    return uuidWithoutDashes;
}

String getApiKey(String uuid) {
    Axios axiosWithoutApiKey(BACKEND_IP);

    String apikey = readFromEEPROM(API_KEY_OFFSET);

    if (apikey == "") {
        Serial.println("No API Key found. Generating a new one...");
        StaticJsonDocument<JSON_BUFFER_SIZE> dataBody = axiosWithoutApiKey.get("/api/device");

        const char* token = dataBody["data"][0]["token"];
        apikey = String(token);

        saveToEEPROM(API_KEY_OFFSET, apikey);
    }

    return apikey;
}