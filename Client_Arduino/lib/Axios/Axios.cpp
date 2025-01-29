#include "Axios.h"
#include "../../include/Util.h"

// Constructor
Axios::Axios(const char* serverAddress, String apiKey = "") {
    server = serverAddress;
    deviceUuid = getUuid();
    deviceApiKey = apiKey;
}

StaticJsonDocument<JSON_BUFFER_SIZE> Axios::get(const char* endpoint) {
    if (client.connect(server, 80)) {
        client.println(String("GET ") + endpoint + " HTTP/1.1");
        client.println("Host: " + String(server));
        client.println("x-api-key: " + String(deviceApiKey));
        client.println("device-id: " + deviceUuid);
        client.println("Content-Type: application/json");
        client.println("Connection: close");
        client.println();

        // Read the response
        String response = "";
        while (client.connected()) {
            if (client.available()) response += client.readString();
        }

        client.stop();

        String jsonBody = extractJsonBody(response);

        return parseJson(jsonBody);
    } else {
        return parseJson("");
    }
}

// POST Method
StaticJsonDocument<JSON_BUFFER_SIZE> Axios::post(const char* endpoint, const String& payload) {
    if (client.connect(server, 80)) {
        client.println(String("POST ") + endpoint + " HTTP/1.1");
        client.println("Host: " + String(server));
        client.println("x-api-key: " + String(deviceApiKey));
        client.println("device-id: " + String(deviceUuid));
        client.println("Content-Type: application/json");
        client.print("Content-Length: ");
        client.println(payload.length());
        client.println();
        client.print(payload);

        String jsonBody = "";
        while (client.connected()) {
            String response = "";

            if (client.available()) response += client.readString();
            if (response == "") continue;

            jsonBody = extractJsonBody(response);
            if (jsonBody != "") break;
        }

        client.stop();

        return parseJson(jsonBody);
    } else {
        return parseJson("");
    }
}

// Helper to extract JSON body from response
String Axios::extractJsonBody(const String& response) {
    int jsonStart = response.indexOf("\r\n\r\n");
    if (jsonStart == -1) {
        return "";
    }
    return response.substring(jsonStart + 4);
}

// Helper to parse JSON
StaticJsonDocument<JSON_BUFFER_SIZE> Axios::parseJson(const String& json) {
    StaticJsonDocument<JSON_BUFFER_SIZE> doc;
    DeserializationError error = deserializeJson(doc, json);

    if (error) {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.f_str());
        doc.clear();
    }

    return doc;
}