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
#include<unordered_map>

class LevelRegistry{
  public:
    /*enum COMMON_ENTITY_TYPES {
        ENTITY_TYPE_PLAYER = 0,
        ENTITY_TYPE_GOAL,
        ENTITY_TYPE_CAMERA,
        ENTITY_TYPE_LEVELINFO,
        ENTITY_TYPE_INPUT_HANDLER,
        ENTITY_TYPE_RNG,
        ENTITY_TYPE_ENUM_SIZE,
    };*/
    static inline const std::string PLAYER_ENTITY_NAME = "__PLAYER";
    static inline const std::string GOAL_ENTITY_NAME = "__GOAL";
    static inline const std::string CAMERA_ENTITY_NAME = "__CAMERA";
    static inline const std::string INPUT_MANAGER_ENTITY_NAME = "__INPUT";
    static inline const std::string RNG_ENTITY_NAME = "__RNG";

  private:
    std::unique_ptr<entt::registry> registry;
    unsigned int numberOfLevelObjects;
    std::unordered_map<std::string, entt::entity> entityNames;
    //entt::entity commonEntities[ENTITY_TYPE_ENUM_SIZE];
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
    entt::entity new_entity(const std::string& name);
    entt::entity new_entity(std::string&& name);
    entt::entity new_level_object(const std::string& namePrefix, const Position& pos = {0,0}, bool uniqueName = false);
    entt::entity get_entity(const std::string& name) const;
    entt::entity get_entity(std::string&& name) const;
    std::vector<entt::entity> search_entities_by_name(const std::string& prefix) const;
    //entt::entity get_entity(COMMON_ENTITY_TYPES type) const;
    entt::registry& get();
    const entt::registry& get() const;
    std::pair<entt::entity, CollisionComponent&> create_static_body(const Position& pos, std::vector<LayerType>&& layers);
    void recalculate_bounding_box(entt::entity entity);
    void update(float delta);
    void draw(bool debugMode = false) const;
};

