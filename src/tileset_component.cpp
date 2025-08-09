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
    map = util::Matrix<TileID>(gridRows, gridCols, -1);
    for(TileID tileID : map){
        if(tileID != -1){
            std::cout << "BAD BAD BAD BAD BAD THIS IS NOT -1 FOR SOME REASON SOFNISODNFSDGS\n";
        }
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

bool tileset_is_tile_in_range(const TilesetComponent &tileset, size_t row, size_t col)
{
    return (row < tileset.map.rows() && col < tileset.map.cols());
}

TileID tileset_get_tile_at(const TilesetComponent& tileset, size_t row, size_t col){
    return tileset.map[row][col];
    // This doesn't check whether [row][col] is a valid index for the map.
}

void tileset_fit_map_to_content(TilesetComponent& tileset){
    size_t emptyRowsAtEndBegin = -1; // Beggining of the section of empty rows at the end of the map. kind of an oxymoronic name, i know
    size_t emptyColsAtEndBegin = -1; // Same thing but with columns

    for(size_t i = 0; i < tileset.map.rows(); i++){
        bool isRowEmpty = true;
        for(size_t j = 0; j < tileset.map.cols(); j++){
            if(tileset.map[i][j] != -1){
                isRowEmpty = false;
                break;
            }
        }
        if(isRowEmpty){
            if(emptyRowsAtEndBegin == -1) emptyRowsAtEndBegin = i;
        } else {
            emptyRowsAtEndBegin = -1;
        }
    }
    for(size_t j = 0; j < tileset.map.cols(); j++){
        bool isColEmpty = false;
        for(size_t i = 0; i < tileset.map.rows(); i++){
            if(tileset.map[i][j] != -1){
                isColEmpty = false;
                break;
            }
        }
        if(isColEmpty){
            if(emptyColsAtEndBegin == -1) emptyColsAtEndBegin = j;
        } else {
            emptyColsAtEndBegin = -1;
        }
    }

    size_t targetRows = (emptyRowsAtEndBegin == -1) ? tileset.map.rows() : emptyColsAtEndBegin;
    size_t targetCols = (emptyColsAtEndBegin == -1) ? tileset.map.cols() : emptyColsAtEndBegin;
    tileset.map.resize(targetRows, targetCols);
}

void tileset_place_tile(TilesetComponent& tileset, size_t row, size_t col, TileID id){
    if(!tileset_is_tile_in_range(tileset, row, col)){
        tileset.map.resize(row+1, col+1, -1);
    }
    tileset.map[row][col] = id;
}

void tileset_remove_tile(TilesetComponent& tileset, size_t row, size_t col){
    tileset.map[row][col] = -1;
}

void tileset_remove_all_tiles(TilesetComponent& tileset, TileID targetID){
    if(targetID == -1){
        for(auto& tileID : tileset.map){
            tileID = -1;
        }
    } else {
        for(auto& tileID : tileset.map){
            if(tileID == targetID){
                tileID = -1;
            }
        }
    }
}

void tileset_fill_tiles(TilesetComponent& tileset, size_t beginRow, size_t endRow, size_t beginCol, size_t endCol, TileID fill){
    if(!tileset_is_tile_in_range(tileset, endRow, endCol)){
        tileset.map.resize(endRow+1, endCol+1);
    }
    for(int i = beginRow; i < endRow; i++){
        for(int j = beginCol; j < endCol; j++){
            tileset.map[i][j] = fill;
        }
    }
}

void tileset_get_complete_collision(const TilesetComponent& tileset, CollisionComponent& collision){
    collision.shapes.clear();
    collision.isStatic = true;
    size_t tilemapRows = tileset.map.rows();
    size_t tilemapCols = tileset.map.cols();
    for(size_t i = 0; i < tilemapRows; i++){
        for(size_t j = 0; j < tilemapCols; j++){
            TileID tileID = tileset.map[i][j];
            if(tileID < tileset.tiles.size()){
                const TilesetTile& tile = tileset.tiles[tileID];
                Position tilePos = tileset_get_tile_pos(tileset, i, j);
                add_collision(collision, tile.collision, tilePos);
            }
        }
    }
}

void draw_tileset(const TilesetComponent& tileset, const Position& pos){
    Vector2 posVector = to_Vector2(pos);
    size_t tilemapRows = tileset.map.rows();
    size_t tilemapCols = tileset.map.cols();
    for(size_t i = 0; i < tilemapRows; i++){
        for(size_t j = 0; j < tilemapCols; j++){
            TileID tileID = tileset.map[i][j];
            if(tileID < tileset.tiles.size()){
                const TilesetTile& tile = tileset.tiles[tileID];
                Vector2 tilePos = to_Vector2(tileset_get_tile_pos(tileset, i, j)) + posVector;
                Rectangle tileRect = {
                    .x = 0,
                    .y = 0,
                    .width = (float)tile.texture.width,
                    .height = (float)tile.texture.height,
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
}
