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
    this->playerEntity = other.playerEntity;
    this->numberOfLevelObjects = other.numberOfLevelObjects;
}

LevelRegistry& LevelRegistry::operator=(LevelRegistry&& rhs){
    this->registry = move(rhs.registry);
    this->playerEntity = rhs.playerEntity;
    this->numberOfLevelObjects = rhs.numberOfLevelObjects;
}

entt::registry& LevelRegistry::get(){
    return *registry;
}

const entt::registry& LevelRegistry::get() const{
    return *registry;
}

entt::entity LevelRegistry::create_player(const Position& pos){
    static const char* const BALL_SPRITE_FILENAME = "resources/sprites/ball.png";
    static const float PLAYER_RADIUS = 8;

    entt::entity player = registry->create();
    playerEntity = player;
    numberOfLevelObjects++;
    registry->emplace<Position>(player, pos);
    registry->emplace<Velocity>(player, 0, 0);
    registry->emplace<SpriteSheet>(player, BALL_SPRITE_FILENAME, 16, 16);
    CollisionComponent& collision = registry->emplace<CollisionComponent>(player, new CollisionCircle({0,0}, PLAYER_RADIUS), 0, false);
    add_to_layer(collision, PLAYER_COLLISION_LAYER);
    registry->emplace<NormalStoreComponent>(player);
    registry->emplace<PlayerComponent>(player, Vector2{0,0}, 0, true);

    return player;
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

    handle_collisions_general();

    // TODO: implement collisions, handling animations, blahblahblah
    // i'm tired boss

}