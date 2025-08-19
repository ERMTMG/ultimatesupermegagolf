#include"level_builder.h"
#include<sstream>

namespace LevelBuilder
{
    using Json = nlohmann::json;
    Context init_level_parsing(const char* jsonFilename){
        std::ifstream file{jsonFilename};
        if(file.bad()){
            std::cerr << "<ERROR> at init_level_parsing: failed to open file " << jsonFilename << '\n'; 
            return Context {
                .jsonFile = std::move(file),
                .error = {ErrorType::COULDNT_OPEN_FILE, std::string(jsonFilename)},
                .topLevelJsonObject = Json{Json::value_t::null} // null JSON object
            };
        }
        Json levelJson;
        try {
            levelJson = Json::parse(file);
        } catch(Json::parse_error& err){
            std::cerr << "<ERROR> at init level_parsing: parse error in file " << jsonFilename << ": " << err.what() << '\n';
            return Context {
                .jsonFile = std::move(file),
                .error = {ErrorType::PARSE_ERROR, err.what()},
                .topLevelJsonObject = Json{Json::value_t::null}
            };
        }
        return Context {
            .jsonFile = std::move(file),
            .error = {ErrorType::SUCCESS},
            .topLevelJsonObject = levelJson
        };
    }

    static std::string json_get_string(Context& context, const Json& object){
        if(!object.is_string()){
            context.error = {ErrorType::INVALID_JSON_TYPE, "expected string, received '" + to_string(object) + '\''};
            std::cerr << "<ERROR> at json_get_string: " << context.error << '\n';
            return {};
        }
    }

    static Vector2 json_get_Vector2(Context& context, const Json& object){
        if(!object.contains("x") || !object.contains("y")){
            context.error = {ErrorType::INVALID_JSON_OBJECT_STRUCTURE, "expected Vector2-like object, which needs both `x` and `y` fields"};
            std::cerr << "<ERROR> at json_get_Vector2: " << context.error << '\n';
            return VEC2_NULL;
        }
        const Json& xComponent = object.at("x");
        const Json& yComponent = object.at("y");
        if(!xComponent.is_number() || !yComponent.is_number()){
            context.error = {ErrorType::INVALID_JSON_TYPE, "expected number, received x: '" + to_string(xComponent) + "' and y: '" + to_string(yComponent) + '\''};
            std::cerr << "<ERROR> at json_get_Vector2: " << context.error << '\n';
            return VEC2_NULL;
        }
        return Vector2{xComponent.get<float>(), yComponent.get<float>()};
    }

    static void init_level_data(Context& context, LevelRegistry& registry, const Json& levelDict){
        std::string levelName = "<unnamed>";
        Vector2 playerPos;
        Vector2 goalPos;
        Vector2 cameraPos;
        bool isCameraAtPlayer = true;

        if(!levelDict.contains("player_position")){
            context.error = {ErrorType::SETTING_NOT_FOUND, "No `player_position` setting found in level"};
            std::cerr << "<ERROR> at init_level_data: " << context.error << '\n';
            return;
        }
        if(!levelDict.contains("goal_position")){
            context.error = {ErrorType::SETTING_NOT_FOUND, "No `goal_position` setting found in level"};
            std::cerr << "<ERROR> at init_level_data: " << context.error << '\n';
            return;
        }
        playerPos = json_get_Vector2(context, levelDict.at("player_position"));
        if(context.error){
            std::cerr << "\tfrom init_level_data (getting `player_position`)\n";
            return;
        }
        goalPos = json_get_Vector2(context, levelDict.at("goal_position"));
        if(context.error){
            std::cerr << "\tfrom init_level_data (getting `goal_position`)\n";
            return;
        }

        if(levelDict.contains("camera_position")){
            isCameraAtPlayer = false;
            cameraPos = json_get_Vector2(context, levelDict.at("camera_position"));
            if(context.error){
                std::cerr << "\tfrom init_level_data (getting `camera_position`)\n";
                return;
            }
        }

        if(levelDict.contains("level_name")){
            levelName = json_get_string(context, levelDict.at("level_name"));
            if(context.error){
                std::cerr << "\tfrom init_level_data (getting `level_name`)\n";
                return;
            }
        }

        // TODO: do something with level name
        if(isCameraAtPlayer){
            registry.init_level(Position{playerPos}, Position{goalPos});
        } else {
            registry.init_level(Position{playerPos}, Position{goalPos}, Position{cameraPos});
        }
    }

    static void load_level_entity_from_json(Context& context, LevelRegistry& registry, const std::string& entityName, const Json& entityObj){
        if(!entityObj.contains("entity_default")){
            // TODO: only load components
        } else {
            std::string entityDefault = json_get_string(context, entityObj.at("entity_default"));
            if(context.error){
                std::cerr << "\tfrom load_level_entity_from_json (getting `entityDefault`)";
            }
            if(entityDefault == "static_body"){
                // TODO: do static body stuff
            } else if(entityDefault == "tilemap"){
                // TODO: do tilemap stuff
            } else if(entityDefault == "none"){
                // TODO: only load components, same thing as if there weren't an entity_default field
            }
        }
    }

    static void load_level_entities(Context& context, LevelRegistry& registry, const Json& levelDict){
        if(!levelDict.contains("entities")){
            std::cerr << "<WARNING> at load_level_entities: no `entities` field. Level will be empty\n";
            return;
        }
        const Json& entities = levelDict.at("entities");
        if(!entities.is_object()){
            context.error = {ErrorType::INVALID_JSON_TYPE, "expected dictionary for field `entities`, received '" + to_string(entities) + '\''};
            std::cerr << "<ERROR> at load_level_entities: " << context.error;
            return;
        }
        for(const auto& [entityName, entityObj] : entities.items()){
            load_level_entity_from_json(context, registry, entityName, entityObj);
            if(context.error){
                std::cerr << "\tfrom load_level_entities (entity name: '" << entityName << "')\n";
                return;
            }
        }
    }

    static void iterate_level_keys(Context& context, LevelRegistry& registry, const Json& levelDict){
        init_level_data(context, registry, levelDict);
        if(context.error){
            std::cerr << "\tfrom iterate_level_keys\n";
            return;
        }
        load_level_entities(context, registry, levelDict);
        if(context.error){
            std::cerr << "\tfrom iterate_level_keys\n";
            return;
        }
    }

    void build_level(Context& context, LevelRegistry& registry){
        Json levelObject = context.topLevelJsonObject;
        if(!levelObject.is_object()){
            context.error = {ErrorType::INVALID_JSON_TYPE, "Top-level JSON must be of type Object."};
            std::cerr << "<ERROR> at build_level: " << context.error << '\n';
            return;
        }
        iterate_level_keys(context, registry, levelObject);
        if(context.error){
            std::cerr << "\tfrom build_level\n";
            return;
        }
    }
}

