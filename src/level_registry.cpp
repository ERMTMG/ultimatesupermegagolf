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
    this->entityNames = move(other.entityNames);
    this->numberOfLevelObjects = other.numberOfLevelObjects;
}

LevelRegistry& LevelRegistry::operator=(LevelRegistry&& rhs){
    this->entityNames = move(rhs.entityNames);
    this->registry = move(rhs.registry);
    this->numberOfLevelObjects = rhs.numberOfLevelObjects;
    return *this;
}


entt::entity LevelRegistry::new_entity(const std::string& name){
    entt::entity newEntity = registry->create();
    registry->emplace<EntityName>(newEntity, name);
    entityNames[name] = newEntity;
    return newEntity;
}

entt::entity LevelRegistry::new_entity(std::string&& name){
    entt::entity newEntity = registry->create();
    registry->emplace<EntityName>(newEntity, std::move(name));
    entityNames[name] = newEntity;
    return newEntity;
}

entt::entity LevelRegistry::new_level_object(const std::string& namePrefix, const Position& pos, bool uniqueName){
    entt::entity newEntity;
    if(uniqueName){
        newEntity = new_entity(namePrefix);
    } else {
        newEntity = new_entity(namePrefix + std::to_string(numberOfLevelObjects));
    }
    numberOfLevelObjects++;
    registry->emplace<Position>(newEntity, pos);
    return newEntity;
}

entt::entity LevelRegistry::get_entity(const std::string& name) const{
    auto itr = entityNames.find(name);
    if(itr != entityNames.end()){
        return itr->second;
    } else {
        return entt::null;
    }
}

entt::entity LevelRegistry::get_entity(std::string&& name) const{
    auto itr = entityNames.find(std::move(name));
    if(itr != entityNames.end()){
        return itr->second;
    } else {
        return entt::null;
    }
}

std::vector<entt::entity> LevelRegistry::search_entities_by_name(const std::string& prefix) const{
    std::vector<entt::entity> output; 
    output.reserve(registry->view<entt::entity>().size() / 10); // let's say about a tenth of all entities. might change this later
    for(const auto&[name, entity] : entityNames){
        auto nameItr = name.begin(); auto pfxItr = prefix.begin();
        bool matching = true;
        const auto nameEnd = name.end(); const auto pfxEnd = prefix.end();
        while(pfxItr != pfxEnd && nameItr != nameEnd && matching){
            if(*pfxItr != *nameItr){
                matching = false;
            }
            ++nameItr; ++pfxItr;
        }
        if(matching){
            output.push_back(entity);
        }
    }
    return output;
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

    entt::entity player = new_level_object(PLAYER_ENTITY_NAME, pos, true);
    registry->emplace<Velocity>(player, 0, 0);
    registry->emplace<SpriteSheet>(player, BALL_SPRITE_FILENAME, 16, 16);
    registry->emplace<SpriteTransform>(player, VEC2_ZERO, 1, 0);
    CollisionComponent& collision = registry->emplace<CollisionComponent>(player, new CollisionCircle(VEC2_ZERO, PLAYER_RADIUS), 0, false);
    add_to_layer(collision, PLAYER_COLLISION_LAYER);
    registry->emplace<CollisionEntityStoreComponent>(player);
    registry->emplace<PlayerComponent>(player, VEC2_ZERO, VEC2_ZERO, 0, PlayerComponent::MAX_HEALTH, true);
    BoundingBoxComponent playerBB = calculate_bb(collision, 0);
    registry->emplace<BoundingBoxComponent>(player, playerBB);
    registry->emplace<CollisionHandler>(player, default_collision_handler());

    return player;
}

entt::entity LevelRegistry::create_goal(const Position& pos){
    static const char* const GOAL_SPRITE_FILENAME = "resources/sprites/flag.png";

    entt::entity goal = new_level_object(GOAL_ENTITY_NAME, pos, true);
    SpriteSheet& sprite = registry->emplace<SpriteSheet>(goal, GOAL_SPRITE_FILENAME, 16, 32);
    sprite.set_animation_length(0, sprite.numberFramesPerRow);
    registry->emplace<AnimationHandler>(goal, default_animation_handler<>());
    CollisionComponent& collision = registry->emplace<CollisionComponent>(goal, new CollisionRect({-8,21-16}, 16, 11), 0, true);
    add_to_layer(collision, PLAYER_COLLISION_LAYER);
    BoundingBoxComponent goalBB = calculate_bb(collision, 0);
    registry->emplace<BoundingBoxComponent>(goal, goalBB);

    return goal;
}

