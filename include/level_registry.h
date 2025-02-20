#pragma once
#include"entt.hpp"
#include"raylib.h"
#include"basic_components.h"
#include"collision_component.h"
#include"player_component.h"
#include"bounding_box.h"
#include"collision_handler.h"
#include"animation_handler.h"
#include<memory>

class LevelRegistry{
    std::unique_ptr<entt::registry> registry;
    entt::entity playerEntity;
    unsigned int numberOfLevelObjects;

    void handle_collisions_general();
    void handle_animations(float delta);
  public:
    static const LayerType PLAYER_COLLISION_LAYER = 1;

    LevelRegistry();
    ~LevelRegistry();
    LevelRegistry(LevelRegistry&& other);
    LevelRegistry& operator=(LevelRegistry&& rhs);
    LevelRegistry(const LevelRegistry& other) = delete; // don't think we're gonna be copying levels anytime soon
    LevelRegistry& operator=(const LevelRegistry& other) = delete;
    
    entt::registry& get();
    const entt::registry& get() const;
    entt::entity create_player(const Position& pos);
    CollisionComponent& create_static_body(const Position& pos, std::vector<LayerType>&& layers);
    void update(float delta);
    void draw(bool debugMode = false) const;
};

