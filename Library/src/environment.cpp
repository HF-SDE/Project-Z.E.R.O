#include "environment.h"

namespace Environment
{
    // Internal state
    static bool configured = false;

    static EnvironmentMode currentEnvironment = EnvironmentMode::DEVELOPMENT;

    static String productionIP;
    static String testingIP;
    static String developmentIP;

    static const String &resolveServerIP()
    {
        switch (currentEnvironment)
        {
        case EnvironmentMode::PRODUCTION:
            return productionIP;
        case EnvironmentMode::TESTING:
            return testingIP;
        case EnvironmentMode::DEVELOPMENT:
            return developmentIP;
        }
        return developmentIP;
    }

    void configureEnvironment(EnvironmentMode mode, const String &prodIP, const String &testIP, const String &devIP)
    {
        currentEnvironment = mode;
        productionIP = prodIP;
        testingIP = testIP;
        developmentIP = devIP;

        configured = true;
    }

    static inline bool shouldLog()
    {
        // Deterministic behavior before setup/configuration:
        if (!configured)
            return false;

        // Suppress logs in production environment at runtime:
        return currentEnvironment != EnvironmentMode::PRODUCTION;
    }

    void print(const String &message)
    {
#if defined(ENV_DISABLE_LOGS)
        (void)message;
        return;
#else
        if (!shouldLog())
            return;
        Serial.println(message);
#endif
    }

    void print(const char *message)
    {
#if defined(ENV_DISABLE_LOGS)
        (void)message;
        return;
#else
        if (!shouldLog())
            return;
        Serial.println(message);
#endif
    }

    void print(const __FlashStringHelper *message)
    {
#if defined(ENV_DISABLE_LOGS)
        (void)message;
        return;
#else
        if (!shouldLog())
            return;
        Serial.println(message);
#endif
    }

    [[nodiscard]] String getServerIP()
    {
        if (!configured)
            return String();
        return resolveServerIP();
    }

    [[nodiscard]] EnvironmentMode getEnvironmentMode()
    {
        return currentEnvironment;
    }

    [[nodiscard]] bool isProduction()
    {
        return currentEnvironment == EnvironmentMode::PRODUCTION;
    }

    [[nodiscard]] bool isTesting()
    {
        return currentEnvironment == EnvironmentMode::TESTING;
    }

    [[nodiscard]] bool isDevelopment()
    {
        return currentEnvironment == EnvironmentMode::DEVELOPMENT;
    }
}