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
    build_level_info(context, registry, levelDict);
    if(context.error){
        std::cerr << "At build_level\n";
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

static void build_level_info(Context& context, LevelRegistry& registry, simdjson::ondemand::object& levelDict){
    using namespace simdjson;
    Vector2 playerPos = VEC2_ZERO;
    Vector2 goalPos = VEC2_ZERO;
    Vector2 cameraPos = VEC2_ZERO;
    bool cameraPosIsNonDefault = false;
    std::string levelName;
    for(auto field: levelDict)
    {
        std::string key;
        error_code error = field.escaped_key().get(key);
        if(error){
            std::cerr << "Error on build_level_info at field recognition\n";
            context.error = error;
            return;
        }
        if(key == "level_name"){
            error = field.value().get_string().get(levelName);
            if(error){
                std::cerr << "Error on build_level_info: value on field \"level_name\" is not string\n";
                context.error = error;
                return;
            }
        } else if(key == "player_position"){ // TODO: offload this into a function that just gets a Vector2 with a generic error message
            ondemand::object vec2;
            error = field.value().get_object().get(vec2);
            if(error){
                std::cerr << "Error on build_level_info: value on field \"player_position\" is not object\n";
                context.error = error;
                return;
            }
            Vector2 pos = parse_Vector2(context, vec2);
            if(context.error){
                std::cerr << "At build_level_info\n";
                return;
            } else {
                playerPos = pos;
            }
        } else if(key == "goal_position"){
            ondemand::object vec2;
            error = field.value().get_object().get(vec2);
            if(error){
                std::cerr << "Error on build_level_info: value on field \"goal_position\" is not object\n";
                context.error = error;
                return;
            }
            Vector2 pos = parse_Vector2(context, vec2);
            if(context.error){
                std::cerr << "At build_level_info\n";
                return;
            } else {
                goalPos = pos;
            }
        } else if(key == "camera_position"){
            ondemand::object vec2;
            error = field.value().get_object().get(vec2);
            if(error){
                std::cerr << "Error on build_level_info: value on field \"camera_position\" is not object\n";
                context.error = error;
                return;
            }
            Vector2 pos = parse_Vector2(context, vec2);
            if(context.error){
                std::cerr << "At build_level_info\n";
                return;
            } else {
                cameraPos = pos;
                cameraPosIsNonDefault = true;
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
