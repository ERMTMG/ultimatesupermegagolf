/*
    FILE: level_registry.h
    Defines the LevelRegistry class which symbolizes a whole level / area in the game,
    and allows control over its entities.
*/
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

/*
    Main class that represents a level with all its entities, and allows basic 
    manipulation of them. Wraps the entt::registry class, giving it functionality
    to function within the game (updating, drawing,...). Has functionality to get
    entities by custom-given names.
*/
class LevelRegistry{
  // Static constants that declare the names of all important entities for easy searching
  public:
    static inline const std::string PLAYER_ENTITY_NAME = "__PLAYER";
    static inline const std::string GOAL_ENTITY_NAME = "__GOAL";
    static inline const std::string CAMERA_ENTITY_NAME = "__CAMERA";
    static inline const std::string INPUT_MANAGER_ENTITY_NAME = "__INPUT";
    static inline const std::string RNG_ENTITY_NAME = "__RNG";

  private:
    // dynamically allocated registry because the size of entt::registry is ridiculous.
    // Maybe actually just having it as a member would be better? most of the registry's 
    // info is on the heap anyway
    std::unique_ptr<entt::registry> registry;
    // Stores the total number of level objects in the registry. What counts as a level
    // object is a little arbitrary, but it mostly just depends on it not being part of the UI.
    unsigned int numberOfLevelObjects;
    // Stores all the entity names in a hash map for easy lookup.
    std::unordered_map<std::string, entt::entity> entityNames;
    // Does the collision logic (detecting and resolving collisions, calling handlers,...)
    void handle_collisions_general();
    // Calls the respective animation handlers to update the sprites of all objects
    void handle_animations(float delta);
    // Camera movement, etc.
    void handle_camera(float delta);
    // Handles player input (dragging, pausing,...) and player-specific actions.
    void handle_input_and_player();
    // Creates adn sets up the player entity and returns its ID.
    entt::entity create_player(const Position& pos);
    // Creates and sets up the goal entity and returns its ID.
    entt::entity create_goal(const Position& pos);
    // Creates and sets up a camera entity with a CameraView centered at the given position
    // and returns its ID.
    entt::entity create_camera_centered_at(const Position& pos);
  public:
    // Collision layer in which the player resides.
    static const LayerType PLAYER_COLLISION_LAYER = 1;

    LevelRegistry();
    ~LevelRegistry();
    LevelRegistry(LevelRegistry&& other);
    LevelRegistry& operator=(LevelRegistry&& rhs);
    ;// no copy constructor/assignment because copying levels probably isn't useful
    LevelRegistry(const LevelRegistry& other) = delete; 
    LevelRegistry& operator=(const LevelRegistry& other) = delete;
    // initializes the level (i.e, creates all important entities) with the relevant positions
    void init_level(const Position& playerPos, const Position& goalPos, const Position& cameraPos);
    inline void init_level(const Position& playerPos, const Position& goalPos){
        init_level(playerPos, goalPos, playerPos);
    }
    // creates a new entity with the given name and returns its ID
    entt::entity new_entity(const std::string& name);
    entt::entity new_entity(std::string&& name);
    /* 
        creates a new level object with the given name prefix at the given position. the prefix
        will be followed by a number correspondent to the total number of level objects, unless
        uniqueName is set to true in which case the prefix will be the entity's full name if possible.
    */ 
    entt::entity new_level_object(const std::string& namePrefix, const Position& pos = {0,0}, bool uniqueName = false);
    // Gets the entity ID with the given name, if no entity exists with that name, returns entt::null.
    entt::entity get_entity(const std::string& name) const;
    entt::entity get_entity(std::string&& name) const;
    // Gets a vector containing all the entity ID's whose names start with prefix.
    // might not be a good idea to run this while ingame as it can take up a long time.
    std::vector<entt::entity> search_entities_by_name(const std::string& prefix) const;
    // Returns a reference to the wrapped registry.
    entt::registry& get();
    // Returns a *const* reference to the wrapped registry.
    const entt::registry& get() const;
    /*
        Creates a static collision entity at the given position and with the given collision
        layers. Returns a pair containing the corresponding entity ID and a reference to the entity's
        created collision component. Intended syntax:
          auto[block, blockCollision] = registry.create_static_body(VEC2_ZERO, {1,2,5,8});
          blockCollision.add_rect(VEC2_ZERO, 16, 16);
    */
    std::pair<entt::entity, CollisionComponent&> create_static_body(const Position& pos, std::vector<LayerType>&& layers);
    // Recalculates the bounding box component of the given entity, in case its collision and/or sprite 
    // has been modified. If the entity doesn't have a bounding box yet, it adds it.
    void recalculate_bounding_box(entt::entity entity);
    // Basic game logic function. Of course, runs 60 times a second.
    void update(float delta);
    // Draws the level to the screen. Includes a Raylib BeginDrawing() and EndDrawing() call, so there's no
    // need to nest the function inside another BeginDrawing() ... EndDrawing(). Runs 60 times a second too.
    void draw(bool debugMode = false) const;
};

