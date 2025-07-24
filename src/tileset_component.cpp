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

TilesetComponent::TilesetComponent(size_t gridRows, size_t gridCols) : TilesetComponent() {
    map.resize(gridRows);
    for(size_t i = 0; i < gridRows; i++){
        map[i] = std::vector<TileID>(gridCols, -1);
    }
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
    return tileset.map[row][col];
    // This doesn't check whether [row][col] is a valid index for the map.
}

void tileset_place_tile(TilesetComponent& tileset, int row, int col, TileID id){
    tileset.map[row][col] = id;
}

void tileset_remove_tile(TilesetComponent& tileset, int row, int col){
    tileset.map[row][col] = -1;
}

void tileset_remove_all_tiles(TilesetComponent& tileset, TileID targetID){
    if(targetID == -1){
        for(auto& row : tileset.map){
            for(auto& tileID : row){
                tileID = -1;
            }
        }
    } else {
        for(auto& row : tileset.map){
            for(auto& tileID : row){
                if(tileID == targetID){
                    tileID = -1;
                }
            }
        }
    }
}

void tileset_fill_tiles(TilesetComponent& tileset, int beginRow, int endRow, int beginCol, int endCol, TileID fill){
    for(int i = beginRow; i < endRow; i++){
        for(int j = beginCol; j < endCol; j++){
            tileset.map[i][j] = fill;
        }
    }
}

void tileset_get_complete_collision(const TilesetComponent& tileset, CollisionComponent& collision){
    collision.shapes.clear();
    collision.isStatic = true;
    size_t tilemapRows = tileset.map.size();
    size_t tilemapCols = (tilemapRows == 0 ? 0 : tileset.map[0].size());
    for(size_t i = 0; i < tilemapRows; i++){
        for(size_t j = 0; j < tilemapCols; j++){
            TileID tileID = tileset.map[i][j];
            const TilesetTile& tile = tileset.tiles[tileID];
            Position tilePos = tileset_get_tile_pos(tileset, i, j);
            add_collision(collision, tile.collision, tilePos);
        }
    }
}

void draw_tileset(const TilesetComponent& tileset, const Position& pos){
    Vector2 posVector = to_Vector2(pos);
    size_t tilemapRows = tileset.map.size();
    size_t tilemapCols = (tilemapRows == 0 ? 0 : tileset.map[0].size());
    for(size_t i = 0; i < tilemapRows; i++){
        for(size_t j = 0; j < tilemapCols; j++){
            const TilesetTile& tile = tileset.tiles[tileset.map[i][j]];
            Vector2 tilePos = to_Vector2(tileset_get_tile_pos(tileset, i, j)) + posVector;
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
}
