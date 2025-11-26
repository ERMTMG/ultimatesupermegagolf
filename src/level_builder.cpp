#include"level_builder.h"
#include "basic_components.h"
#include "collision_component.h"
#include "sound_component.h"
#include "particle_generator.h"
#include "bounding_box.h"
#include "level_registry.h"
#include "raylib.h"
#include "utility.h"
#include "utility/random_range.h"
#include "utility/vector2_util.h"
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>
#include<unordered_set>
#include<unordered_map>

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

#define CHECK_ERROR_RETURN(statement, location, ret) statement; \
if(context.error){ \
        std::cerr << "\tfrom " #location "\n"; \
        return (ret); \
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
            .entityNames = {},
            .tilesets = {},
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
            .entityNames = {},
            .tilesets = {},
            .error = {ErrorType::PARSE_ERROR, err.what()},
            .topLevelJsonObject = Json{Json::value_t::null}
        };
    }
    return Context {
        .jsonFile = std::move(file),
        .entityNames = {},
        .tilesets = {},
        .error = {ErrorType::SUCCESS},
        .topLevelJsonObject = levelJson
    };
}

static size_t json_get_pos_int(Context& context, const Json& object){
    if(!object.is_number_unsigned()){
        THROW_ERROR_RETURN(
            ErrorType::INVALID_JSON_TYPE,
            "expected non-negative integer, received '" + to_string(object) + '\'',
            json_get_pos_int,
            -1
        );
    }
    return object.get<size_t>();
}

static int json_get_int(Context& context, const Json& object){
    if(!object.is_number_integer()){
        THROW_ERROR_RETURN(
            ErrorType::INVALID_JSON_TYPE,
            "expected integer, received '" + to_string(object) + '\'',
            json_get_pos_int,
            std::numeric_limits<int>::max()
        );
    }
    return object.get<int>();
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

static Color json_get_color(Context& context, const Json& object){
    static const Color ERROR_COLOR = Color{0,0,0,0};
    std::string str;
    CHECK_ERROR_RETURN(
        str = json_get_string(context, object);, 
        json_get_color, 
        ERROR_COLOR
    );
    if(str.empty() || str[0] != '#'){
        THROW_ERROR_RETURN(
            ErrorType::INVALID_SETTING_VALUE, 
            "Color code on string has invalid syntax (must start with '#' character)", 
            json_get_color,
            ERROR_COLOR
        );
    }
    char* outPtr;
    unsigned int colorCode = strtoul(str.c_str() + 1, &outPtr, 16);
    if(outPtr != nullptr){
        THROW_ERROR_RETURN(
            ErrorType::INVALID_SETTING_VALUE, 
            "Color code on string has invalid syntax (not a valid hexadecimal value)",
            json_get_color, 
            ERROR_COLOR
        );
    }
    return GetColor(colorCode);
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

static std::pair<float, float> json_get_width_and_height(Context& context, const Json& object){
    static constexpr std::pair<float, float> ERROR_PAIR = {std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
    if(!object.contains("width")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND,
            "No `width` setting found in object",
            json_get_width_and_height,
            ERROR_PAIR
        );
    }
    if(!object.contains("height")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND,
            "No `height` setting found in object",
            json_get_width_and_height,
            ERROR_PAIR
        );
    }
    float width;
    float height;
    CHECK_ERROR_RETURN(
        width = json_get_float(context, object.at("width"));,
        json_get_width_and_height (getting `width`),
        ERROR_PAIR
    );
    CHECK_ERROR_RETURN(
        height = json_get_float(context, object.at("height"));,
        json_get_width_and_height (getting `height`),
        ERROR_PAIR
    );
    return {width, height};
}

static ColorRange json_get_color_range(Context& context, const Json& object){
    static const ColorRange ERROR_RANGE = ColorRange{Color{0,0,0,0}, Color{0,0,0,0}};
    if(!object.contains("from")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `from` color found in color range", 
            json_get_color_range, 
            ERROR_RANGE
        );
    }
    if(!object.contains("to")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `to` color found in color range", 
            json_get_color_range, 
            ERROR_RANGE
        );
    }
    Color from;
    Color to;
    CHECK_ERROR_RETURN(
        from = json_get_color(context, object.at("from"));, 
        json_get_color_range, 
        ERROR_RANGE
    );
    CHECK_ERROR_RETURN(
        to = json_get_color(context, object.at("to"));,
        json_get_color_range,
        ERROR_RANGE
    );
    return ColorRange{from, to};
}