entt::entity LevelRegistry::create_camera_centered_at(const Position& pos){
    entt::entity camera = new_entity(CAMERA_ENTITY_NAME);
    registry->emplace<CameraView>(camera, camera_centered_at(pos));
    return camera;
}

void LevelRegistry::init_level(const Position& playerPos, const Position& goalPos, const Position& cameraPos){
    create_player(playerPos);
    create_goal(goalPos);
    create_camera_centered_at(cameraPos);
    // TODO later: level info component 
    entt::entity input = new_entity(INPUT_MANAGER_ENTITY_NAME);
    registry->emplace<InputManager>(input); 

    entt::entity rng = new_entity(RNG_ENTITY_NAME);
    registry->emplace<RNGComponent>(rng, new_rng_component());
}

std::pair<entt::entity, CollisionComponent&> LevelRegistry::create_static_body(const Position& pos, std::vector<LayerType>&& layers){
    using std::forward;
    using std::vector;

    entt::entity body = new_level_object("staticBody", pos);
    CollisionComponent& collision = registry->emplace<CollisionComponent>(body);
    set_layers(collision, forward<vector<LayerType>>(layers));
    registry->emplace<BoundingBoxComponent>(body);

    return std::pair(body, std::ref(collision));
}

void LevelRegistry::recalculate_bounding_box(entt::entity entity){
    BoundingBoxComponent bb;
    CollisionComponent* collision = registry->try_get<CollisionComponent>(entity);
    if(collision != nullptr){
        bb = calculate_bb(*collision);
    } else {
        SpriteSheet* sprite = registry->try_get<SpriteSheet>(entity);
        if(sprite != nullptr){
            bb = calculate_bb(*sprite);
        }
    }
    registry->emplace_or_replace<BoundingBoxComponent>(entity, bb);
}

