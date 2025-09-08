import json
from math import *
from numpy import random
LEVEL_FILENAME: str = "resources/levels_json/test_level_1.json"
type vec2 = tuple[float, float]
type entity_entry = tuple[str, dict]
current_num_entities = 0
true = True
false = False

def rotate(v: vec2, angle: float) -> vec2:
    return (v[0]*cos(angle) - v[1]*sin(angle), v[1]*cos(angle) + v[0]*sin(angle))

def to_json(v: vec2) -> dict:
    return {"x": v[0], "y": v[1]}

def get_jolly_obstacle(position: vec2, size: float = 1) -> entity_entry:
    global current_num_entities
    name: str = "Jolly_" + str(current_num_entities)
    entity = {
        "entity_default": "static_body",
        "layers": ["player"],
        "x": position[0], "y": position[1],
        "colliders": [
            {
                "type": "circle",
                "radius": 16 * size
            }
        ],
        "components": [
            {
                "type": "SpriteSheet",
                "texture": "resources/sprites/jolly_little_guy.png",
                "width": 32,
                "height": 32
            },
            {
                "type": "SpriteTransform",
                "scale": to_json((size, size))
            },
            {
                "type": "BoundingBoxComponent",
                "auto": true
            }
        ]
    }
    return (name, entity)

def get_thingy_at(position: vec2, rotation: float = 0) -> entity_entry:
    global current_num_entities
    name: str = "Thing_" + str(current_num_entities)
    points: list[vec2] = [(-20,10), (0, -15), (20, 10)]
    points = [rotate(point, rotation) for point in points]
    entity = {
        "entity_default": "static_body",
        "layers": ["player"],
        "x": position[0], "y": position[1],
        "colliders": [
          {
              "type": "line",
              "from": to_json(points[0]),
              "to": to_json(points[1])
          },
          {
              "type": "line",
              "from": to_json(points[1]),
              "to": to_json(points[2])
          },
          {
              "type": "line",
              "from": to_json(points[2]),
              "to": to_json(points[0])
          },
        ],
        "components": [
            {
                "type": "SpriteSheet",
                "texture": "resources/sprites/triangle_thing.png",
                "width": 40,
                "height": 25
            },
            {
                "type": "SpriteTransform",
                "rotation": degrees(rotation)
            },
            {
                "type": "BoundingBoxComponent",
                "auto": true
            }
        ]
    }
    return name,entity

def get_entities() -> dict:
    global current_num_entities

    entities: dict = {}
    angle: float = 0
    while angle < 2*pi:
        if(not isclose(angle, 3*pi/2)):
            pos: vec2 = rotate((0,225), angle)
            thing: entity_entry = get_thingy_at(pos, angle)
            current_num_entities += 1
            entities[thing[0]] = thing[1]
        angle += pi/22

    rng = random.default_rng()

    for _ in range(16):
        size: float = rng.normal(1, 0.25)
        if size <= 0.25: size = 0.25
        position: vec2 = rotate((rng.uniform(0, 200), 0), rng.uniform(0, 2*pi))
        jolly: entity_entry = get_jolly_obstacle(position, size)
        current_num_entities += 1
        entities[jolly[0]] = jolly[1]

    return entities

def main() -> None:
    level_data: dict = {
        "level_name": "level_script_test",
        "player_position": {"x": 0, "y": 0},
        "goal_position": {"x": 256, "y": 0},
        "entities": get_entities(),
    }
    with open(LEVEL_FILENAME, "w") as level_json:
        json_content: str = json.dumps(level_data, indent = 4)
        level_json.write(json_content + "\n")


if __name__ == "__main__":
    main()