static Vec2Range json_get_Vector2_range(Context& context, const Json& object){
    static const Vec2Range ERROR_RANGE = Vec2Range{VEC2_NULL, VEC2_NULL};
    if(!object.contains("min")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `min` found in Vector2 range", 
            json_get_Vector2_range, 
            ERROR_RANGE
        );
    }
    if(!object.contains("max")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `max` found in Vector2 range", 
            json_get_Vector2_range, 
            ERROR_RANGE
        );
    }
    Vector2 min;
    Vector2 max;
    CHECK_ERROR_RETURN(
        min = json_get_Vector2(context, object.at("min"));, 
        json_get_Vector2_range, 
        ERROR_RANGE
    );
    CHECK_ERROR_RETURN(
        max = json_get_Vector2(context, object.at("max"));, 
        json_get_Vector2_range,
        ERROR_RANGE
    )
    return Vec2Range{min, max};
}

static FloatRange json_get_float_range(Context& context, const Json& object){ 
    static const FloatRange ERROR_RANGE = FloatRange{std::numeric_limits<float>::quiet_NaN(), std::numeric_limits<float>::quiet_NaN()};
    if(!object.contains("min")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `min` found in float range", 
            json_get_float_range, 
            ERROR_RANGE
        );
    }
    if(!object.contains("max")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `max` found in float range", 
            json_get_float_range, 
            ERROR_RANGE
        );
    }
    float min;
    float max;
    CHECK_ERROR_RETURN(
        min = json_get_float(context, object.at("min"));, 
        json_get_float_range,
        ERROR_RANGE
    );
    CHECK_ERROR_RETURN(
        max = json_get_float(context, object.at("max"));, 
        json_get_float_range, 
        ERROR_RANGE
    );
    return FloatRange{min, max};
}

static IntRange json_get_int_range(Context& context, const Json& object){
    static const IntRange ERROR_RANGE = IntRange{INT32_MIN, INT32_MIN};
    if(!object.contains("min")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `min` found in int range", 
            json_get_int_range, 
            ERROR_RANGE
        );
    }
    if(!object.contains("max")){
        THROW_ERROR_RETURN(
            ErrorType::SETTING_NOT_FOUND, 
            "No `max` found in int range", 
            json_get_int_range, 
            ERROR_RANGE
        );
    }
    int min;
    int max;
    CHECK_ERROR_RETURN(
        min = json_get_int(context, object.at("min"));, 
        json_get_int_range,
        ERROR_RANGE
    );
    CHECK_ERROR_RETURN(
        max = json_get_int(context, object.at("max"));, 
        json_get_int_range, 
        ERROR_RANGE
    );
    return IntRange{min, max};
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

static ParticleSettings load_particle_settings_from_json(Context& context, const Json& particleSettingsDict){
    throw 1;
}

static void load_particle_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    if(!componentObj.contains("settings")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND, 
            "No `settings` field found in ParticleGenerator component", 
            load_particle_component
        )
    }
    const Json& settingsDict = componentObj.at("settings");
    if(!settingsDict.is_object()){
        THROW_ERROR(
            ErrorType::INVALID_JSON_TYPE, 
            "Expected dictionary for settings, received '" + to_string(settingsDict) + '\'',
            load_particle_component
        );
    }

}

static void load_sound_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    if(!componentObj.contains("sounds")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND, 
            "No `sounds` field found in SoundComponent component", 
            load_sound_component
        );
    }
    const Json& soundsDict = componentObj.at("sounds");
    if(!soundsDict.is_object()){
        THROW_ERROR(
            ErrorType::INVALID_JSON_TYPE, 
            "Expected dictionary for sounds, received '" + to_string(soundsDict) + '\'',
            load_sound_component
        );
    }
    registry.add_component<SoundComponent>(entityID);
    SoundComponent& sound = *registry.get_component<SoundComponent>(entityID);
    for(const auto&[soundKey, soundFilenameJson] : soundsDict.items()){
        std::string soundFilename;
        CHECK_ERROR(
            soundFilename = json_get_string(context, soundFilenameJson);,
            load_sound_component
        );
        add_sound_to_component(sound, soundFilename.c_str(), to_key(soundKey));
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
        registry.add_component<BoundingBoxComponent>(entityID, VEC2_ZERO, 0.f, 0.f);
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
    float bbWidth; float bbHeight;
    CHECK_ERROR(
        std::tie(bbWidth, bbHeight) = json_get_width_and_height(context, componentObj);,
        load_bounding_box_component (getting `width` and `height`)
    );
    registry.add_component<BoundingBoxComponent>(entityID, bbPosition, bbWidth, bbHeight);
}

