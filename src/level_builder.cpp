#include"level_builder.h"
#include<sstream>

#define THROW_ERROR(errorType, errorMsg, location) context.error = {errorType, errorMsg}; \
std::cerr << "<ERROR> at " #location ": " << context.error << '\n'; \
return;

#define THROW_ERROR_RETURN(errorType, errorMsg, location, ret) context.error = {errorType, errorMsg}; \
std::cerr << "<ERROR> at " #location ": " << context.error << '\n'; \
return (ret);

#define CHECK_ERROR(statement, location) statement; \
if(context.error){\
    std::cerr << "\tfrom " #location "\n";\
    return;\
}

#define CHECK_ERROR_STR(statement, locationStr) statement; \
if(context.error){\
    std::cerr << "\tfrom " + std::string(locationStr) + "\n"; \
    return; \
}

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
            THROW_ERROR_RETURN(
                ErrorType::INVALID_JSON_TYPE, 
                "expected string, received '" + to_string(object) + '\'', 
                json_get_string, 
                ""
            );
        }
    }

    static Vector2 json_get_Vector2(Context& context, const Json& object){
        if(!object.contains("x") || !object.contains("y")){
            THROW_ERROR_RETURN(
                ErrorType::INVALID_JSON_OBJECT_STRUCTURE, 
                "expected Vector2-like object, which needs both `x` and `y` fields",
                json_get_Vector2, 
                VEC2_NULL
            );
        }
        const Json& xComponent = object.at("x");
        const Json& yComponent = object.at("y");
        if(!xComponent.is_number() || !yComponent.is_number()){
            THROW_ERROR_RETURN(
                ErrorType::INVALID_JSON_TYPE,
                "expected number, received x: '" + to_string(xComponent) + "' and y: '" + to_string(yComponent) + '\'',
                json_get_Vector2,
                VEC2_NULL
            );
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
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `player_position` setting found in level",
                init_level_data
            );
        }
        if(!levelDict.contains("goal_position")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `goal_position` setting found in level",
                init_level_data
            );
        }
        CHECK_ERROR(
            playerPos = json_get_Vector2(context, levelDict.at("player_position"));,
            init_level_data (getting `player_position`)
        );
        CHECK_ERROR(
            goalPos = json_get_Vector2(context, levelDict.at("goal_position"));,
            init_level_data (getting `player_position`)
        );

        if(levelDict.contains("camera_position")){
            isCameraAtPlayer = false;
            CHECK_ERROR(
                cameraPos = json_get_Vector2(context, levelDict.at("camera_position"));,
                init_level_data (getting `camera_position`)
            );
        }

        if(levelDict.contains("level_name")){
            CHECK_ERROR(
                levelName = json_get_string(context, levelDict.at("level_name"));,
                init_level_data (getting `level_name`)
            );
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
            THROW_ERROR(
                ErrorType::INVALID_JSON_TYPE,
                "expected object for `components` array element, found '" + to_string(componentObj) + '\'',
                load_entity_component_generic
            );
        }
        if(!componentObj.contains("type")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `type` field found in component",
                load_entity_component_generic
            );
        }
        CHECK_ERROR(
            std::string componentType = json_get_string(context, componentObj.at("type"));,
            load_entity_component_generic
        );
        if(componentType == "Position" || componentType == "Velocity" || componentType == "Acceleration"){
            // TODO: all of this. Maybe i should use an std::unordered map or similar to check the string value, there are a _lot_ of components
        } else if(componentType == "SpriteSheet"){
            
        } else if(componentType == "SpriteTransform"){

        } else if(componentType == "BoundingBoxComponent"){

        } else if(1/*...*/){

        } else {
            THROW_ERROR(
                ErrorType::INVALID_SETTING_VALUE,
                "invalid component type '" + componentType + '\'',
                load_entity_component_generic
            );
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
            THROW_ERROR(
                ErrorType::INVALID_JSON_TYPE,
                "expected array for field `components`, found '" + to_string(componentArray) + '\'',
                load_entity_components_from_json
            );
        }
        size_t i = 0;
        for(const Json& componentObj : componentArray){
            CHECK_ERROR_STR(
                load_entity_component_generic(context, registry, componentObj, entityID);,
                "load_entity_components_from_json (at component index " + std::to_string(i) + ")"
            );
        }
    }

    static void load_level_entity_from_json(Context& context, LevelRegistry& registry, const std::string& entityName, const Json& entityObj){
        if(!entityObj.contains("entity_default")){
            entt::entity currEntity = registry.new_entity(entityName);
            CHECK_ERROR_STR(
                load_entity_components_from_json(context, registry, entityObj, currEntity, entityName);,
                "load_level_entity_from_json (entity name: " +  entityName + ")" 
            );
        } else {
            entt::entity currEntity = entt::null;
            CHECK_ERROR(
                std::string entityDefault = json_get_string(context, entityObj.at("entity_default"));,
                load_level_entity_from_json (getting `entity_default`)
            );
            if(entityDefault == "static_body"){
                // TODO: do static body stuff
            } else if(entityDefault == "tilemap"){
                // TODO: do tilemap stuff
            } else if(entityDefault == "none"){
                // TODO: only load components, same thing as if there weren't an entity_default field
            } else {
                THROW_ERROR(
                    ErrorType::INVALID_SETTING_VALUE,
                    "Invalid value '" + entityDefault + "' for `entity_default`",
                    load_level_entity_from_json
                );
            }
            CHECK_ERROR_STR(
                load_entity_components_from_json(context, registry, entityObj, currEntity, entityName);,
                "load_level_entity_from_json (entity name: " + entityName + ")"
            );
        }
    }

    static void load_level_entities(Context& context, LevelRegistry& registry, const Json& levelDict){
        if(!levelDict.contains("entities")){
            std::cerr << "<WARNING> at load_level_entities: no `entities` field. Level will be empty\n";
            return;
        }
        const Json& entities = levelDict.at("entities");
        if(!entities.is_object()){
            THROW_ERROR(
                ErrorType::INVALID_JSON_TYPE,
                "expected dictionary for field `entities`, received '" + to_string(entities) + '\'',
                load_level_entities
            );
        }
        for(const auto& [entityName, entityObj] : entities.items()){
            CHECK_ERROR_STR(
                load_level_entity_from_json(context, registry, entityName, entityObj);,
                "load_level_entities (entity name: '" + entityName + "')"
            );
        }
    }

    static void iterate_level_keys(Context& context, LevelRegistry& registry, const Json& levelDict){
        CHECK_ERROR(
            init_level_data(context, registry, levelDict);,
            iterate_level_keys
        );
        CHECK_ERROR(
            load_level_entities(context, registry, levelDict);,
            iterate_level_keys
        );
    }

    void build_level(Context& context, LevelRegistry& registry){
        Json levelObject = context.topLevelJsonObject;
        if(!levelObject.is_object()){
            THROW_ERROR(
                ErrorType::INVALID_JSON_TYPE,
                "Top-level JSON must be of type object.",
                build_level
            );
        }
        CHECK_ERROR(
            iterate_level_keys(context, registry, levelObject);,
            build_level
        );
    }
}

