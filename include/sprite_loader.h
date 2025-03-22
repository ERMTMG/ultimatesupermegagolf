#pragma once
#include"raylib.h"
#include<memory>
#include<unordered_map>

namespace SpriteLoader{
    struct TextureInfo{
        size_t refCount;
        Texture texture;

        TextureInfo(const char* filepath);
        ~TextureInfo();
    };
    std::unordered_map<std::string, TextureInfo> _spriteFileMap;

    /* 
    Gets a newly loaded texture independently of if it's already loaded that filename.
    Probably not recommended to use this, because:
        1. it's the whole thing we're trying to avoid with the SpriteLoader
        2. it appends underscores to the filename to get a unique name in the _spriteFileMap
    */
    Texture load_new_texture_always(const char* filepath);

    /* 
    Gets the texture from the specified filepath and increases its ref count. if
    the texture isn't registered it loads it.
    */
    Texture load_or_get_texture(const char* filepath);

    /*
    Decreases the texture's ref count. To find the texture it linear-searches through the texture
    map so it's probably best not to do this on the fly.
    */
    void return_texture(Texture texture);

    /*
    Don't know if this is useful at all honestly
    */
    size_t _get_texture_ref_count(const char* filepath);

} // namespace name
