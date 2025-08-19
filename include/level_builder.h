#pragma once
#include<raylib.h>
#include"entt.hpp"
#include"nlohmann/json.hpp"
#include"level_registry.h"
#include<fstream>
#include<iostream>

namespace LevelBuilder
{
    enum ErrorType {
        SUCCESS = 0,
        GENERIC_ERROR = 1,
        COULDNT_OPEN_FILE,
        PARSE_ERROR,
        INVALID_JSON_TYPE,
        INVALID_JSON_OBJECT_STRUCTURE,
        SETTING_NOT_FOUND,
        INVALID_SETTING_NAME,
        INVALID_SETTING_VALUE,
        INVALID_COMPONENT_NAME,
    };

    struct Error {
        std::string message;
        ErrorType type;
        Error(ErrorType type, std::string message) : type{type}, message{message} {};
        Error(ErrorType type) : type{type}, message() {};
        inline bool ok() const { return type == ErrorType::SUCCESS; };
        inline operator bool() const { return type != ErrorType::SUCCESS; };
    };

    inline std::ostream& operator<< (std::ostream& out, const Error& error){
        std::string errorString;
        switch(error.type){
          case ErrorType::SUCCESS:
            errorString = "[Success]"; break;
          case ErrorType::GENERIC_ERROR:
            errorString = "[Error]"; break;
          case ErrorType::COULDNT_OPEN_FILE:
            errorString = "[Couldn't open file]"; break;
          case ErrorType::PARSE_ERROR:
            errorString = "[Parsing error]"; break;
          case ErrorType::INVALID_JSON_TYPE:
            errorString = "[Invalid JSON type]"; break;
          case ErrorType::INVALID_JSON_OBJECT_STRUCTURE:
            errorString = "[Invalid JSON object structure]"; break;
          case ErrorType::SETTING_NOT_FOUND:
            errorString = "[Needed setting not found]"; break;
          case ErrorType::INVALID_SETTING_NAME:
            errorString = "[Invalid setting name]"; break;
          case ErrorType::INVALID_SETTING_VALUE:
            errorString = "[Invalid setting value]"; break;
          case ErrorType::INVALID_COMPONENT_NAME:
            errorString = "[Invalid component name]"; break;
          default:
            errorString = "[Unknown error]"; break;
        }
        out << errorString << ' ' << error.message;
        return out;
    }

    struct Context {
        std::ifstream jsonFile;
        Error error;
        nlohmann::json topLevelJsonObject;
    };

    Context init_level_parsing(const char* jsonFilename);

    void build_level(Context& context, LevelRegistry& registry);

}