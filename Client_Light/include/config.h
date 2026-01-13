namespace ButtonConfig
{
    struct Config
    {
        std::string name;
        std::string comment;
    };

    struct keys
    {
        std::string id;
        int value;
        std::string value_type;
        Config config;
    };
}