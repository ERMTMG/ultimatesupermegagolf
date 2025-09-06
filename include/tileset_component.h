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

    enum class TileCollisionPreset {
        NONE = 0,
        SOLID,
        SLOPE_SW_TO_NE,
        SLOPE_NW_TO_SE,
        CIRCULAR
    };

    // Constructs an empty invalid tile.
    TilesetTile() : collision(), texture(), id(-1) {}

    // Constructs a tile whose texture is the image included in the filename and
    // whose collision is a rectangle as large as the texture.
    // TODO: maybe make it able to support more shapes than just squares?
    TilesetTile(const char* textureFilename, TileCollisionPreset preset = TileCollisionPreset::SOLID);
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


/*
    Main component that handles both the list of available tiles and also the whole tilemap.
*/
struct TilesetComponent {
    // The vector that stores the information about all available tiles. This also
    // means that all valid TileIDs must be in the range [0, tiles.size), the only
    // exception being the null TileID (-1).
    std::vector<TilesetTile> tiles;

    // This stores the tilemap itself, in a 2D array starting from offset (0,0).
    // Empty tiles are assigned the ID -1.
    util::Matrix<TileID> map;

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

    TilesetComponent(size_t gridRows, size_t gridCols);
};

// Adds a new tile to the tileset's tile list (doesn't place it anywhere), and
// returns the new tile's ID. The parameter `tile` is copied over except for its
// ID, which is determined based on how many tiles `tileset` has already.
TileID tileset_add_new_tile(TilesetComponent& tileset, const TilesetTile& tile);

// Clears all of the tileset's information: available tiles, the tilemap and the tilesize.
void tileset_clear_all(TilesetComponent& tileset);

// Returns true only if the position (row, col) is within range of the allocated tilemap
bool tileset_is_tile_in_range(const TilesetComponent& tileset, size_t row, size_t col);

// If there's a tile placed at (row, col) in the tilemap, returns that tile's ID.
// Else, returns -1 (null tile ID)
TileID tileset_get_tile_at(const TilesetComponent& tileset, size_t row, size_t col);

// Checks if there's a tile placed at (row, col) in the tilemap.
inline bool tileset_has_tile_at(const TilesetComponent& tileset, size_t row, size_t col){
    return tileset_get_tile_at(tileset, row, col) != -1;
}

// Returns the relative position that would be associated to the tile in (row, col)
// (the tilemap doesn't need to actually have a tile in that position). To get the
// _absolute_ position, add the returned position to the tilemap entity's position
// component.
inline Position tileset_get_tile_pos(const TilesetComponent& tileset, size_t row, size_t col){
    return Position{tileset.tileSize.x * col, tileset.tileSize.y * row};
}

// Resizes the tilemap grid so that all-empty rows or columns at the end of the map
// get removed
void tileset_fit_map_to_content(TilesetComponent& tileset);

// Places the tile with the given ID at the (row, col)-th position in the tileset's
// tilemap. If id == -1, removes the tile at (row, col) instead (nothing happens if
// there's no tile)
void tileset_place_tile(TilesetComponent& tileset, size_t row, size_t col, TileID id);

// Removes the tile placed at the (row, col)-th position in the tileset's tilemap.
// If there's no tile there, nothing happens.
void tileset_remove_tile(TilesetComponent& tileset, size_t row, size_t col);

// Removes all tiles with the given tile ID from the tilemap. If the given ID is
// -1, the function will remove every single tile in the tilemap instead.
void tileset_remove_all_tiles(TilesetComponent& tileset, TileID id = -1);

// Sets all tiles in the rectangle with upper left corner (beginRow, beginCol) and
// lower right corner (endRow - 1, endCol - 1) to have the ID `fill`. If fill == -1,
// then the function removes all the tiles in the rectangle instead.
void tileset_fill_tiles(TilesetComponent& tileset, size_t beginRow, size_t endRow, size_t beginCol, size_t endCol, TileID fill);

// Constructs the combined collision of the whole tilemap (given each tile type's
// individual collision) and returns it through the reference parameter `collision`.
// TODO: optimize this by replacing large squares of contiguous square collisions
//       with only one rectangle collision. This function should only get called at
//       loadtime so it doesn't need to run fast.
void tileset_get_complete_collision(const TilesetComponent& tileset, CollisionComponent& collision);

// Draws the tilemap to the screen using each tile type's included texture. Meant
// to be used between Raylib's BeginDrawing()...EndDrawing() functions.
void draw_tileset(const TilesetComponent& tileset, const Position& pos);
