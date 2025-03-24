#include"sprite_loader.h"
#include<stdexcept>
#include<algorithm>

// this assumes that constructing a texture means someone is gonna use it right now
SpriteLoader::TextureInfo::TextureInfo(const char* filepath) : texture(LoadTexture(filepath)), refCount(1) {}
SpriteLoader::TextureInfo::~TextureInfo(){
    //if(refCount > 0) throw std::runtime_error("Unloading a texture with ref count greater than zero");
    UnloadTexture(texture);
}

Texture SpriteLoader::load_new_texture_always(const char* filepath){
    std::string uniqueFileName(filepath);
    TextureInfo texture{filepath};
    while(_spriteFileMap.find(uniqueFileName) != _spriteFileMap.end()){
        uniqueFileName += '_';
    }
    _spriteFileMap.emplace(uniqueFileName, std::move(texture));
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
        return textureInfo.texture;
    }
}

bool operator==(const Texture& texture1, const Texture& texture2){
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

size_t SpriteLoader::_get_texture_ref_count(const char* filepath){
    auto iter = _spriteFileMap.find(filepath);
    if(iter != _spriteFileMap.end()){
        return iter->second.refCount;
    } else {
        return 0;
    }
}
