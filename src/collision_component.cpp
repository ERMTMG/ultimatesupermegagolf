#include"collision_component.h"
CollisionComponent::CollisionComponent(unsigned short layer, bool isStatic): layerFlags(layer), isStatic(isStatic){
    shapes.reserve(3);
}

CollisionComponent::CollisionComponent(CollisionShape* shape, unsigned short layer, bool isStatic): layerFlags(layer), isStatic(isStatic){
    shapes.reserve(3);
    shapes.emplace_back(shape); // constructs a unique_ptr from that shape. no need to move it :3
}

void CollisionComponent::add_circle(float radius, const Vector2 &pos)
{
    shapes.emplace_back(new CollisionCircle(pos, radius));
}

void CollisionComponent::add_rect(float width, float height, const Vector2& pos){
    shapes.emplace_back(new CollisionRect(pos, width, height));
}

void CollisionComponent::add_rect_centered(float width, float height){
    shapes.emplace_back(new CollisionRect(Vector2{-width/2, -height/2}, width, height));
}

void CollisionComponent::add_line(const Vector2& pos1, const Vector2& pos2){
    shapes.emplace_back(new CollisionLine(pos1, pos2 - pos1));
}

void CollisionComponent::add_barrier(const Vector2& pos, const Vector2& dir){
    float angleOfVector = atan2(dir.y, dir.x);
    shapes.emplace_back(new CollisionBarrier(pos, angleOfVector));
}

void CollisionComponent::add_point(const Vector2& point){
    shapes.emplace_back(new CollisionPoint(point));
}

void set_layers(CollisionComponent &collision, std::vector<LayerType> &&layers){
    collision.layerFlags = 0;
    for(LayerType layer : layers){
        if(layer >= NUMBER_OF_LAYERS) throw std::invalid_argument("layer must be less than " + std::to_string(NUMBER_OF_LAYERS));
        collision.layerFlags |= (1 << layer);
    }
}

void clone_collision(const CollisionComponent& source, CollisionComponent& destination){
    destination.isStatic = source.isStatic;
    destination.layerFlags = source.layerFlags;
    for(const auto& shapePtr : source.shapes){
        destination.shapes.push_back(shapePtr->clone());
    }
}

// Adds all shapes of collision2 to collision1 with an added offset given by pos. Doesn't modify the layers of collision1, just adds the shapes.
void add_collision(CollisionComponent& collision1, const CollisionComponent& collision2, const Position& pos){
    for(const auto& shapePtr : collision2.shapes){
        std::unique_ptr<CollisionShape> newShape = shapePtr->clone();
        newShape->offset += to_Vector2(pos);
        collision1.shapes.push_back(std::move(newShape));
    } 
}

CollisionInformation get_collision(const CollisionComponent& collision1, const CollisionComponent& collision2, const Position& pos1, const Position& pos2){
    CollisionInformation output {false, VEC2_ZERO};
    if(!has_common_layers(collision1, collision2)) {
        return output;
    }
    int shapeCount = 0; // for cumulative calculation of average normal vector
    for(const auto& shapePtr1 : collision1.shapes){
        CollisionShape* currentShape = shapePtr1.get();
        Vector2 currentShapeAverageNormal = VEC2_ZERO; // same but on the individual shape
        int currentShapeAveCount = 0;
        for(const auto& shapePtr2 : collision2.shapes){
            CollisionShape* checkingShape = shapePtr2.get();
            auto[colliding, currentNormal] = process_collision(currentShape, checkingShape, pos1, pos2);
            if(colliding){
                output.collision = true;
                currentShapeAveCount++;
                currentShapeAverageNormal = currentShapeAverageNormal * (currentShapeAveCount - 1) / currentShapeAveCount + (currentNormal/currentShapeAveCount);
            }
        }
        if(currentShapeAverageNormal != VEC2_ZERO){
            shapeCount++;
            output.unitNormal = output.unitNormal * (shapeCount - 1) / shapeCount + (currentShapeAverageNormal / shapeCount);
        }
    }
    output.unitNormal = unit_vector(output.unitNormal);
    return output;    
}

