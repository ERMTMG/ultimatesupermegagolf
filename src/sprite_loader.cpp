#include"sprite_loader.h"
#include<stdexcept>

// this assumes that constructing a texture means someone is gonna use it right now
SpriteLoader::TextureInfo::TextureInfo(const char* filepath) : texture(LoadTexture(filepath)), refCount(1) {}
SpriteLoader::TextureInfo::~TextureInfo(){
    if(refCount > 0) throw std::runtime_error("Unloading a texture with ref count greater than zero");
    UnloadTexture(texture);
}

Texture SpriteLoader::load_new_texture_always(const char* filepath){
    std::string uniqueFileName(filepath);
    TextureInfo texture(filepath);
    while(_spriteFileMap.find(uniqueFileName) != _spriteFileMap.end()){
        uniqueFileName += '_';
    }
    _spriteFileMap[uniqueFileName] = texture;
}

Texture SpriteLoader::load_or_get_texture(const char* filepath){
    auto itr = _spriteFileMap.find(filepath);
    if(itr != _spriteFileMap.end()){
        itr->second.refCount++;
        return itr->second.texture;
    } else {
        _spriteFileMap[filepath] = TextureInfo(filepath);
    }
}

void SpriteLoader::return_texture(Texture texture){
    // TODO: finish this. might need to rework
}