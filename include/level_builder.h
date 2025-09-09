/*
    FILE: level_builder.h
    Defines a module that loads levels from special .json files, allowing
    easy level creation and modification (at least, easier than creating levels
    directly on code)
*/
#pragma once
#include<raylib.h>
#include"entt.hpp"
#include"nlohmann/json.hpp"
#include"level_registry.h"
#include<fstream>
#include<iostream>
#include <vector>

namespace LevelBuilder
{
    // Differentiates between types of common errors. "Generic Error" is used when
    // none of the other options fit well
    enum ErrorType {
        SUCCESS = 0,
        GENERIC_ERROR = 1,
        COULDNT_OPEN_FILE, // Error when opening .json file, due to it not existing or not having read permissions
        PARSE_ERROR, // Error when parsing the .json file, unrelated to the actual JSON content
        INVALID_JSON_TYPE, // Expected one type of data when processing the JSON contents, got data of another type
        INVALID_JSON_OBJECT_STRUCTURE, // Expected an object with special determined structure (fields), found incompatible object
        SETTING_NOT_FOUND, // One or more fields needed by the processed JSON object type weren't present in the object
        INVALID_SETTING_VALUE, // Value of a field on the processed JSON object type is of the correct type but has an incorrect value
        INVALID_COMPONENT_NAME, // Tried to add a component of a non-existent or inaccessible type to an entity
        NEEDED_COMPONENT_NOT_FOUND, // The current component depends on one or more component types that the current entity doesn't have
    };

    // Error information when processing a file is stored here
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
          case ErrorType::INVALID_SETTING_VALUE:
            errorString = "[Invalid setting value]"; break;
          case ErrorType::INVALID_COMPONENT_NAME:
            errorString = "[Invalid component name]"; break;
          case ErrorType::NEEDED_COMPONENT_NOT_FOUND:
              errorString = "[Needed component not found]"; break;
          default:
            errorString = "[Unknown error]"; break;
        }
        out << errorString << ' ' << error.message;
        return out;
    }

    // Stores necessary context information on the level being built
    // (not just the error, but object data that needs to be kept alive
    // for the duration of the parsing and processing)
    struct Context {
        std::ifstream jsonFile; // The file stream from which the level data is taken
        std::map<std::string, entt::entity> entityNames; // Maps the level entity names used in the file to the entity IDs used in the registry
        std::map<std::string, std::vector<TilesetTile>> tilesets; // Maps the tileset names used in the file to the tileset tile data
        Error error;
        nlohmann::json topLevelJsonObject; // The JSON object that occupies the top level of the file.
    };

    // Takes a filename corresponding to a .json file and parses it,
    // returning a Context object with the obtained JSON data.
    Context init_level_parsing(const char* jsonFilename);

    // Takes a newly initialized context and processes all of the level data
    // to build the level into the passed registry. Both the context
    // and the registry are modified (context is modified to store
    // needed data and in case of error)
    void build_level(Context& context, LevelRegistry& registry);

}