void mutually_move_objects_out_of_collision(const CollisionComponent& collision1, const CollisionComponent& collision2, Position& pos1, Position& pos2, const CollisionInformation& info){
    static const float STEP_MULTIPLIER = 1.02;
    CollisionInformation current{};
    Vector2 normalVector = info.unitNormal;
    do {
        Vector2 newPos1 = to_Vector2(pos1) + normalVector;
        Vector2 newPos2 = to_Vector2(pos2) - normalVector;
        pos1 = Position{newPos1}; pos2 = Position{newPos2};
        normalVector *= STEP_MULTIPLIER;
        current = get_collision(collision1, collision2, pos1, pos2);
    } while (current.collision);
}

void move_object_out_of_collision(const CollisionComponent& movingCollision, const CollisionComponent& staticCollision, Position& movingPosition, const Position& staticPosition, const CollisionInformation& info){
    static const float STEP_MULTIPLIER = 1.05; // multiplies normal vector each check so that if objects are colliding too deep then it takes less steps
    CollisionInformation current{};
    Vector2 normalVector = info.unitNormal;
    do {
        Vector2 newPos = to_Vector2(movingPosition) + normalVector;
        movingPosition = Position(newPos);
        normalVector *= STEP_MULTIPLIER;
        current = get_collision(movingCollision, staticCollision, movingPosition, staticPosition);
    } while(current.collision);
}

void draw_collision_debug(const CollisionComponent &collision, const Position &pos)
{
    static const Color DEBUG_COLLISION_COLOR = Color{0x40, 0xFF, 0x00, 0xFF};
    static const float POINT_THICKNESS = 1.5;
    static const float BARRIER_THICKNESS = 3;

    Vector2 position = {pos.x, pos.y};
    for(const auto& shapePtr : collision.shapes){
        CollisionShapeType type = shapePtr->get_type();
        switch (type){
          case CollisionShapeType::POINT:{
            Vector2 drawPos = position + shapePtr->offset;
            DrawCircle(drawPos.x, drawPos.y, POINT_THICKNESS, DEBUG_COLLISION_COLOR);
            break;
          }
          case CollisionShapeType::LINE:{
            const CollisionLine& line = *static_cast<const CollisionLine*>(shapePtr.get());
            Vector2 start = position + line.offset;
            Vector2 end = position + line.offset + line.target;
            DrawLine(start.x, start.y, end.x, end.y, DEBUG_COLLISION_COLOR);
            break;
          }
          case CollisionShapeType::BARRIER:{
            const CollisionBarrier& barrier = *static_cast<const CollisionBarrier*>(shapePtr.get());
            Vector2 vectorAlongBarrier = barrier.get_unit_normal();
            vectorAlongBarrier = {vectorAlongBarrier.y, -vectorAlongBarrier.x}; //normal to the normal
            Vector2 start = position + barrier.offset - 1000*vectorAlongBarrier;
            Vector2 end = position + barrier.offset + 1000*vectorAlongBarrier;
            //DrawLineEx(start, end, BARRIER_THICKNESS, DEBUG_COLLISION_COLOR);
            DrawLine(start.x, start.y, end.x, end.y, DEBUG_COLLISION_COLOR);
            break;
          }
          case CollisionShapeType::RECT:{
            const CollisionRect& rect = *static_cast<const CollisionRect*>(shapePtr.get());
            Vector2 drawPos = position + rect.offset;
            DrawRectangleLines(drawPos.x, drawPos.y, rect.width, rect.height, DEBUG_COLLISION_COLOR);
            break;
          }
          case CollisionShapeType::CIRCLE:{
            const CollisionCircle& circle = *static_cast<const CollisionCircle*>(shapePtr.get());
            Vector2 center = position + circle.offset;
            DrawCircleLines(center.x, center.y, circle.radius, DEBUG_COLLISION_COLOR);
            break;
          }
          default: break;
        }
    }
}