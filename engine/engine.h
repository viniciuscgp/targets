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
    std::vector<std::unique_ptr<GameObject>> objects;
    std::vector<GameObject *> ordered_objects;

public:
    Engine() = default;
    ~Engine();

    bool init(const char *title, int largura, int altura);

    SDL_Renderer *getRenderer() { return renderer; }

    void drawImage(std::string texture, int x, int y, int w = 0, int h = 0);
    void drawObject(GameObject *go);

    bool checkCollision(const GameObject &a, const GameObject &b);

    SDL_Texture *loadImage(std::string path);
    void loadTexture(std::string path, std::string tag);

    void loadSound(std::string path, std::string tag);
    void playSound(std::string snd);

    GameObject *createGameObject(int x, int y, int w, int h, std::string texture, int type = 0, int depth = 0);

    void centerXGameObject(GameObject *go);
    void centerYGameObject(GameObject *go);
    void centerGameObject(GameObject *go);

    void destroyGameObject(GameObject *obj);

    int getW();
    int getH();

    int choose(int count, ...);
    int choose(std::initializer_list<int> values);

    void calculateAll();
    void renderAll();
    void clear();
};
