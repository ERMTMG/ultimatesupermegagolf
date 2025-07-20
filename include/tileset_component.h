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

    TilesetTile() : collision(), texture(), id(-1) {
        std::cout << "TilesetTile: constructor called\n";
    }
    TilesetTile(const char* textureFilename);
    /*
        NOTE: this cost me an hour of debugging. Turns out, g++ compiler optimizations 
        eat up this destructor and act as if it never existed at all, even though it
        prints out that it's been called and it also calls SpriteLoader::return_texture.
        for some godforsaken reason, g++ thinks it isn't important to go ahead and
        UNLOAD THE FUCKING TEXTURES. so i have to force it to not optimize with this hacky
        __attribute__((noinline)). istg i'm going to kill somebody
    */
    ~TilesetTile() __attribute__((noinline)); 
    TilesetTile(const TilesetTile& other);
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


    TilesetComponent(){
        std::cout << "TilesetComponent: constructor called\n";
    }
    virtual ~TilesetComponent();

    TilesetComponent(const TilesetComponent&) = delete;
    TilesetComponent& operator=(const TilesetComponent&) = delete;

    TilesetComponent(TilesetComponent&&);
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

void draw_tileset(const TilesetComponent& tileset, const Position& pos);
