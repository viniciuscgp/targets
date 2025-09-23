#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <initializer_list>
#include <cstdlib>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <cstdarg>
#include <unordered_map>
#include "resources.h"
#include "gameobject.h"

class Engine
{
private:
    static constexpr const char* TEXTURE_PREFIX = "t";
    static constexpr const char* SOUND_PREFIX = "s";

    int w, h;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    std::unordered_map<std::string, GameResource> resources;

    // controle de objetos
    std::vector<std::unique_ptr<Object>> objects;
    std::vector<Object *> ordered_objects;

public:
    Engine() = default;
    ~Engine();

    bool init(const char *title, int largura, int altura);

    SDL_Renderer *getRenderer() { return renderer; }

    void loadImage(std::string path, std::string tag);
    void splitImage(std::string baseImageRef, int numberOfParts, std::string baseTag);   
    void drawImage(std::string imageRef, int x, int y, int w = 0, int h = 0);
    void drawObject(Object *go);

    bool checkCollision(const Object &a, const Object &b);

    void loadSound(std::string path, std::string tag);
    void playSound(std::string soundRef);

    Object *createObject(int x, int y, int w, int h, std::string texture, int type = 0, int depth = 0);

    void centerXObject(Object *go);
    void centerYObject(Object *go);
    void centerObject(Object *go);

    void destroyObject(Object *obj);

    int getW();
    int getH();

    int choose(int count, ...);
    int choose(std::initializer_list<int> values);

    void calculateAll();
    void renderAll();
    void clear();
};
