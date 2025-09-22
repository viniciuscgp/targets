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

class GameObject
{
public:
    float x, y;
    float force_x, force_y;
    float friction;
    int w, h;
    int texture_index;
    int type;
    int depth;
    bool visible;

    GameObject(int x, int y, int w, int h, int texture_index, int type = 0, int depth = 0)
        : x(x), y(y), w(w), h(h), texture_index(texture_index), type(type), depth(depth), visible(true)
    {
        force_x = 0;
        force_y = 0;
        friction = 0;
    }

    void calculate();
};

class GameResource
{
public:
    int type;
    std::string tag;
    SDL_Texture *texture;
    Mix_Chunk *sound;
    GameResource(int type, const std::string &tag, SDL_Texture *texture = nullptr, Mix_Chunk *sound = nullptr)
        : type(type), tag(tag), texture(texture), sound(sound) {}
};

class Engine
{
private:
    int w, h;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    std::vector<GameResource> resource_list;

    // controle de objetos
    std::vector<std::unique_ptr<GameObject>> objects;
    std::vector<GameObject *> ordered_objects;

public:
    Engine() = default;
    ~Engine();

    bool init(const char *title, int largura, int altura);

    SDL_Renderer *getRenderer() { return renderer; }

    SDL_Texture *loadImage(const char *path);
    void drawImage(int texture_index, int x, int y, int w = 0, int h = 0);
    void drawObject(GameObject *go);

    bool checkCollision(const GameObject &a, const GameObject &b);

    int choose(int count, ...);
    int choose(std::initializer_list<int> values);

    SDL_Texture *getTexture(int texture_index);
    int loadTexture(const char *path, const char *tag);
    int loadTexture(const char *path);

    int loadSound(const char *path, const char *tag);
    int loadSound(const char *path);
    void playSound(int sound_index);

    GameObject *createGameObject(int x, int y, int w, int h, int texture_index, int type = 0, int depth = 0);
    GameObject *createGameObject(int x, int y, int w, int h, const char *texture_path, int type = 0, int depth = 0);

    void centerXGameObject(GameObject *go);
    void centerYGameObject(GameObject *go);
    void centerGameObject(GameObject *go);

    void destroyGameObject(GameObject *obj);

    int getW();
    int getH();

    void calculateAll();
    void renderAll();
    void clear();
};
