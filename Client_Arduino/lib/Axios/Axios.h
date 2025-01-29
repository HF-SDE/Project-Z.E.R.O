#ifndef AXIOS_H
#define AXIOS_H

#include <WiFiClient.h>
#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_ENABLE_PRETTY_PRINT 0
#include <ArduinoJson.h>
#include <String.h>

constexpr size_t JSON_BUFFER_SIZE = 2048;

class Axios {
private:
    WiFiClient client;
    const char* server;
    String deviceApiKey = "";
    String deviceUuid;

public:
    Axios(const char* serverAddress, String apiKey = "");

    StaticJsonDocument<JSON_BUFFER_SIZE> get(const char* endpoint);
    StaticJsonDocument<JSON_BUFFER_SIZE> post(const char* endpoint, const String& payload);

    String extractJsonBody(const String& response);
    StaticJsonDocument<JSON_BUFFER_SIZE> parseJson(const String& json);
};

#endif