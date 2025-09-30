#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

class GameResource {
public:
    enum Type { NONE = 0, TEXTURE = 1, SOUND = 2, MUSIC = 3};
    
    Type type;
    SDL_Texture *texture;
    Mix_Chunk *sound;
    Mix_Music *music;
    
    GameResource() : type(NONE), texture(nullptr), sound(nullptr) {}
    
    static GameResource CreateTexture(SDL_Texture* tex) {
        GameResource res;
        res.type = TEXTURE;
        res.texture = tex;
        return res;
    }
    
    static GameResource CreateSound(Mix_Chunk* snd) {
        GameResource res;
        res.type = SOUND;
        res.sound = snd;
        return res;
    }
    
    static GameResource CreateMusic(Mix_Music* mus) {
        GameResource res;
        res.type = MUSIC;
        res.music = mus;
        return res;
    }

    bool isValid() const {
        return (type == TEXTURE && texture != nullptr) ||
               (type == SOUND   && sound   != nullptr) ||
               (type == MUSIC   && music   != nullptr);
    }
};