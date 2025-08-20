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

    static void load_entity_component_generic(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
        if(!componentObj.is_object()){
            context.error = {ErrorType::INVALID_JSON_TYPE, "expected object for `components` array element, found '" + to_string(componentObj) + '\''};
            std::cerr << "<ERROR> at load_entity_component_generic: " << context.error << '\n';
            return;
        }
        if(!componentObj.contains("type")){
            context.error = {ErrorType::SETTING_NOT_FOUND, "No `type` field found in component"};
            std::cerr << "<ERROR> at load_entity_component_generic: " << context.error << '\n';
            return;
        }
        std::string componentType = json_get_string(context, componentObj.at("type"));
        if(context.error){
            std::cerr << "\tfrom load_entity_component_generic\n";
            return;
        }
        if(componentType == "Position" || componentType == "Velocity" || componentType == "Acceleration"){
            // TODO: all of this. Maybe i should use an std::unordered map or similar to check the string value, there are a _lot_ of components
        } else if(componentType == "SpriteSheet"){
            
        } else if(componentType == "SpriteTransform"){

        } else if(componentType == "BoundingBoxComponent"){

        } else if(1/*...*/){

        } else {
            context.error = {ErrorType::INVALID_SETTING_VALUE, "invalid component type '" + componentType + '\''};
            std::cerr << "<ERROR> at load_entity_component_generic: " << context.error << '\n';
            return;
        }
    }

    static void load_entity_components_from_json(Context& context, LevelRegistry& registry, const Json& entityObj, entt::entity entityID, 
                                                 const std::string& entityName){
        if(!entityObj.contains("components")){
            std::cerr << "<WARNING> at load_entity_components_from_json (at entity with name " + entityName + "): No `components` field found for entity. Entity will have no additional components\n";
            return; 
        }
        const Json& componentArray = entityObj.at("components");
        if(!componentArray.is_array()){
            context.error = {ErrorType::INVALID_JSON_TYPE, "expected array for field `components`, found '" + to_string(componentArray) + '\''};
            std::cerr << "<ERROR> at load_entity_components_from_json: " << context.error << '\n';
            return;
        }
        size_t i = 0;
        for(const Json& componentObj : componentArray){
            load_entity_component_generic(context, registry, componentObj, entityID);
            if(context.error){
                std::cerr << "\tfrom load_entity_components_from_json (at component index " + std::to_string(i) + ")\n";
                return;
            }
        }
    }

    static void load_level_entity_from_json(Context& context, LevelRegistry& registry, const std::string& entityName, const Json& entityObj){
        if(!entityObj.contains("entity_default")){
            entt::entity currEntity = registry.new_entity(entityName);
            load_entity_components_from_json(context, registry, entityObj, currEntity, entityName);
            if(context.error){
                std::cerr << "\tfrom load_level_entity_from_json (entity name: " + entityName + ")\n";
                return;
            }
        } else {
            std::string entityDefault = json_get_string(context, entityObj.at("entity_default"));
            entt::entity currEntity = entt::null;
            if(context.error){
                std::cerr << "\tfrom load_level_entity_from_json (getting `entityDefault`)\n";
                return;
            }
            if(entityDefault == "static_body"){
                // TODO: do static body stuff
            } else if(entityDefault == "tilemap"){
                // TODO: do tilemap stuff
            } else if(entityDefault == "none"){
                // TODO: only load components, same thing as if there weren't an entity_default field
            } else {
                context.error = {ErrorType::INVALID_SETTING_VALUE, "Invalid value '" + entityDefault + "' for `entity_default`"};
                std::cerr << "<ERROR> at load_level_entity_from_json: " << context.error << '\n';
                return;
            }
            load_entity_components_from_json(context, registry, entityObj, currEntity, entityName);
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
            std::cerr << "<ERROR> at load_level_entities: " << context.error << '\n';
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

