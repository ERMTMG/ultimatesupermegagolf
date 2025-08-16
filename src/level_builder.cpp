#include "level_builder.h"

namespace LevelBuilder
{

Context init_parser(const char *jsonFilename)
{
    using namespace simdjson;
    padded_string json = padded_string::load(jsonFilename);
    ondemand::parser parser;
    ondemand::document doc = parser.iterate(json);
    return Context {
        .jsonParser = std::move(parser),
        .topLevelDoc = std::move(doc),
        .error = simdjson::error_code::SUCCESS
    };
}    

void build_level(Context& context, LevelRegistry& registry){
    using namespace simdjson;
    ondemand::object levelDict;
    auto error = context.topLevelDoc.get_object().get(levelDict);
    if(error){
        context.error = error;
        return;
    }
    iterate_level_keys(context, registry, levelDict);
    if(context.error){
        std::cerr << "At build_level\n";
        return;
    }
    // TODO: finish literally everything else: tileset loading, entity spawning, etc.
}

static Vector2 parse_Vector2(Context& context, simdjson::ondemand::object& vectorObj){
    using namespace simdjson;
    Vector2 output;
    error_code err = vectorObj["x"].get(output.x);
    err = (err != 0) ? err : vectorObj["y"].get(output.y);
    if(err){
        context.error = err;
        std::cerr << "Error parsing Vector2 object\n";
        return VEC2_NULL;
    } else {
        return output;
    }
}

static bool level_info_get_Vector2(Context& context, simdjson::simdjson_result<simdjson::ondemand::field>& field, Vector2& outVector2, const char* errorName){
    using namespace simdjson;
    ondemand::object vec2;
    error_code error = field.value().get_object().get(vec2);
    if(error){
        std::cerr << "Error on level_info_get_Vector2: value on field \"" << errorName << "\" is not object\n";
        context.error = error;
        return false;
    }
    Vector2 pos = parse_Vector2(context, vec2);
    if(context.error){
        std::cerr << "At build_level_info\n";
        return false;
    } else {
        outVector2 = pos;
        return true;
    }
}

static void build_level_entities(Context& context, LevelRegistry& registry, simdjson::ondemand::object& entitiesDict){
    using namespace simdjson;
    for(auto field : entitiesDict){
        std::string key;
        ondemand::object entityData;
        error_code error = field.escaped_key().get(key);
        if(error){
            std::cerr << "Error on build_level_entities: unable to get field key\n";
            context.error = error;
        }
        error = field.value().get_object().get(entityData);
        if(error){
            std::cerr << "Error on build_level_entities: field \"" << key << "\"'s value is not an object\n";
        }
    }
}

static void iterate_level_keys(Context& context, LevelRegistry& registry, simdjson::ondemand::object& levelDict){
    using namespace simdjson;
    Vector2 playerPos = VEC2_ZERO;
    Vector2 goalPos = VEC2_ZERO;
    Vector2 cameraPos = VEC2_ZERO;
    bool cameraPosIsNonDefault = false;
    std::string levelName;
    for(auto field: levelDict){
        std::string key;
        error_code error = field.escaped_key().get(key);
        if(error){
            std::cerr << "Error on iterate_level_keys at field recognition\n";
            context.error = error;
            return;
        }
        if(key == "level_name"){
            error = field.value().get_string().get(levelName);
            if(error){
                std::cerr << "Error on iterate_level_keys: value on field \"level_name\" is not string\n";
                context.error = error;
                return;
            }
        } else if(key == "player_position"){
            level_info_get_Vector2(context, field, playerPos, "player_position");
            if(context.error){
                std::cerr << "At iterate_level_keys\n";
                return;
            } 
        } else if(key == "goal_position"){
            level_info_get_Vector2(context, field, goalPos, "goal_position");
            if(context.error){
                std::cerr << "At iterate_level_keys\n";
                return;
            } 
        } else if(key == "camera_position"){
            bool success = level_info_get_Vector2(context, field, cameraPos, "camera_position");
            if(success){
                cameraPosIsNonDefault = true;
            } else if(context.error){
                std::cerr << "At iterate_level_keys\n";
            }
        } else if(key == "tilesets"){
            // TODO: parse tilesets
        } else if(key == "entities"){
            ondemand::object entitiesDict;
            error = field.value().get_object().get(entitiesDict);
            if(error){
                std::cerr << "Error on build_level_info: field \"entities\" is not an object\n";
                context.error = error;
                return;
            }
            build_level_entities(context, registry, entitiesDict);
            if(context.error){
                std::cerr << "At iterate_level_keys";
            }
        }
    }
    // TODO: set level registry name or smth
    if(cameraPosIsNonDefault){
        registry.init_level(Position{playerPos}, Position{goalPos}, Position{cameraPos});
    } else {
        registry.init_level(Position{playerPos}, Position{goalPos});
    }
}




}
