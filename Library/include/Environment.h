#pragma once
#include <Arduino.h>

namespace Environment
{
    /**
     * @brief Defines the runtime environment mode.
     */
    enum class EnvironmentMode
    {
        PRODUCTION,
        TESTING,
        DEVELOPMENT
    };

    /**
     * @def ENVIRONMENT_COMPILETIME_MODE
     * @brief Optional compile-time environment selection.
     *
     * If defined, the environment mode is fixed at compile time for zero-cost builds:
     * - Logging can be compiled out in Production.
     * - The active environment can be treated as constant by the compiler.
     *
     * Valid values: Environment::EnvironmentType::Production, ::Testing, ::Development
     *
     * Example (in build flags or before includes):
     * @code
     * #define ENVIRONMENT_COMPILETIME_MODE Environment::EnvironmentType::Production
     * @endcode
     */

    /**
     * @brief Configures the active runtime environment settings.
     *
     * @details Sets the current environment mode and associates each environment
     *          with its corresponding server IP address.
     *
     * @warning Must be called once during startup (e.g., in setup()) before calling
     *          print(), getServerIP(), getEnvironmentMode(), or any is*() helper
     *
     * @param mode    Environment mode (PRODUCTION, TESTING, DEVELOPMENT)
     * @param prodIP  Production server IP
     * @param testIP  Testing server IP
     * @param devIP   Development server IP
     */
    void configureEnvironment(EnvironmentMode mode = EnvironmentMode::DEVELOPMENT, const String &prodIP = "", const String &testIP = "", const String &devIP = "");

    /**
     * @brief Print message based on environment
     *
     * @details
     * - If ENV_DISABLE_LOGS is defined, logging is compiled out (zero-cost).
     * - Otherwise, output is suppressed when running in PRODUCTION.
     *
     * @param message Message to print to the serial monitor.
     */
    void print(const String &message);

    /// @brief Overload that avoids String allocations for string literals.
    void print(const char *message);

    /// @brief Overload for flash-stored strings: Environment::print(F("text"));
    void print(const __FlashStringHelper *message);

    /**
     * @brief Returns the server IP for the active environment.
     *
     * @return Server IP address as a String.
     */
    [[nodiscard]] String getServerIP();

    /**
     * @brief Returns the currently active environment mode.
     *
     * @return Active environment mode.
     */
    [[nodiscard]] EnvironmentMode getEnvironmentMode();

    /**
     * @brief Indicates whether the active environment is Production.
     *
     * @return true if Production; false otherwise.
     */
    [[nodiscard]] bool isProduction();

    /**
     * @brief Indicates whether the active environment is Testing.
     *
     * @return true if Testing; false otherwise.
     */
    [[nodiscard]] bool isTesting();

    /**
     * @brief Indicates whether the active environment is Development.
     *
     * @return true if Development; false otherwise.
     */
    [[nodiscard]] bool isDevelopment();

#if defined(ENVIRONMENT_COMPILETIME_MODE)
    // Convenience constant for compile-time selection.
    inline constexpr EnvironmentType kCompileTimeMode = ENVIRONMENT_COMPILETIME_MODE;
#endif

}
