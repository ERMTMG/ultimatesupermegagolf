#include "tileset_component.h"

TileID tileset_add_new_tile(TilesetComponent& tileset, const TilesetTile& tile){
    TileID newTileID = tileset.tiles.size();
    tileset.tiles.push_back(TilesetTile{
        .collision = CollisionComponent(), //TODO: copy collision component over to the tile in tileset.tiles
        .texture = tile.texture,
        .id = newTileID
    });
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
    collision.layerFlags = 0;
    collision.isStatic = true;
    for(const auto& [pos, tileID] : tileset.map){
        const TilesetTile& tile = tileset.tiles[tileID];
        Vector2 tilePos = tileset_get_tile_pos(tileset, pos.row, pos.col);
        for(const auto& shape : tile.collision.shapes){
            //TODO: complete this, i gotta make sure the shapes are being cloned correctly
        }
    }
}

void draw_tileset(const TilesetComponent& tileset){
    for(const auto& [pos, tileID] : tileset.map){
        const TilesetTile& tile = tileset.tiles[tileID];
        Vector2 tilePos = tileset_get_tile_pos(tileset, pos.row, pos.col);
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
