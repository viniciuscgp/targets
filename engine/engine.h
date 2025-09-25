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
#include <random>
#include "resources.h"
#include "gameobject.h"
#include "input.h"

struct FontKey {
    string name;
    int size;
    bool operator==(const FontKey &o) const { return name == o.name && size == o.size; }
};
struct FontKeyHash {
    size_t operator()(const FontKey &k) const {
        return hash<string>()(k.name) ^ (hash<int>()(k.size) << 1);
    }
};

class Engine {
private:
    static constexpr const char *TEXTURE_PREFIX = "TEX";
    static constexpr const char *SOUND_PREFIX   = "SND";
    Input inputSys;

    int w, h;
    SDL_Window   *window   = nullptr;
    SDL_Renderer *renderer = nullptr;
    unordered_map<string, GameResource> resources;

    // controle de objetos
    vector<unique_ptr<Object>> objects;
    vector<Object*> ordered_objects;
    unordered_map<FontKey, TTF_Font*, FontKeyHash> fontCache;
    
    vector<Object*> destroy_queue;    

    bool checkCollision(const Object &a, const Object &b);
    void destroyObject(Object *obj);
    void flushDestroyQueue();  

    static inline mt19937 &rng() {
        static thread_local mt19937 gen{ random_device{}() };
        return gen;
    }

    inline void inputBeginFrame()  { inputSys.beginFrame(); }

public:
    Engine() = default;
    ~Engine();

    static constexpr const int RANDOM_X = 99999999;
    static constexpr const int RANDOM_Y = 99999999;

    bool running;

// --- Wrappers de input (o jogo só chama Engine) ---
    inline bool quitRequested()              { return inputSys.quitRequested(); }
    inline bool keyHeld(SDL_Scancode sc)     { return inputSys.keyHeld(sc); }
    inline bool keyPressed(SDL_Scancode sc)  { return inputSys.keyPressed(sc); }
    inline bool keyReleased(SDL_Scancode sc) { return inputSys.keyReleased(sc); }

    inline bool  openGamepad(int index=0)         { return inputSys.openGamepad(index); }
    inline void  closeGamepads()                  { inputSys.closeGamepads(); }
    inline bool  padHeld(SDL_GameControllerButton btn, int i=0)     { return inputSys.padHeld(btn,i); }
    inline bool  padPressed(SDL_GameControllerButton btn, int i=0)  { return inputSys.padPressed(btn,i); }
    inline bool  padReleased(SDL_GameControllerButton btn, int i=0) { return inputSys.padReleased(btn,i); }
    inline Sint16 padAxis(SDL_GameControllerAxis axis, int i=0)     { return inputSys.padAxis(axis,i); }

    void requestDestroy(Object* obj); // agenda destruição

    template <typename T>
    static T choose(initializer_list<T> values) {
        if (values.size() == 0) return T{};
        uniform_int_distribution<size_t> dist(0, values.size() - 1);
        auto it = values.begin();
        advance(it, dist(rng()));
        return *it;
    }

    template <typename Container>
    static auto choose(const Container& c) -> typename Container::value_type {
        if (c.empty()) return typename Container::value_type{};
        uniform_int_distribution<size_t> dist(0, c.size() - 1);
        return *(begin(c) + dist(rng()));
    }

    template <typename T, typename... Ts>
    static decay_t<T> choose(T&& first, Ts&&... rest) {
        using U = decay_t<T>;
        constexpr size_t N = sizeof...(Ts) + 1;
        array<U, N> arr{{ forward<T>(first), forward<Ts>(rest)... }};
        uniform_int_distribution<size_t> dist(0, N - 1);
        return arr[dist(rng())];
    }


    bool init(const char *title, int largura, int altura);

    void loadImage(string path, string tag);
    void splitImage(string baseImageRef, int numberOfParts, string baseTag);
    void drawImage(string imageRef, int x, int y, int w, int h, float angle);
    void drawObject(Object *go);

    void loadSound(string path, string tag);
    void playSound(string soundRef);

    Object* createObject(int x, int y, int w, int h, string texture, int type = 0, int depth = 0);

    void centerXObject(Object *go);
    void centerYObject(Object *go);
    void centerObject(Object *go);

    int getW();
    int getH();

    TTF_Font* getFont(const string &name, int size);

    void drawText(const string &text, int x, int y,
                  const string &fontName, int fontSize,
                  SDL_Color color, bool centered);

    void calculateAndRender();
    void calculateAll();
    void renderAll();
    void clear();

    string padzero(int n, int width);

    void processCollisions();
    int countObjectTypes(int type);
    int countObjectTags(int tag);

    template <typename T>
    void log(const string &msg, const T &value) {
        cerr << msg << value << '\n';
    }
};
