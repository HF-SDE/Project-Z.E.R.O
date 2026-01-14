#include <string>
#include <variant>

namespace ConfigComponents
{
    using std::string;
    using std::variant;
    
    struct Config
    {
        std::string name;
        std::string comment;
    };

    struct keys
    {
        std::string id;
        variant<int, string> value;
        std::string value_type;
        Config config;
    };
}