void LevelRegistry::handle_collisions_general(){
    // TODO: divide this into substeps to avoid things clipping through each other. maybe has to be in the update function itself
    
    // set all collidedEntityIDs to null
    auto collisionStoreEntities = registry->view<CollisionEntityStoreComponent>();
    for(auto[entity, store] : collisionStoreEntities.each()){
        store.collidedEntityID = entt::null;
    }

    auto collisionEntities = registry->view<CollisionComponent, Position, const BoundingBoxComponent>();
    for(auto[entity_i, collision_i, position_i, bb_i] : collisionEntities.each()){
        for(auto[entity_j, collision_j, position_j, bb_j] : collisionEntities.each()){
            if(entity_i < entity_j // avoid repeated collisions
             && (!collision_i.isStatic || !collision_j.isStatic) // no need to check if both bodies are static
             && overlapping_bb(bb_i, bb_j, position_i, position_j)){ // only check if their bounding boxes are colliding

                CollisionInformation info = get_collision(collision_i, collision_j, position_i, position_j);
                if(info.collision){ // congrats, they're colliding
                    Velocity* velocity_i = registry->try_get<Velocity>(entity_i);
                    Velocity* velocity_j = registry->try_get<Velocity>(entity_j);
                    // fix collision (move objects out of the way)
                    if(velocity_i != nullptr && velocity_j != nullptr){ // neither object is static
                        mutually_move_objects_out_of_collision(collision_i, collision_j, position_i, position_j, info);
                    } else if(velocity_i != nullptr){ // entity_i isn't static
                        move_object_out_of_collision(collision_i, collision_j, position_i, position_j, info);
                    } else { // entity_j isn't static
                        move_object_out_of_collision(collision_j, collision_i, position_j, position_i, info);
                    }

                    // call collision handlers
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

                    // store collided entity IDs
                    CollisionEntityStoreComponent* store_i = registry->try_get<CollisionEntityStoreComponent>(entity_i);
                    CollisionEntityStoreComponent* store_j = registry->try_get<CollisionEntityStoreComponent>(entity_j);
                    if(store_i != nullptr){
                        store_i->collidedEntityID = entity_j;
                    }
                    if(store_j != nullptr){
                        store_j->collidedEntityID = entity_i;
                    }
                    std::cout << "collision detected! entities: " << (unsigned int)entity_i << ", " << (unsigned int)entity_j << "\n";
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

void LevelRegistry::handle_camera(float delta){
    // only moves camera to player, for now
    static const float LERP_WEIGHT = 0.25;

    CameraView& camera = registry->get<CameraView>(get_entity(CAMERA_ENTITY_NAME));
    const Position& playerPos = registry->get<Position>(get_entity(PLAYER_ENTITY_NAME));
    Vector2 newCameraPos = camera->target + (to_Vector2(playerPos) - camera->target) * LERP_WEIGHT;
    set_camera_center(camera, Position{newCameraPos});
}

void LevelRegistry::handle_input_and_player(){
    InputManager& input = registry->get<InputManager>(get_entity(INPUT_MANAGER_ENTITY_NAME));
    entt::entity playerID = get_entity(PLAYER_ENTITY_NAME);
    PlayerComponent& player = registry->get<PlayerComponent>(playerID);
    Velocity& vel = registry->get<Velocity>(playerID);
    const CameraView& camera = registry->get<CameraView>(get_entity(CAMERA_ENTITY_NAME));

    update_input(input);
    update_player(player, vel, input, camera);
    if(is_input_pressed_this_frame(input, InputManager::RESET)){
        // TODO: implement level resetting
    } else if(is_input_pressed_this_frame(input, InputManager::PAUSE)){
        // TODO: also implement pausing. god i have zero idea how to do this
    }

    const auto& store = registry->get<CollisionEntityStoreComponent>(playerID);
    if(store.collidedEntityID == entityNames.at(GOAL_ENTITY_NAME)){
        //TODO: player won, level ends. don't know where to go, just panic segfault
        int x = *(int*)nullptr;
        std::cout << x;
    }
}

#include<iostream>

void LevelRegistry::update(float delta){
    //move objects with velocity
    //std::cout << "frame update!\n";
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
    handle_input_and_player();
    handle_animations(delta);
    //handle_camera(delta);
    
}

void LevelRegistry::draw(bool debugMode) const{
    static const Color BACKGROUND_COLOR = DARKGRAY;

    BeginDrawing();
        const CameraView& camera = registry->get<CameraView>(get_entity(CAMERA_ENTITY_NAME));
        BeginMode2D(camera.cam);
            ClearBackground(BACKGROUND_COLOR);

            /*auto spriteEntities = registry->view<const SpriteSheet, const Position>();
            for(auto[entity, sprite, pos] : spriteEntities.each()){
                
                const SpriteTransform* transform = registry->try_get<SpriteTransform>(entity);
                if(transform != nullptr){
                    draw_sprite(sprite, *transform, pos);
                } else {
                    draw_sprite(sprite, pos);
                }
            }*/
            auto onlySprites = registry->view<const SpriteSheet, const Position>(entt::exclude<SpriteTransform>); 
            for(auto[entity, sprite, pos] : onlySprites.each()){
                const BoundingBoxComponent* bb = registry->try_get<BoundingBoxComponent>(entity);
                if(bb != nullptr && is_in_view(camera, *bb, pos)){
                    draw_sprite(sprite, pos);
                }
            }
            auto transfomedSprites = registry->view<const SpriteSheet, const Position, const SpriteTransform>(); // separated into two distinct views for performance reasons
            for(auto[entity, sprite, pos, transform] : transfomedSprites.each()){
                const BoundingBoxComponent* bb = registry->try_get<BoundingBoxComponent>(entity);
                if(bb != nullptr && is_in_view(camera, *bb, pos)){
                    draw_sprite(sprite, transform, pos);
                }
            }

            auto tilemaps = registry->view<const TilesetComponent, const Position>();
            for(auto[entity, tilemap, pos] : tilemaps.each()){
                const BoundingBoxComponent* bb = registry->try_get<BoundingBoxComponent>(entity);
                if(bb != nullptr && is_in_view(camera, *bb, pos)){
                    draw_tileset(tilemap, pos);
                }
            }

            if(debugMode){
                auto collisionEntites = registry->view<const CollisionComponent, const Position>();
                for(auto[entity, collision, pos] : collisionEntites.each()){
                    draw_collision_debug(collision, pos);
                }

                auto boundingBoxes = registry->view<const BoundingBoxComponent, const Position>();
                for(auto[entity, bb, pos] : boundingBoxes.each()){
                    draw_bb_debug(bb, pos);
                }
            }
            entt::entity playerEntity = get_entity(PLAYER_ENTITY_NAME);
            const PlayerComponent& player = registry->get<PlayerComponent>(playerEntity);
            const Position& pos = registry->get<Position>(playerEntity);
            draw_player_drag_velocity(player, pos);
        EndMode2D();
        DrawFPS(10,10);
        // TODO later: implement and draw UI 
    EndDrawing();
}