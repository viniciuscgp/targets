#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
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
#include <random>

struct FontKey
{
    std::string name;
    int size;
    bool operator==(const FontKey &o) const { return name == o.name && size == o.size; }
};
struct FontKeyHash
{
    size_t operator()(const FontKey &k) const
    {
        return std::hash<std::string>()(k.name) ^ (std::hash<int>()(k.size) << 1);
    }
};

class Engine
{
private:
    static constexpr const char *TEXTURE_PREFIX = "t";
    static constexpr const char *SOUND_PREFIX = "s";

    int w, h;
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    unordered_map<string, GameResource> resources;

    // controle de objetos
    vector<unique_ptr<Object>> objects;
    vector<Object *> ordered_objects;
    unordered_map<FontKey, TTF_Font *, FontKeyHash> fontCache;

public:
    static constexpr const int RANDOM_X = 99999999;
    static constexpr const int RANDOM_Y = 99999999;

    // RNG único por thread (bem leve)
    static inline std::mt19937 &rng()
    {
        static thread_local std::mt19937 gen{std::random_device{}()};
        return gen;
    }

    // 1) choose({a, b, c})
    template <typename T>
    static T choose(std::initializer_list<T> values)
    {
        if (values.size() == 0)
            return T{};
        std::uniform_int_distribution<std::size_t> dist(0, values.size() - 1);
        auto it = values.begin();
        std::advance(it, dist(rng()));
        return *it; // retorna por valor
    }

    // 2) choose(container) — vector/deque/etc.
    template <typename Container>
    static auto choose(const Container &c) -> typename Container::value_type
    {
        if (c.empty())
            return typename Container::value_type{};
        std::uniform_int_distribution<std::size_t> dist(0, c.size() - 1);
        return *(std::begin(c) + dist(rng()));
    }

    // 3) choose(a, b, c, ...) — parâmetros variádicos
    template <typename T, typename... Ts>
    static std::decay_t<T> choose(T &&first, Ts &&...rest)
    {
        using U = std::decay_t<T>;
        constexpr std::size_t N = sizeof...(Ts) + 1;
        std::array<U, N> arr{{std::forward<T>(first), std::forward<Ts>(rest)...}};
        std::uniform_int_distribution<std::size_t> dist(0, N - 1);
        return arr[dist(rng())];
    }

    Engine() = default;
    ~Engine();

    bool init(const char *title, int largura, int altura);

    SDL_Renderer *getRenderer() { return renderer; }

    void loadImage(std::string path, std::string tag);
    void splitImage(std::string baseImageRef, int numberOfParts, std::string baseTag);
    void drawImage(std::string imageRef, int x, int y, int w, int h, float angle);
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

    TTF_Font *getFont(const std::string &name, int size);

    void drawText(const std::string &text, int x, int y,
                  const std::string &fontName, int fontSize,
                  SDL_Color color, bool centered);

    void calculateAll();
    void renderAll();
    void clear();

    string padzero(int n, int width);

    template <typename T>
    void log(const std::string &msg, const T &value)
    {
        std::cerr << msg << value << '\n';
    }
};