static void load_spritesheet_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    if(!componentObj.contains("texture")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "No `texture` field found in SpriteSheet component",
            load_spritesheet_component
        );
    }
    std::string textureFilename;
    CHECK_ERROR(
        textureFilename = json_get_string(context, componentObj.at("texture"));,
        load_spritesheet_component (getting `texture`)
    );

    float width; float height;
    CHECK_ERROR(
        std::tie(width, height) = json_get_width_and_height(context, componentObj);,
        load_spritesheet_component
    );
    if(width <= 0){
        THROW_ERROR(
            ErrorType::INVALID_SETTING_VALUE,
            "SpriteSheet component `width` must be a positive number",
            load_spritesheet_component
        );
    }
    if(height <= 0){
        THROW_ERROR(
            ErrorType::INVALID_SETTING_VALUE,
            "SpriteSheet component `height` must be a positive number",
            load_spritesheet_component
        );
    }
    registry.add_component<SpriteSheet>(entityID, textureFilename.c_str(), (uint)width, (uint)height);
    // TODO: do something to configure animation lengths
}

static void load_sprite_transform_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    Vector2 scale = VEC2_ONE;
    Vector2 offset = VEC2_ZERO;
    float rotation = 0;
    SpriteSheet* sprite = registry.get_component<SpriteSheet>(entityID);
    if(sprite == nullptr){
        THROW_ERROR(
            ErrorType::NEEDED_COMPONENT_NOT_FOUND,
            "SpriteTransform component needs to have a corresponding SpriteSheet component (did you load it *before* the SpriteTransform?)",
            load_sprite_transform_component
        );
    }
    if(componentObj.contains("fit_to_width_and_height")){
        float targetWidth;
        float targetHeight;
        CHECK_ERROR(
            std::tie(targetWidth, targetHeight) = json_get_width_and_height(context, componentObj);,
            load_sprite_transform_component (note: presence of field `fit_to_width_and_height` requires a corresponding `width` and `height` field)
        );
        float spriteWidth = sprite->texture.width / sprite->numberFramesPerRow;
        float spriteHeight = sprite->texture.height / sprite->numberRows;
        scale = Vector2 {targetWidth / spriteWidth, targetHeight / spriteHeight};
    } else if(componentObj.contains("scale")){
        CHECK_ERROR(
            scale = json_get_Vector2(context, componentObj.at("scale"));,
            load_sprite_transform_component (getting `scale`)
        );
    }
    if(componentObj.contains("offset")){
        CHECK_ERROR(
            offset = json_get_Vector2(context, componentObj.at("offset"));,
            load_sprite_transform_component (getting `offset`)
        );
    }
    if(componentObj.contains("rotation")){
        CHECK_ERROR(
            rotation = json_get_float(context, componentObj.at("rotation"));,
            load_sprite_transforma component (getting `rotation`)
        )
    }
    registry.add_component<SpriteTransform>(entityID, offset, scale, rotation);
}

static void load_position_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    if(!componentObj.contains("position")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `position` field found in Position component",
            load_position_component
        );
    }
    Vector2 pos = VEC2_NULL;
    CHECK_ERROR(
        pos = json_get_Vector2(context, componentObj.at("position")),
        load_position_component
    );
    registry.add_component<Position>(entityID, pos);
}

static void load_velocity_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    if(!componentObj.contains("velocity")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `velocity` field found in Velocity component",
            load_velocity_component
        );
    }
    Vector2 vel = VEC2_NULL;
    CHECK_ERROR(
        vel = json_get_Vector2(context, componentObj.at("velocity")),
        load_velocity_component
    );
    registry.add_component<Velocity>(entityID, vel);
}

static void load_acceleration_component(Context& context, LevelRegistry& registry, const Json& componentObj, entt::entity entityID){
    if(!componentObj.contains("acceleration")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `acceleration` field found in Acceleration component",
            load_acceleration_component
        );
    }
    Vector2 accel = VEC2_NULL;
    CHECK_ERROR(
        accel = json_get_Vector2(context, componentObj.at("acceleration")),
        load_acceleration_component
    );
    registry.add_component<Acceleration>(entityID, accel);
}

