#include"raylib.h"
#include"utility.h"
#include"collision_component.h"
#include"sprite_loader.h"
#include<unordered_map>
#include<vector>

using TileID = size_t;

struct TilesetTile {
    CollisionComponent collision;
    Texture texture;
    TileID id;

    TilesetTile() : collision(), texture(), id(-1) {}
    TilesetTile(const char* textureFilename);
};

struct TilePosition {
    int row;
    int col;
    bool operator==(const TilePosition& other) const {
        return (this->row == other.row && this->col == other.col);
    }
};

template<>
struct std::hash<TilePosition>{
    size_t operator()(const TilePosition& pos) const noexcept {
        return size_t(pos.row << 10 + pos.col);
    }
};

struct TilesetComponent {
    std::vector<TilesetTile> tiles;
    std::unordered_map<TilePosition, TileID> map;
    Vector2 tileSize;

    TilesetComponent(const TilesetComponent&) = delete;
    TilesetComponent& operator=(const TilesetComponent&) = delete;

    TilesetComponent(TilesetComponent&&) = default;
    TilesetComponent& operator=(TilesetComponent&&) = default;
};

TileID tileset_add_new_tile(TilesetComponent& tileset, const TilesetTile& tile);

void tileset_clear_all(TilesetComponent& tileset);

TileID tileset_get_tile_at(const TilesetComponent& tileset, int row, int col);

inline bool tileset_has_tile_at(const TilesetComponent& tileset, int row, int col){
    return tileset_get_tile_at(tileset, row, col) != -1;
}

inline Position tileset_get_tile_pos(const TilesetComponent& tileset, int row, int col){
    return Position{tileset.tileSize.x * col, tileset.tileSize.y * row};
}

void tileset_place_tile(TilesetComponent& tileset, int row, int col, TileID id);

void tileset_remove_tile(TilesetComponent& tileset, int row, int col);

void tileset_remove_all_tiles(TilesetComponent& tileset, TileID id = -1);

void tileset_fill_tiles(TilesetComponent& tileset, int beginRow, int endRow, int beginCol, int endCol, TileID fill);

void tileset_get_complete_collision(const TilesetComponent& tileset, CollisionComponent& collision);

void draw_tileset(const TilesetComponent& tileset);
