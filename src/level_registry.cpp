#include"level_registry.h"

using std::make_unique;
using std::move;

LevelRegistry::LevelRegistry(){
    registry = make_unique<entt::registry>();
}

LevelRegistry::~LevelRegistry(){
    registry->clear();
    registry.reset();
}

LevelRegistry::LevelRegistry(LevelRegistry&& other){
    this->registry = move(other.registry);
    for(int i = 0; i < ENTITY_TYPE_ENUM_SIZE; i++){
        this->commonEntities[i] = other.commonEntities[i];
    }
    this->numberOfLevelObjects = other.numberOfLevelObjects;
}

LevelRegistry& LevelRegistry::operator=(LevelRegistry&& rhs){
    for(int i = 0; i < ENTITY_TYPE_ENUM_SIZE; i++){
        this->commonEntities[i] = rhs.commonEntities[i];
    }
    this->registry = move(rhs.registry);
    this->numberOfLevelObjects = rhs.numberOfLevelObjects;
}

entt::entity LevelRegistry::get_entity(COMMON_ENTITY_TYPES type) const{
    assert(type < ENTITY_TYPE_ENUM_SIZE);
    return commonEntities[type];
}

entt::registry &LevelRegistry::get()
{
    return *registry;
}

const entt::registry& LevelRegistry::get() const{
    return *registry;
}

entt::entity LevelRegistry::create_player(const Position& pos){
    static const char* const BALL_SPRITE_FILENAME = "resources/sprites/ball.png";
    static const float PLAYER_RADIUS = 8;

    entt::entity player = registry->create();
    commonEntities[ENTITY_TYPE_PLAYER] = player;
    numberOfLevelObjects++;
    registry->emplace<Position>(player, pos);
    registry->emplace<Velocity>(player, 0, 0);
    registry->emplace<SpriteSheet>(player, BALL_SPRITE_FILENAME, 16, 16);
    CollisionComponent& collision = registry->emplace<CollisionComponent>(player, new CollisionCircle(VEC2_ZERO, PLAYER_RADIUS), 0, false);
    add_to_layer(collision, PLAYER_COLLISION_LAYER);
    registry->emplace<NormalStoreComponent>(player);
    registry->emplace<PlayerComponent>(player, VEC2_ZERO, VEC2_ZERO, 0, PlayerComponent::MAX_HEALTH, true);
    BoundingBoxComponent playerBB = calculate_bb(collision, 0);
    registry->emplace<BoundingBoxComponent>(player, playerBB);
    registry->emplace<CollisionHandler>(player, default_collision_handler());
    return player;
}

entt::entity LevelRegistry::create_goal(const Position& pos){
    static const char* const GOAL_SPRITE_FILENAME = "resources/sprites/flag.png";

    entt::entity goal = registry->create();
    commonEntities[ENTITY_TYPE_GOAL] = goal;
    numberOfLevelObjects++;
    registry->emplace<Position>(goal, pos);
    registry->emplace<SpriteSheet>(goal, GOAL_SPRITE_FILENAME, 16, 32);
    registry->emplace<AnimationHandler>(goal, default_animation_handler<>());
    CollisionComponent& collision = registry->emplace<CollisionComponent>(goal, new CollisionRect({0,21}, 16, 11), 0, true);
    add_to_layer(collision, PLAYER_COLLISION_LAYER);
    BoundingBoxComponent goalBB = calculate_bb(collision, 0);
    registry->emplace<BoundingBoxComponent>(goal, goalBB);

    return goal;
}

entt::entity LevelRegistry::create_camera_centered_at(const Position& pos){
    entt::entity camera = registry->create();
    commonEntities[ENTITY_TYPE_CAMERA] = camera;
    registry->emplace<CameraView>(camera, camera_centered_at(pos));
    return camera;
}