using ComponentLoadingFunc = void(*)(Context&, LevelRegistry&, const Json&, entt::entity);
static const std::unordered_map<std::string, ComponentLoadingFunc> COMPONENT_LOADING_FUNCTIONS = {
    {"SoundComponent", &load_sound_component},
    {"BoundingBoxComponent", &load_bounding_box_component},
    {"SpriteSheet", &load_spritesheet_component},
    {"SpriteTransform", &load_sprite_transform_component},
    {"Position", &load_position_component},
    {"Velocity", &load_velocity_component},
    {"Acceleration", &load_acceleration_component},
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

    float rectWidth; float rectHeight;
    CHECK_ERROR(
        std::tie(rectWidth, rectHeight) = json_get_width_and_height(context, colliderJson);,
        add_centered_collision_rect_to_component (getting `width` and `height`)
    );
    collision.add_rect_centered(rectWidth, rectHeight);
}

static void add_collision_rect_to_component(Context& context, const Json& colliderJson, const Vector2& colliderPosition, CollisionComponent& collision){
    float rectWidth; float rectHeight;
    CHECK_ERROR(
        std::tie(rectWidth, rectHeight) = json_get_width_and_height(context, colliderJson);,
        add_centered_collision_rect_to_component (getting `width` and `height`)
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
    //barrierNormal = {-barrierNormal.y, barrierNormal.x};
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

static void load_entity_static_body_settings(Context& context, LevelRegistry& registry, const Json& entityObj, entt::entity entityID,
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
    CollisionComponent& entityCollision = registry.make_entity_into_static_body(entityID, positionVector, std::move(layers));
    CHECK_ERROR(
        load_entity_static_body_colliders(context, entityObj, entityID, entityCollision);,
        load_entity_static_body_settings
    )

}

static void load_tilemap_array(Context& context, const Json& tilemapArray, TilesetComponent& entityTilemap){
    if(!tilemapArray.is_array()){
        THROW_ERROR(
            ErrorType::INVALID_JSON_TYPE,
            "expected array of tile IDs for field `tilegrid`, found '" + to_string(tilemapArray) + '\'',
            load_tilemap_array
        );
    }
    if(tilemapArray.size() != entityTilemap.map.total_num_elements()){
        THROW_ERROR(
            ErrorType::INVALID_SETTING_VALUE,
            "expected array of size " + std::to_string(entityTilemap.map.total_num_elements()) + " for tilegrid value, found array of size " + std::to_string(tilemapArray.size()),
            load_tilemap_array
        );
    }
    size_t currentRow = 0;
    size_t currentCol = 0;
    size_t idx = 0;
    for(const auto& tileIDJson : tilemapArray){
        TileID tileID;
        CHECK_ERROR_STR(
            tileID = json_get_int(context, tileIDJson),
            "load_tilemap_array (at array index " + std::to_string(idx) + " corresponding to row " + std::to_string(currentRow) + ", col " + std::to_string(currentCol) + ')'
        );
        if(tileID >= entityTilemap.tiles.size() && tileID != -1){
            THROW_ERROR(
                ErrorType::INVALID_SETTING_VALUE,
                "`tilegrid` array at index " + std::to_string(idx) + " contains number that doesn't correspond to a valid tile",
                load_tilemap_array
            );
        }
        entityTilemap.map[currentRow][currentCol] = tileID;
        idx++;
        currentCol++;
        if(currentCol == entityTilemap.map.cols()){
            currentCol = 0;
            currentRow++;
        }
    }
}

static void load_entity_tilemap_settings(Context& context, LevelRegistry& registry, const Json& entityObj, entt::entity entityID,
                                         const std::string& entityName){
    Vector2 positionVector = VEC2_NULL;
    std::vector<LayerType> layers;
    CHECK_ERROR_STR(
        positionVector = json_get_Vector2(context, entityObj);,
        "load_entity_tilemap_settings (getting position, entity name: " + entityName + ")"
    );
    if(!entityObj.contains("layers")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "No `layers` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    const Json& layersJson = entityObj.at("layers");
    if(!layersJson.is_array()){
        THROW_ERROR(
            ErrorType::INVALID_JSON_TYPE,
            "expected number array for field `layers`, found '" + to_string(layersJson) + '\'',
            load_entity_tilemap_settings
        );
    }
    CHECK_ERROR(
        get_collision_layer_array(context, registry, layersJson, layers);,
        load_entity_tilemap_settings
    );
    auto [tilemap, collision] = registry.make_entity_into_tileamp(entityID, positionVector, std::move(layers));
    Vector2 tileSize = VEC2_NULL;
    if(!entityObj.contains("tile_width")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `tile_width` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    if(!entityObj.contains("tile_height")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `tile_height` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    CHECK_ERROR(
        tileSize.x = json_get_float(context, entityObj.at("tile_width")),
        load_entity_tilemap_settings (getting `tile_width`)
    );
    CHECK_ERROR(
        tileSize.y = json_get_float(context, entityObj.at("tile_height")),
        load_entity_tilemap_settings (getting `tile_height`)
    );
    tilemap.tileSize = tileSize;
    if(!entityObj.contains("tileset")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `tileset` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    std::string tilesetName;
    CHECK_ERROR(
        tilesetName = json_get_string(context, entityObj.at("tileset")),
        load_entity_tilemap_settings (getting `tileset`)
    );
    auto iter = context.tilesets.find(tilesetName);
    if(iter == context.tilesets.end()){
        THROW_ERROR(
            ErrorType::INVALID_SETTING_VALUE,
            "tileset with name '" + tilesetName + "' doesn't exist in current level context",
            load_entity_tilemap_settings
        );
    }
    const auto& tilesetTiles = iter->second;
    tilemap.tiles = tilesetTiles; // NOTE: copy assigned!
    if(!entityObj.contains("tilegrid_rows")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `tilegrid_rows` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    if(!entityObj.contains("tilegrid_cols")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `tilegrid_cols` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    size_t tilemapRows;
    size_t tilemapCols;
    CHECK_ERROR(
        tilemapRows = json_get_pos_int(context, entityObj.at("tilegrid_rows")),
        load_entity_tilemap_settings (getting `tilegrid_rows`)
    );
    CHECK_ERROR(
        tilemapCols = json_get_pos_int(context, entityObj.at("tilegrid_cols")),
        load_entity_tilemap_settings (getting `tilegrid_cols`)
    );
    tilemap.map.resize(tilemapRows, tilemapCols, (TileID)-1);
    if(!entityObj.contains("tilegrid")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `tilegrid` field found on tilemap entity",
            load_entity_tilemap_settings
        );
    }
    CHECK_ERROR(
        load_tilemap_array(context, entityObj.at("tilegrid"), tilemap);,
        load_entity_tilemap_settings
    );
    tileset_get_complete_collision(tilemap, collision);
}

static void load_level_entity_from_json(Context& context, LevelRegistry& registry, const std::string& entityName, const Json& entityObj){
    entt::entity currEntity = context.entityNames.at(entityName);
    if(!entityObj.contains("entity_default")){
        CHECK_ERROR_STR(
            load_entity_components_from_json(context, registry, entityObj, currEntity, entityName);,
            "load_level_entity_from_json (entity name: " +  entityName + ")"
            );
    } else {
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
            CHECK_ERROR(
                load_entity_tilemap_settings(context, registry, entityObj, currEntity, entityName),
                load_level_entity_from_json
            );
        } else if(entityDefault != "none"){
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

static bool is_entity_name_protected(const std::string& entityName){
    static const std::unordered_set<std::string> PROTECTED_NAMES = {
        LevelRegistry::PLAYER_ENTITY_NAME,
        LevelRegistry::GOAL_ENTITY_NAME,
        LevelRegistry::CAMERA_ENTITY_NAME,
        LevelRegistry::INPUT_MANAGER_ENTITY_NAME,
        LevelRegistry::RNG_ENTITY_NAME
    };
    return PROTECTED_NAMES.count(entityName) != 0;
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
    // first pass: collect entity names and create corresponding entity IDs
    auto& entityNamesMap = context.entityNames;
    for(const auto& [entityName, entityObj] : entities.items()){
        if(is_entity_name_protected(entityName)){
            THROW_ERROR(
                ErrorType::GENERIC_ERROR,
                "entity name " + entityName + " is protected, it can't be used as the name of an entity",
                load_level_entities
            );
        }
        entt::entity& entityID = entityNamesMap[entityName]; // this creates a new default constructed entityID (0) if entityNamesMap doesn't
                                                             // have an entity with this name. Since 0 is an impossible entity ID
                                                             // (it's already taken by the player), in this context it means a new entity.
        if(entityID != entt::entity{}){
            THROW_ERROR(
                ErrorType::GENERIC_ERROR,
                "Entity name " + entityName + " already taken on entityID " + std::to_string((int)entityID),
                load_level_entities
            );
        } else {
            entityID = registry.new_entity(entityName);
        }
    }
    // TODO: load all entity names beforehand and assign them entities. Perhaps with a new Context member variable
    // that holds a entityname -> entityID map?
    for(const auto& [entityName, entityObj] : entities.items()){
        CHECK_ERROR_STR(
            load_level_entity_from_json(context, registry, entityName, entityObj);,
            "load_level_entities (entity name: '" + entityName + "')"
        );
    }
}

static void load_tile_into_vector(Context& context, const Json& tileObj, std::vector<TilesetTile>& contextTileset){
    if(!tileObj.contains("texture")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `texture` field found in tile object",
            load_tile_into_vector
        );
    }
    std::string tileTextureFilename;
    CHECK_ERROR(
        tileTextureFilename = json_get_string(context, tileObj.at("texture")),
        load_tile_into_vector (getting `texture`)
    );
    if(!tileObj.contains("collision")){
        THROW_ERROR(
            ErrorType::SETTING_NOT_FOUND,
            "no `collision` field found in tile object",
            load_tile_into_vector
        );
    }
    std::string tileCollisionPresetName;
    CHECK_ERROR(
        tileCollisionPresetName = json_get_string(context, tileObj.at("collision"));,
        load_tile_into_vector (getting `collision`)
    );
    TilesetTile::TileCollisionPreset tileCollisionPreset;
    static const std::map<std::string, TilesetTile::TileCollisionPreset> TILE_COLLISION_PRESETS = {
        {"solid", TilesetTile::TileCollisionPreset::SOLID},
        {"none", TilesetTile::TileCollisionPreset::NONE},
        {"slope_nw_se", TilesetTile::TileCollisionPreset::SLOPE_NW_TO_SE},
        {"slope_sw_ne", TilesetTile::TileCollisionPreset::SLOPE_SW_TO_NE},
        {"circular", TilesetTile::TileCollisionPreset::CIRCULAR},
    };
    auto iter = TILE_COLLISION_PRESETS.find(tileCollisionPresetName);
    if(iter == TILE_COLLISION_PRESETS.end()){
        THROW_ERROR(
            ErrorType::INVALID_SETTING_VALUE,
            "Invalid tile collision preset '" + tileCollisionPresetName + '\'',
            load_tile_into_vector
        );
    } else {
        tileCollisionPreset = iter->second;
    }
    contextTileset.emplace_back(tileTextureFilename.c_str(), tileCollisionPreset);
}

static void load_tileset_into_context(Context& context, const std::string& tilesetName, const Json& tilesetTiles){
    if(!tilesetTiles.is_array()){
        THROW_ERROR(
            ErrorType::INVALID_JSON_TYPE,
            "expected array of tile objects for tileset, received '" + to_string(tilesetTiles) + '\'',
            load_tileset_into_component
        );
    }
    size_t numTiles = tilesetTiles.size();
    auto [iter, insertSucceeded] = context.tilesets.emplace(tilesetName, std::vector<TilesetTile>{});
    if(!insertSucceeded){
        THROW_ERROR(
            ErrorType::GENERIC_ERROR,
            "insertion of tileset '" + tilesetName + "' into tilesets map was unsuccessful (have you made sure to give each tileset a unique name?)",
            load_tileset_into_context
        );
    }
    auto& tilesVector = iter->second;
    tilesVector.reserve(numTiles);
    size_t tileIdx = 0;
    for(const auto& tileObj : tilesetTiles){
        CHECK_ERROR_STR(
            load_tile_into_vector(context, tileObj, tilesVector);,
            "load_tileset_into_context (tile index: " + std::to_string(tileIdx) + ')'
        )
        tileIdx++;
    }
}

static void load_level_tilesets(Context& context, const Json& levelDict){
    if(!levelDict.contains("tilesets")){
        return; // Not an error, just don't load any tilesets.
    }
    const Json& tilesetsDict = levelDict.at("tilesets");
    if(!tilesetsDict.is_object()){
        THROW_ERROR(
            ErrorType::INVALID_JSON_TYPE,
            "expected dictionary (object) for level field `tilesets`, received '" + to_string(tilesetsDict) + '\'' ,
            load_level_tilesets
        );
    }
    for(const auto& [tilesetName, tilesetTiles] : tilesetsDict.items()){
        CHECK_ERROR_STR(
            load_tileset_into_context(context, tilesetName, tilesetTiles),
            "load_level_tilesets (current tileset name: " + tilesetName + ')'
        );
    }
}

static void iterate_level_keys(Context& context, LevelRegistry& registry, const Json& levelDict){
    CHECK_ERROR(
        init_level_data(context, registry, levelDict);,
        iterate_level_keys
    );
    CHECK_ERROR(
        load_level_tilesets(context, levelDict);,
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

} // namespace LevelBuilder
