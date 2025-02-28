#pragma once
#include"entt.hpp"
#include"raylib.h"
#include"basic_components.h"
#include"collision_component.h"
#include"player_component.h"
#include"bounding_box.h"
#include"collision_handler.h"
#include"animation_handler.h"
#include"rng_component.h"
#include<memory>

class LevelRegistry{
  public:
    enum COMMON_ENTITY_TYPES {
        ENTITY_TYPE_PLAYER = 0,
        ENTITY_TYPE_GOAL,
        ENTITY_TYPE_CAMERA,
        ENTITY_TYPE_LEVELINFO,
        ENTITY_TYPE_INPUT_HANDLER,
        ENTITY_TYPE_RNG,
        ENTITY_TYPE_ENUM_SIZE,
    };
  private:
    std::unique_ptr<entt::registry> registry;
    unsigned int numberOfLevelObjects;
    entt::entity commonEntities[ENTITY_TYPE_ENUM_SIZE];
    void handle_collisions_general();
    void handle_animations(float delta);
    void handle_camera(float delta);
    void handle_input_and_player();
    entt::entity create_player(const Position& pos);
    entt::entity create_goal(const Position& pos);
    entt::entity create_camera_centered_at(const Position& pos);
  public:
    static const LayerType PLAYER_COLLISION_LAYER = 1;

    LevelRegistry();
    ~LevelRegistry();
    LevelRegistry(LevelRegistry&& other);
    LevelRegistry& operator=(LevelRegistry&& rhs);
    LevelRegistry(const LevelRegistry& other) = delete; // don't think we're gonna be copying levels anytime soon
    LevelRegistry& operator=(const LevelRegistry& other) = delete;
    void init_level(const Position& playerPos, const Position& goalPos, const Position& cameraPos);
    inline void init_level(const Position& playerPos, const Position& goalPos){
        init_level(playerPos, goalPos, playerPos);
    }
    entt::entity get_entity(COMMON_ENTITY_TYPES type) const;
    entt::registry& get();
    const entt::registry& get() const;
    CollisionComponent& create_static_body(const Position& pos, std::vector<LayerType>&& layers);
    void update(float delta);
    void draw(bool debugMode = false) const;
};

