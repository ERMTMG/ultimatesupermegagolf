#include"sprite_loader.h"
#include<stdexcept>
#include<algorithm>

#include<iostream>

SpriteLoader::TextureInfo::TextureInfo(const char* filepath) : texture(LoadTexture(filepath)), refCount(1), unloadOnDestruct(false) {}
SpriteLoader::TextureInfo::~TextureInfo(){
    if(unloadOnDestruct){
        if(refCount > 0) throw std::runtime_error("Unloading a texture with ref count greater than zero");
        UnloadTexture(texture);
    }
}

Texture SpriteLoader::load_new_texture_always(const char* filepath){
    std::string uniqueFileName(filepath);
    TextureInfo texture{filepath};
    while(_spriteFileMap.find(uniqueFileName) != _spriteFileMap.end()){
        uniqueFileName += '_';
    }
    _spriteFileMap.emplace(uniqueFileName, std::move(texture));
    _spriteFileMap[uniqueFileName].unloadOnDestruct = true;
    return texture.texture;
}

Texture SpriteLoader::load_or_get_texture(const char* filepath){
    auto itr = _spriteFileMap.find(filepath);
    if(itr != _spriteFileMap.end()){
        itr->second.refCount++;
        return itr->second.texture;
    } else {
        TextureInfo textureInfo{filepath};
        _spriteFileMap.emplace(filepath, std::move(textureInfo));
        _spriteFileMap[filepath].unloadOnDestruct = true;
        return textureInfo.texture;
    }
}

static bool operator==(const Texture& texture1, const Texture& texture2){
    return (texture1.id == texture2.id);
}

void SpriteLoader::return_texture(Texture texture){
    auto iter = std::find_if(_spriteFileMap.begin(), _spriteFileMap.end(), [&texture](const std::pair<const std::string, TextureInfo>& pair){
        return pair.second.texture == texture;
    });

    if(iter != _spriteFileMap.end()){
        iter->second.refCount--;
        if(iter->second.refCount == 0){
            _spriteFileMap.erase(iter);
        }
    }
}

Texture SpriteLoader::get_texture_copy(Texture texture){
    auto iter = std::find_if(_spriteFileMap.begin(), _spriteFileMap.end(), [&texture](const std::pair<const std::string, TextureInfo>& pair){
        return pair.second.texture == texture;
    });
    if(iter != _spriteFileMap.end()){
        iter->second.refCount++;
        return texture;
    } else {
        throw std::invalid_argument("Copying texture not registered by SpriteLoader");
    }
}

size_t SpriteLoader::_get_texture_ref_count(const char* filepath){
    auto iter = _spriteFileMap.find(filepath);
    if(iter != _spriteFileMap.end()){
        return iter->second.refCount;
    } else {
        return 0;
    }
}
