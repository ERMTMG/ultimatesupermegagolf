/* 
    FILE: sprite_loader.h 
    Defines a module that manages the sprite loading and unloading, to prevent 
    Raylib from loading duplicate sprites. 
*/
#pragma once
#include"raylib.h"
#include<memory>
#include<string>
#include<unordered_map>

namespace SpriteLoader{
    // (private) Struct that saves the info and reference count of a specific texture.
    struct TextureInfo{
        size_t refCount;
        Texture texture;
        TextureInfo() = default;
        TextureInfo(const char* filepath); 
        // Loads a texture from the corresponding filepath. Sets its reference 
        // count to 1 automatically because it's assumed that constructing a 
        // texture means it's going to get used right now.
        TextureInfo(TextureInfo&&) = default;
        ~TextureInfo();
      private:
        // Field that controls whether the destructor should unload the texture when it's called. 
        // Used to prevent premature unloading when moving textures from local scope to global scope.
        bool unloadOnDestruct = false; 
        friend Texture load_new_texture_always(const char*); 
        friend Texture load_or_get_texture(const char*);
    };
    inline std::unordered_map<std::string, TextureInfo> _spriteFileMap;

    /* 
        Gets a newly loaded texture independently of if it's already loaded that filename.
        Probably not recommended to use this, because:
            1. it's the whole thing we're trying to avoid with the SpriteLoader
            2. it appends underscores to the filename to get a unique name in the _spriteFileMap
    */
    Texture load_new_texture_always(const char* filepath);

    /* 
        Gets the texture from the specified filepath and increases its ref count. If
        the texture isn't registered it loads it.
    */
    Texture load_or_get_texture(const char* filepath);

    /*
        Decreases the texture's ref count. To find the texture it linear-searches through the texture
        map so it's probably best not to do this on the fly. Of course, if the ref count goes to 0,
        the texture is unloaded.
    */
    void return_texture(Texture texture);

    /*
        Copies the texture (i.e., increases its ref count). This one also does a linear search.
    */
    Texture get_texture_copy(Texture texture);

    /*
        Don't know if this is useful at all honestly. Does what it says on the tin
    */
    size_t _get_texture_ref_count(const char* filepath);

} // namespace name
