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
#include <array>
#include <utility>
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
    static constexpr const char *MUSIC_PREFIX   = "MUS";
    string currentMusicTag;

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
    // --- Helpers de AABB (funcionam com x/y sendo centro ou canto)
    static inline int objLeft  (const Object* o) { return o->centered ? int(o->x - o->getW()/2) : int(o->x); }
    static inline int objTop   (const Object* o) { return o->centered ? int(o->y - o->getH()/2) : int(o->y); }
    static inline int objRight (const Object* o) { return objLeft(o) + o->getW(); }
    static inline int objBottom(const Object* o) { return objTop(o)  + o->getH(); }

    inline bool quitRequested()                  { return inputSys.quitRequested(); }
    inline bool keyHeld(SDL_Scancode sc)         { return inputSys.keyHeld(sc); }
    inline bool keyPressed(SDL_Scancode sc)      { return inputSys.keyPressed(sc); }
    inline bool keyReleased(SDL_Scancode sc)     { return inputSys.keyReleased(sc); }

    inline bool  openGamepad(int index=0)                           { return inputSys.openGamepad(index); }
    inline void  closeGamepads()                                    { inputSys.closeGamepads(); }
    inline bool  padHeld(SDL_GameControllerButton btn, int i=0)     { return inputSys.padHeld(btn,i); }
    inline bool  padPressed(SDL_GameControllerButton btn, int i=0)  { return inputSys.padPressed(btn,i); }
    inline bool  padReleased(SDL_GameControllerButton btn, int i=0) { return inputSys.padReleased(btn,i); }
    inline Sint16 padAxis(SDL_GameControllerAxis axis, int i=0)     { return inputSys.padAxis(axis,i); }

    void requestDestroyAll();                // destroi todos
    void requestDestroyAllTypeBut(int type); // destroi todos
    void requestDestroy(Object* obj);        // destroi este objeto
    void requestDestroyByType(int type);     // destroi todos objetos do tipo type
    void requestDestroyByTag(int tag);       // destroi todos objetos da tag tag

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
        std::array<U, N> arr{{ static_cast<U>(std::forward<T>(first)), static_cast<U>(std::forward<Ts>(rest))... }};
        uniform_int_distribution<size_t> dist(0, N - 1);
        return arr[dist(rng())];
    }

    bool init(const char *title, int largura, int altura);

    void loadImage(string path, string tag);
    void splitImage(string baseImageRef, int numberOfParts, string baseTag);

    // >>> Parâmetro opcional fx (retrocompatível)
    void drawImage(string imageRef, int x, int y, int w, int h, float angle,
                   const FxParams* fx = nullptr);

    void drawObject(Object *go);

    void loadSound(string path, string tag);
    void playSound(string soundRef);

    // --- Música (Mix_Music) ---
    void loadMusic(const std::string& path, const std::string& tag);
    void playMusic(const std::string& tag, int loops = -1);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    bool musicIsPlaying() const;
    void setMusicVolume(int volume);
    string getCurrentMusicTag() const { return currentMusicTag; }    

    Object *createObject(int x, int y, int w, int h, string texture, int type = 0, int depth = 0);
    Object *createObject(int x, int y, string imageRef, int type);
    Object *createObject(int x, int y, string imageRef);
    Object *createObject(int x, int y, int type);
    Object *createObject(int x, int y);

    void centerXObject(Object *go);
    void centerYObject(Object *go);
    void centerObject(Object *go);

    int getW();
    int getH();

    TTF_Font* getFont(const string &name, int size);

    void drawText(const string &text, int x, int y, const string &fontName, int fontSize, SDL_Color color, bool centered);
    void drawRect(int x, int y, int w, int h, const Color& c, bool filled);
    void drawLine(int x1, int y1, int x2, int y2, const Color& c);
    void drawCircle(int cx, int cy, int radius, const Color& c, bool filled);
    void drawPoint(int x, int y, const Color& c);
    void drawLineRect(int x, int y, int w, int h, const Color& c);
    void drawGrid(int cellW, int cellH, const Color& c);
    void drawPolygon(const vector<pair<int,int>>& pts, const Color& c, bool closed=true);
    void drawCross(int cx, int cy, int size, const Color& c);

    void calculateAndRender();
    void calculateAll();
    void renderAll();
    void clear();

    string padzero(int n, int width);

    void processCollisions();
    int countObjectTypes(int type);
    int countObjectTags(int tag);
    int randRangeInt(int x, int y);
    int countObject();
    int countObjectDefuncts();    

    template <typename T>
    void log(const string &msg, const T &value) {
        cerr << msg << value << '\n';
    }

    Object* getObject(int i);
};
