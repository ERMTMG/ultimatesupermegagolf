#include "tileset_component.h"
#include "entt.hpp"

TilesetTile::TilesetTile(const char* textureFilename){
    texture = SpriteLoader::load_or_get_texture(textureFilename);
    collision = CollisionComponent(new CollisionRect(VEC2_ZERO, texture.width, texture.height));
    id = -1;
}

TilesetTile::~TilesetTile() {
    SpriteLoader::return_texture(texture);
}

TilesetTile::TilesetTile(const TilesetTile& other){
    this->id = other.id;
    clone_collision(other.collision, this->collision);
    if(other.texture.id != 0) this->texture = SpriteLoader::get_texture_copy(other.texture);
}

TilesetComponent::TilesetComponent(TilesetComponent&& other){
    tiles = std::move(other.tiles);
    map = std::move(other.map);
    tileSize = other.tileSize;
}

TileID tileset_add_new_tile(TilesetComponent& tileset, const TilesetTile& tile){
    TileID newTileID = tileset.tiles.size();
    tileset.tiles.emplace_back(tile);
    tileset.tiles[newTileID].id = newTileID;
    return newTileID;
}

void tileset_clear_all(TilesetComponent& tileset){
    tileset.tiles.clear();
    tileset.map.clear();
    tileset.tileSize = VEC2_ZERO;
}

TileID tileset_get_tile_at(const TilesetComponent& tileset, int row, int col){
    auto itr = tileset.map.find({row, col});
    if(itr == tileset.map.end()){
        return -1;
    } else {
        return itr->second;
    }
}

void tileset_place_tile(TilesetComponent& tileset, int row, int col, TileID id){
    if(id == -1){
        tileset.map.erase({row, col});
    } else {
        tileset.map[{row, col}] = id;
    }
}

void tileset_remove_tile(TilesetComponent& tileset, int row, int col){
    tileset.map.erase({row, col});
}

void tileset_remove_all_tiles(TilesetComponent& tileset, TileID id){
    if(id == -1){
        tileset.map.clear();
    } else {
        for(auto itr = tileset.map.begin(); itr != tileset.map.end(); ){
            if(itr->second == id){
                itr == tileset.map.erase(itr);
            } else {
                ++itr;
            }
        }
    }
}

void tileset_fill_tiles(TilesetComponent& tileset, int beginRow, int endRow, int beginCol, int endCol, TileID fill){
    if(fill == -1){
        for(int row = beginRow; row < endRow; row++){
            for(int col = beginCol; col < endCol; col++){
                tileset.map.erase({row, col});
            }
        }
    } else {
        for(int row = beginRow; row < endRow; row++){
            for(int col = beginCol; col < endCol; col++){
                tileset.map[{row, col}] = fill;
            }
        }
    }
}

void tileset_get_complete_collision(const TilesetComponent& tileset, CollisionComponent& collision){
    collision.shapes.clear();
    collision.isStatic = true;
    for(const auto& [pos, tileID] : tileset.map){
        const TilesetTile& tile = tileset.tiles[tileID];
        Position tilePos = tileset_get_tile_pos(tileset, pos.row, pos.col);
        add_collision(collision, tile.collision, tilePos);
    }
}

void draw_tileset(const TilesetComponent& tileset, const Position& pos){
    Vector2 posVector = to_Vector2(pos);
    for(const auto& [pos, tileID] : tileset.map){
        const TilesetTile& tile = tileset.tiles[tileID];
        Vector2 tilePos = to_Vector2(tileset_get_tile_pos(tileset, pos.row, pos.col)) + posVector;
        Rectangle tileRect = {
            .x = 0,
            .y = 0,
            .width = tile.texture.width,
            .height = tile.texture.height,
        };
        Rectangle destRect = {
            .x = tilePos.x,
            .y = tilePos.y,
            .width = tileset.tileSize.x,
            .height = tileset.tileSize.y
        };
        DrawTexturePro(tile.texture, tileRect, destRect, VEC2_ZERO, 0, WHITE);
    }
}
