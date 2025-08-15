#include<raylib.h>
#include"entt.hpp"
#include"simdjson.h"
#include"level_registry.h"

namespace LevelBuilder
{

struct Context {
    simdjson::ondemand::parser jsonParser;
    simdjson::ondemand::document topLevelDoc;
    simdjson::error_code error;
};

Context init_parser(const char* jsonFilename);

void build_level(Context& context, LevelRegistry& registry);

}