void LevelRegistry::init_level(const Position& playerPos, const Position& goalPos, const Position& cameraPos){
    create_player(playerPos);
    create_goal(goalPos);
    create_camera_centered_at(cameraPos);
    // TODO later: level info component 
    entt::entity input = registry->create();
    commonEntities[ENTITY_TYPE_INPUT_HANDLER] = input;
    registry->emplace<InputManager>(input); 
}

CollisionComponent& LevelRegistry::create_static_body(const Position& pos, std::vector<LayerType>&& layers){
    using std::forward;
    using std::vector;

    entt::entity body = registry->create();
    numberOfLevelObjects++;
    registry->emplace<Position>(body, pos);
    CollisionComponent& collision = registry->emplace<CollisionComponent>(body);
    set_layers(collision, forward<vector<LayerType>>(layers));

    return collision;
}

void LevelRegistry::handle_collisions_general(){
    auto collisionEntities = registry->view<CollisionComponent, const Position, const BoundingBoxComponent>();
    for(auto[entity_i, collision_i, position_i, bb_i] : collisionEntities.each()){
        for(auto[entity_j, collision_j, position_j, bb_j] : collisionEntities.each()){
            if(entity_i < entity_j // avoid repeated collisions
                && (!collision_i.isStatic || !collision_j.isStatic) // no need to check if both bodies are static
                && overlapping_bb(bb_i, bb_j, position_i, position_j)){ // only check if their bounding boxes are colliding

                CollisionInformation info = get_collision(collision_i, collision_j, position_i, position_j);
                if(info.collision){
                    Velocity* velocity_i = registry->try_get<Velocity>(entity_i);
                    Velocity* velocity_j = registry->try_get<Velocity>(entity_j);
                    CollisionHandler* handler_i = registry->try_get<CollisionHandler>(entity_i);
                    CollisionHandler* handler_j = registry->try_get<CollisionHandler>(entity_j);
                    if(handler_i != nullptr){
                        if(handler_i->type == COLLISION_HANDLER_DEFAULT){
                            (handler_i->defaultHandler)(info, velocity_i, velocity_j, handler_j);
                        } else {
                            (handler_i->customHandler)(info, entity_i, entity_j, registry.get());
                        }
                    }
                    if(handler_j != nullptr){
                        if(handler_j->type == COLLISION_HANDLER_DEFAULT){
                            (handler_j->defaultHandler)(info, velocity_j, velocity_i, handler_i);
                        } else {
                            (handler_i->customHandler)(info, entity_j, entity_i, registry.get());
                        }
                    }
                }

            }
        }
    }
}

void LevelRegistry::handle_animations(float delta){
    auto spriteEntities = registry->view<SpriteSheet>();
    for(auto[entity, sprite] : spriteEntities.each()){
        AnimationHandler* handler = registry->try_get<AnimationHandler>(entity);
        if(handler != nullptr){
            handler->handler(delta, handler->timer, sprite);
        }
    }
}

void LevelRegistry::update(float delta){
    //move objects with velocity
    auto viewPositionAndVelocity = registry->view<Position, Velocity>();
    for(auto[entity, pos, vel] : viewPositionAndVelocity.each()){
        if(registry->all_of<const Acceleration>(entity)){ //const may not be necessary?
            const Acceleration& accel = registry->get<const Acceleration>(entity);
            move_position(pos, vel, accel, delta);
        } else {
            move_position(pos, vel, delta);
        }
    }

    handle_collisions_general(); // maybe dispatch this to another thread?
    handle_animations(delta);
}

void LevelRegistry::draw(bool debugMode) const{
    BeginDrawing();
        //TODO: add camera shenanigans?
        auto spriteEntities = registry->view<const SpriteSheet, const Position>();
        for(auto[entity, sprite, pos] : spriteEntities.each()){
            draw_sprite(sprite, pos);
        }
        if(debugMode){
            auto collisionEntites = registry->view<const CollisionComponent, const Position>();
            for(auto[entity, collision, pos] : collisionEntites.each()){
                draw_collision_debug(collision, pos);
            }
        }
    EndDrawing();
}