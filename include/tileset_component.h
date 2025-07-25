/*
    FILE: tileset_component.h
    Defines a component that manages a tilemap as one entity.
*/
#include"raylib.h"
#include"utility.h"
#include"collision_component.h"
#include"sprite_loader.h"
#include<unordered_map>
#include<vector>

using TileID = size_t;

/*
    Struct that handles all the information related to one tile on the tileset.
    Contains the collision and a texture for now, might have more in the future.
*/
struct TilesetTile {
    CollisionComponent collision;
    Texture texture;
    TileID id;

    // Constructs an empty invalid tile.
    TilesetTile() : collision(), texture(), id(-1) {}

    // Constructs a tile whose texture is the image included in the filename and
    // whose collision is a rectangle as large as the texture.
    // TODO: maybe make it able to support more shapes than just squares?
    TilesetTile(const char* textureFilename);
    /*
        NOTE: this cost me an hour of debugging. Turns out, g++ compiler optimizations 
        eat up this destructor and act as if it never existed at all, even though it
        prints out that it's been called and it also calls SpriteLoader::return_texture.
        for some godforsaken reason, g++ thinks it isn't important to go ahead and
        UNLOAD THE FUCKING TEXTURES. so i have to force it to not optimize with this hacky
        __attribute__((noinline)). istg i'm going to kill somebody.

        Destructor that unloads the tile texture.
    */
    ~TilesetTile() __attribute__((noinline)); 
    TilesetTile(const TilesetTile& other);
};

// Simple struct that stores a pair of two integers to be used as a key for the map in 
// TilesetComponent. I'm declaring a struct over just using std::pair<int, int> for 
// readability reasons.
struct TilePosition {
    int row;
    int col;
    bool operator==(const TilePosition& other) const {
        return (this->row == other.row && this->col == other.col);
    }
};

// std::hash specialization to be able to declare std::unordered_map<TilePosition>
template<>
struct std::hash<TilePosition>{
    size_t operator()(const TilePosition& pos) const noexcept {
        return size_t(pos.row << 10 + pos.col);
    }
};

/*
    Main component that handles both the list of available tiles and also the whole tilemap.
*/
struct TilesetComponent {
    // The vector that stores the information about all available tiles. This also
    // means that all valid TileIDs must be in the range [0, tiles.size), the only
    // exception being the null TileID (-1).
    std::vector<TilesetTile> tiles;

    // This maps stores, well, the tilemap. Every non-empty position is mapped to 
    // the ID of the tile in that position. Positions not containing any tile are
    // simply not stored. Is this more efficient than just storing a 2D array, or a
    // std::vector<std::vector>? i'm not sure, but this also allows me to place tiles
    // in negative positions
    std::unordered_map<TilePosition, TileID> map;

    // This defines the size of a grid square in the tilemap. It's used to determine
    // the separation that both tile sprites and tile collisions have between them when
    // they're rendered. Ideally, this should be equal to the tiles' textures' sizes, or
    // a common divisor of them.
    Vector2 tileSize;

    ;// I don't know why the copy constructor and assignment operator are deleted, 
    ;// but i don't think they're necessary either.
    TilesetComponent() = default;

    TilesetComponent(const TilesetComponent&) = delete;
    TilesetComponent& operator=(const TilesetComponent&) = delete;

    TilesetComponent(TilesetComponent&&) = default;
    TilesetComponent& operator=(TilesetComponent&&) = default;
};

// Adds a new tile to the tileset's tile list (doesn't place it anywhere), and 
// returns the new tile's ID. The parameter `tile` is copied over except for its
// ID, which is determined based on how many tiles `tileset` has already.
TileID tileset_add_new_tile(TilesetComponent& tileset, const TilesetTile& tile);

// Clears all of the tileset's information: available tiles, the tilemap and the tilesize.
void tileset_clear_all(TilesetComponent& tileset);

// If there's a tile placed at (row, col) in the tilemap, returns that tile's ID. 
// Else, returns -1 (null tile ID)
TileID tileset_get_tile_at(const TilesetComponent& tileset, int row, int col);

// Checks if there's a tile placed at (row, col) in the tilemap.
inline bool tileset_has_tile_at(const TilesetComponent& tileset, int row, int col){
    return tileset_get_tile_at(tileset, row, col) != -1;
}

// Returns the relative position that would be associated to the tile in (row, col)
// (the tilemap doesn't need to actually have a tile in that position). To get the
// _absolute_ position, add the returned position to the tilemap entity's position
// component.
inline Position tileset_get_tile_pos(const TilesetComponent& tileset, int row, int col){
    return Position{tileset.tileSize.x * col, tileset.tileSize.y * row};
}

// Places the tile with the given ID at the (row, col)-th position in the tileset's 
// tilemap. If id == -1, removes the tile at (row, col) instead (nothing happens if 
// there's no tile)
void tileset_place_tile(TilesetComponent& tileset, int row, int col, TileID id);

// Removes the tile placed at the (row, col)-th position in the tileset's tilemap.
// If there's no tile there, nothing happens.
void tileset_remove_tile(TilesetComponent& tileset, int row, int col);

// Removes all tiles with the given tile ID from the tilemap. If the given ID is
// -1, the function will remove every single tile in the tilemap instead.
void tileset_remove_all_tiles(TilesetComponent& tileset, TileID id = -1);

// Sets all tiles in the rectangle with upper left corner (beginRow, beginCol) and 
// lower right corner (endRow - 1, endCol - 1) to have the ID `fill`. If fill == -1,
// then the function removes all the tiles in the rectangle instead.
void tileset_fill_tiles(TilesetComponent& tileset, int beginRow, int endRow, int beginCol, int endCol, TileID fill);

// Constructs the combined collision of the whole tilemap (given each tile type's
// individual collision) and returns it through the reference parameter `collision`.
// TODO: optimize this by replacing large squares of contiguous square collisions
//       with only one rectangle collision. This function should only get called at
//       loadtime so it doesn't need to run fast.
void tileset_get_complete_collision(const TilesetComponent& tileset, CollisionComponent& collision);

// Draws the tilemap to the screen using each tile type's included texture. Meant
// to be used between Raylib's BeginDrawing()...EndDrawing() functions.
void draw_tileset(const TilesetComponent& tileset, const Position& pos);
