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

    static float json_get_float(Context& context, const Json& object){
        if(!object.is_number()){
            THROW_ERROR_RETURN(
                ErrorType::INVALID_JSON_TYPE,
                "expected string, received '" + to_string(object) + '\'',
                json_get_string,
                std::numeric_limits<float>::quiet_NaN()
            );
        }
        return object.get<float>();
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
        return object.get<std::string>();
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

    static void load_bounding_box_component_auto(Context& context, LevelRegistry& registry, entt::entity entityID){
        CollisionComponent* collision = registry.get().try_get<CollisionComponent>(entityID);
        SpriteSheet* sprite = registry.get().try_get<SpriteSheet>(entityID);
        if(collision != nullptr && sprite != nullptr){
            BoundingBoxComponent bb = bb_union(
                calculate_bb(*sprite),
                calculate_bb(*collision)
            );
            registry.get().emplace_or_replace<BoundingBoxComponent>(entityID, bb);
        } else if(collision != nullptr){
            registry.get().emplace_or_replace<BoundingBoxComponent>(entityID, calculate_bb(*collision));
        } else if(sprite != nullptr){
            registry.get().emplace_or_replace<BoundingBoxComponent>(entityID, calculate_bb(*sprite));
        } else {
            std::cerr << "<WARNING> at load_bounding_box_component_auto (entity ID " << (int)entityID << "): No SpriteSheet or CollisionComponent components found on entity. Entity will have a zero bounding box\n";
            registry.add_component<BoundingBoxComponent>(entityID, VEC2_ZERO, 0, 0); 
        }
    }

    static void load_bounding_box_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
        if(componentObj.contains("auto")){
            return load_bounding_box_component_auto(context, registry, entityID);
        }
        if(!componentObj.contains("position")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `position` field found in non-auto BoundingBoxComponent",
                load_bounding_box_component
            );
        }
        Vector2 bbPosition = VEC2_NULL;
        CHECK_ERROR(
            bbPosition = json_get_Vector2(context, componentObj.at("position"));,
            load_bounding_box_component (getting `position`)
        );
        if(!componentObj.contains("width")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `WIDTH` field found in non-auto BoundingBoxComponent",
                load_bounding_box_component
            );
        }
        if(!componentObj.contains("height")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `height` field found in non-auto BoundingBoxComponent",
                load_bounding_box_component
            );
        }
        float bbWidth; float bbHeight;
        CHECK_ERROR(
            bbWidth = json_get_float(context, componentObj.at("width"));,
            load_bounding_box_component (getting `width`)
        );
        CHECK_ERROR(
            bbHeight = json_get_float(context, componentObj.at("height"));,
            load_bounding_box_component (getting `height`)
        );
        registry.add_component<BoundingBoxComponent>(entityID, bbPosition, bbWidth, bbHeight);
    }

    using ComponentLoadingFunc = void(*)(Context&, LevelRegistry&, const Json&, entt::entity);
    static const std::map<std::string, ComponentLoadingFunc> COMPONENT_LOADING_FUNCTIONS = {
        {"BoundingBoxComponent", load_bounding_box_component},
    };

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
        auto itr = COMPONENT_LOADING_FUNCTIONS.find(componentType);
        if(itr == COMPONENT_LOADING_FUNCTIONS.end()){
            THROW_ERROR(
                ErrorType::INVALID_SETTING_VALUE,
                "invalid component type '" + componentType + '\'',
                load_entity_component_generic
            );
        } else {
            ComponentLoadingFunc componentLoader = itr->second;
            CHECK_ERROR(
                componentLoader(context, registry, componentObj, entityID);,
                load_entity_component_generic
            )
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

    static void get_collision_layer_array(Context& context, LevelRegistry& registry, const Json& layersJson, std::vector<LayerType>& layersVec){
        size_t idx = 0;
        for(const Json& item : layersJson){
            switch(item.type()){
              case Json::value_t::number_integer: {
                int64_t layerValue = item.get<int64_t>();
                if(layerValue <= 0 || layerValue > NUMBER_OF_LAYERS){
                    THROW_ERROR(
                        ErrorType::INVALID_SETTING_VALUE,
                        "Invalid layer value " + std::to_string(layerValue) + " at `layers` field index " + std::to_string(idx) + "(expected number between 1 and 16 or string 'player')",
                        get_collision_layer_array
                    );
                }
                layersVec.push_back((LayerType)layerValue);
                break;
              }
              case Json::value_t::string: {
                std::string stringValue = item.get<std::string>();
                if(stringValue == "player"){
                    layersVec.push_back(LevelRegistry::PLAYER_COLLISION_LAYER);
                } else {
                    THROW_ERROR(
                        ErrorType::INVALID_SETTING_VALUE,
                        "Invalid layer value '" + stringValue + "' at `layers` field index " + std::to_string(idx) + "(expected number between 1 and 16 or string 'player')",
                        get_collision_layer_array
                    );
                }
                break;
              }
              default:
                THROW_ERROR(
                    ErrorType::INVALID_JSON_TYPE,
                    "Expected layer number or string 'player' for `layers` field item at index " + std::to_string(idx) + ", found '" + to_string(item) + '\'',
                    get_collision_layer_array
                );
            }
            idx++;
        }
    }

    static void add_collision_line_to_component(Context& context, const Json& colliderJson, CollisionComponent& collision){
        if(!colliderJson.contains("from")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `from` field found on \"line\" type collider",
                add_collision_line_to_component
            );
        }
        if(!colliderJson.contains("to")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `to` field found on \"line\" type collider",
                add_collision_line_to_component
            );
        }
        Vector2 lineOrigin; Vector2 lineDestination;
        CHECK_ERROR(
            lineOrigin = json_get_Vector2(context, colliderJson.at("from"));,
            add_collision_line_to_component (getting `from`)
        );
        CHECK_ERROR(
            lineDestination = json_get_Vector2(context, colliderJson.at("to"));,
            add_collision_line_to_component (getting `to`)
        );
        collision.add_line(lineOrigin, lineDestination);
    }

    static void add_centered_collision_rect_to_component(Context& context, const Json& colliderJson, CollisionComponent& collision){
        if(!colliderJson.contains("width")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `width` field found on \"rect\" type collider",
                add_centered_collision_rect_to_component
            );
        }
        if(!colliderJson.contains("height")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `height` field found on \"rect\" type collider",
                add_centered_collision_rect_to_component
            );
        }
        float rectWidth; float rectHeight;
        CHECK_ERROR(
            rectWidth = json_get_float(context, colliderJson.at("width"));,
            add_centered_collision_rect_to_component (getting `width`)
        );
        CHECK_ERROR(
            rectHeight = json_get_float(context, colliderJson.at("height"));,
            add_centered_collision_rect_to_component (getting `height`)
        );
        collision.add_rect_centered(rectWidth, rectHeight);
    }

    static void add_collision_rect_to_component(Context& context, const Json& colliderJson, const Vector2& colliderPosition, CollisionComponent& collision){
        if(!colliderJson.contains("width")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `width` field found on \"rect\" type collider",
                add_collision_rect_to_component
            );
        }
        if(!colliderJson.contains("height")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `height` field found on \"rect\" type collider",
                add_collision_rect_to_component
            );
        }
        float rectWidth; float rectHeight;
        CHECK_ERROR(
            rectWidth = json_get_float(context, colliderJson.at("width"));,
            add_collision_rect_to_component (getting `width`)
        );
        CHECK_ERROR(
            rectHeight = json_get_float(context, colliderJson.at("height"));,
            add_collision_rect_to_component (getting `height`)
        );
        collision.add_rect(rectWidth, rectHeight, colliderPosition);
    }

    static void add_collision_circle_to_component(Context& context, const Json& colliderJson, const Vector2& colliderPosition, CollisionComponent& collision){
        if(!colliderJson.contains("radius")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `radius` field found on \"circle\" type collider",
                add_collision_circle_to_component
            );
        }
        float circleRadius;
        CHECK_ERROR(
            circleRadius = json_get_float(context, colliderJson.at("radius"));,
            add_collision_circle_to_component (getting `radius`)
        );
        collision.add_circle(circleRadius, colliderPosition);
    }

    static void add_collision_barrier_to_component(Context& context, const Json& colliderJson, const Vector2& colliderPosition, CollisionComponent& collision){
        if(!colliderJson.contains("normal")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `normal` field found on \"barrier\" type collider",
                add_collision_barrier_to_component
            );
        }
        Vector2 barrierNormal;
        CHECK_ERROR(
            barrierNormal = json_get_Vector2(context, colliderJson.at("normal"));,
            add_collision_barrier_to_component (getting `normal`)
        );
        barrierNormal = {-barrierNormal.y, barrierNormal.x};
        collision.add_barrier(colliderPosition, barrierNormal);
    }

    static void load_collision_shape_into_component(Context& context, const Json& colliderJson, CollisionComponent& collision){
        if(!colliderJson.contains("type")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `type` found on collider object. `type` must be one of: \"rect\", \"circle\", \"barrier\" or \"line\"",
                load_collision_shape_into_component
            );
        }
        std::string colliderType;
        CHECK_ERROR(
            colliderType = json_get_string(context, colliderJson.at("type"));,
            load_collision_shape_into_component (getting `type` field)
        );
        if(colliderType == "line"){
            CHECK_ERROR(
                add_collision_line_to_component(context, colliderJson, collision);,
                load_collision_shape_into_component
            );
        } else if(colliderJson.contains("centered")){
            CHECK_ERROR(
                add_centered_collision_rect_to_component(context, colliderJson, collision);,
                load_collision_shape_into_component
            );
        } else {
            Vector2 colliderPosition = VEC2_ZERO;
            if(colliderJson.contains("position")){
                CHECK_ERROR(
                    colliderPosition = json_get_Vector2(context, colliderJson.at("position"));,
                    load_collision_shape_into_component (getting `position` field)
                );
            }
            void (*collider_loading_function)(Context&, const Json&, const Vector2&, CollisionComponent&) = nullptr;

            if(colliderType == "rect"){ // does g++ optimize this if-else chain + function call? good fuckin question!
                collider_loading_function = add_collision_rect_to_component;
            } else if(colliderType == "circle"){
                collider_loading_function = add_collision_circle_to_component;
            } else if(colliderType == "barrier"){
                collider_loading_function = add_collision_barrier_to_component;
            } else {
                THROW_ERROR(
                    ErrorType::INVALID_SETTING_VALUE,
                    "Invalid value '" + colliderType + "' for collider `type`",
                    load_collision_shape_into_component
                );
            }

            CHECK_ERROR(
                collider_loading_function(context, colliderJson, colliderPosition, collision);,
                load_collision_shape_into_component
            )
        }
    }

    static void load_entity_static_body_colliders(Context& context, const Json& entityObj, entt::entity entityID, CollisionComponent& entityCollision){
        if(!entityObj.contains("colliders")){
            std::cerr << "<WARNING> at load_entity_static_body_colliders: no `colliders` field. Static body will be collisionless\n";
            return;
        }
        const Json& collidersJson = entityObj.at("colliders");
        if(!collidersJson.is_array()){
            THROW_ERROR(
                ErrorType::INVALID_JSON_TYPE,
                "expected number array for field `colliders`, found '" + to_string(collidersJson) + '\'',
                load_entity_static_body_colliders
            );
        }
        size_t colliderIdx = 0;
        for(const Json& collider : collidersJson){
            CHECK_ERROR_STR(
                load_collision_shape_into_component(context, collider, entityCollision);,
                "load_entity_static_body_colliders (collider index: " + std::to_string(colliderIdx) + ')'
            );
        }
    }

    static void load_entity_static_body_settings(Context& context, LevelRegistry& registry, const Json& entityObj, entt::entity& entityID, 
                                                 const std::string& entityName){
        Vector2 positionVector = VEC2_NULL;
        std::vector<LayerType> layers;
        CHECK_ERROR_STR(
            positionVector = json_get_Vector2(context, entityObj);,
            "load_entity_static_body_settings (getting position, entity name: " + entityName + ")"
        );
        if(!entityObj.contains("layers")){
            THROW_ERROR(
                ErrorType::SETTING_NOT_FOUND,
                "No `layers` field found on static body",
                load_entity_static_body_settings
            );
        }
        const Json& layersJson = entityObj.at("layers");
        if(!layersJson.is_array()){
            THROW_ERROR(
                ErrorType::INVALID_JSON_TYPE,
                "expected number array for field `layers`, found '" + to_string(layersJson) + '\'',
                load_entity_static_body_settings
            );
        }
        CHECK_ERROR(
            get_collision_layer_array(context, registry, layersJson, layers);,
            load_entity_static_body_settings
        );
        auto [temp, entityCollision] = registry.create_static_body(positionVector, std::move(layers));
        entityID = temp;
        CHECK_ERROR(
            load_entity_static_body_colliders(context, entityObj, entityID, entityCollision);,
            load_entity_static_body_settings
        )

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
                CHECK_ERROR(
                    load_entity_static_body_settings(context, registry, entityObj, currEntity, entityName);,
                    load_level_entity_from_json
                );
            } else if(entityDefault == "tilemap"){
                // TODO: do tilemap stuff
            } else if(entityDefault == "none"){
                currEntity = registry.new_entity(entityName);
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

