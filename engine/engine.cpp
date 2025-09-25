#include "engine.h"
#include <cmath> // floor

// Converter Color -> SDL_Color
static inline SDL_Color toSDL(const Color &c) {
    return SDL_Color{ c.r, c.g, c.b, c.a };
}

Engine::~Engine()
{
    for (auto &[key, res] : resources) {
        if (res.type == GameResource::TEXTURE) {
            SDL_DestroyTexture(res.texture);
            res.texture = nullptr;
        }
        if (res.type == GameResource::SOUND) {
            Mix_FreeChunk(res.sound);
            res.sound = nullptr;
        }
    }
    resources.clear();

    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);

    for (auto &kv : fontCache)
        if (kv.second) TTF_CloseFont(kv.second);
    fontCache.clear();

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
    TTF_Quit();
}

bool Engine::init(const char *title, int w, int h)
{
    this->w = w;
    this->h = h;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        log("Erro SDL_Init: ", SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              w, h, SDL_WINDOW_SHOWN);
    if (!window) {
        log("Erro CreateWindow: ", SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED /*| SDL_RENDERER_PRESENTVSYNC*/);
    if (!renderer) {
        log("Erro CreateRenderer: ", SDL_GetError());
        return false;
    }

    int initialized_flags = IMG_Init(IMG_INIT_PNG);
    if ((initialized_flags & IMG_INIT_PNG) != IMG_INIT_PNG) {
        log("Erro IMG_Init: ", IMG_GetError());
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        log("Erro Mix_OpenAudio: ", Mix_GetError());
        return false;
    }

    if (TTF_Init() != 0) {
        log("TTF_Init failed: ", TTF_GetError());
    }

    return true;
}

void Engine::drawImage(std::string imageRef, int x, int y, int w, int h, float angle)
{
    SDL_Rect dst{ x, y, w, h };

    auto it = resources.find(std::string(TEXTURE_PREFIX) + imageRef);
    if (it == resources.end() || !it->second.texture) return;

    if (angle <= 0.0f) {
        SDL_RenderCopy(renderer, it->second.texture, nullptr, &dst);
    } else {
        SDL_Point center{ dst.w / 2, dst.h / 2 };
        SDL_RenderCopyEx(renderer, it->second.texture, nullptr, &dst, angle, &center, SDL_FLIP_NONE);
    }
}

void Engine::drawObject(Object *go)
{
    if (go->onBeforeDraw) go->onBeforeDraw(go);

    // imagem
    const std::string img = go->getCurrentImageRef();
    if (!img.empty()) {
        drawImage(img, (int)go->x, (int)go->y, go->w, go->h, go->angle);
    }

    // texto acima
    if (!go->text.empty() && !go->font_name.empty()) {
        int fsize = 16;
        if (go->font_size > 0) fsize = go->font_size;

        TTF_Font *font = getFont(go->font_name, fsize);
        if (font) {
            int tw = 0, th = 0;
            TTF_SizeUTF8(font, go->text.c_str(), &tw, &th);

            const int padding = 2;
            int cx = (int)go->x + go->w / 2;
            int ty = (int)go->y - th - padding;

            drawText(go->text, cx, ty, go->font_name, fsize, toSDL(go->font_color), true);
        }
    }
}

void Engine::loadImage(std::string path, std::string tag)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (!surface) {
        log("Erro IMG_Load: ", IMG_GetError());
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    resources[std::string(TEXTURE_PREFIX) + tag] = GameResource::CreateTexture(texture);
}

void Engine::splitImage(std::string baseImageRef, int numberOfParts, std::string baseTag)
{
    std::string fullRef = std::string(TEXTURE_PREFIX) + baseImageRef;
    if (resources.find(fullRef) == resources.end()) {
        std::cerr << "Erro: Textura base '" << baseImageRef << "' não encontrada!\n";
        return;
    }

    SDL_Texture *originalTexture = resources[fullRef].texture;
    if (!originalTexture) {
        log("Erro: Textura base é nula! ", "");
        return;
    }

    if (numberOfParts <= 1) {
        log("Erro: Número de partes deve ser pelo menos 2! ", "");
        return;
    }

    Uint32 fmt; int access;
    int originalWidth, originalHeight;
    SDL_QueryTexture(originalTexture, &fmt, &access, &originalWidth, &originalHeight);

    int partsTop, partsBottom;
    if (numberOfParts % 2 == 0) {
        partsTop = numberOfParts / 2;
        partsBottom = numberOfParts / 2;
    } else {
        partsTop = (numberOfParts + 1) / 2;
        partsBottom = numberOfParts / 2;
    }

    int topH    = originalHeight / 2;
    int bottomH = originalHeight - topH;

    auto createSlice = [&](int rowY, int rowH, int cols, int &partIndex)
    {
        if (cols <= 0 || rowH <= 0) return;

        int colW = originalWidth / cols;

        for (int i = 0; i < cols; ++i) {
            int srcW = (i == cols - 1) ? (originalWidth - i * colW) : colW;
            if (srcW <= 0) continue;

            SDL_Texture *partTexture = SDL_CreateTexture(renderer, fmt, SDL_TEXTUREACCESS_TARGET, srcW, rowH);
            if (!partTexture) {
                log("Erro ao criar textura da parte ", std::to_string(partIndex) + " - " + SDL_GetError());
                ++partIndex;
                continue;
            }
            SDL_SetTextureBlendMode(partTexture, SDL_BLENDMODE_BLEND);

            SDL_SetRenderTarget(renderer, partTexture);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
            SDL_RenderClear(renderer);

            SDL_Rect srcRect{ i * colW, rowY, srcW, rowH };
            SDL_Rect dstRect{ 0, 0, srcW, rowH };
            SDL_RenderCopy(renderer, originalTexture, &srcRect, &dstRect);

            SDL_SetRenderTarget(renderer, nullptr);

            std::string partTag = baseTag + std::to_string(partIndex + 1);
            resources[std::string(TEXTURE_PREFIX) + partTag] = GameResource::CreateTexture(partTexture);

            ++partIndex;
        }
    };

    int partIndex = 0;
    createSlice(0,     topH,    partsTop,    partIndex);
    createSlice(topH,  bottomH, partsBottom, partIndex);
}

void Engine::playSound(std::string soundRef)
{
    Mix_Chunk *sound = resources[std::string(SOUND_PREFIX) + soundRef].sound;
    Mix_PlayChannel(-1, sound, 0);
}

void Engine::loadSound(std::string path, std::string soundRef)
{
    Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
    if (!sound) {
        log("Erro Mix_LoadWAV:  ", Mix_GetError());
        return;
    }
    resources[std::string(SOUND_PREFIX) + soundRef] = GameResource::CreateSound(sound);
}

Object *Engine::createObject(int x, int y, int w, int h, std::string imageRef, int type, int depth)
{
    if (resources.find(TEXTURE_PREFIX + imageRef) != resources.end())
    {        
        auto text = resources[TEXTURE_PREFIX + imageRef].texture;
        int texW, texH;
        SDL_QueryTexture(text, nullptr, nullptr, &texW, &texH);
        if (w == 0) w = texW;
        if (h == 0) h = texH;
    }
    
    if (x == this->RANDOM_X) x = rand() % getW();
    if (y == this->RANDOM_Y) y = rand() % getH();

    auto obj = make_unique<Object>(x, y, w, h, imageRef, type, depth);
    Object *ptr = obj.get();
    objects.push_back(std::move(obj));
    ordered_objects.push_back(ptr);
    ptr->engine = this;
    return ptr;
}

void Engine::centerXObject(Object *go) { go->x = (w - go->w) / 2; }
void Engine::centerYObject(Object *go) { go->y = (h - go->h) / 2; }
void Engine::centerObject (Object *go) { centerXObject(go); centerYObject(go); }

void Engine::destroyObject(Object *obj)
{
    ordered_objects.erase(std::remove(ordered_objects.begin(), ordered_objects.end(), obj), ordered_objects.end());
    for (auto it = objects.begin(); it != objects.end(); ++it) {
        if (it->get() == obj) { objects.erase(it); break; }
    }
}

TTF_Font *Engine::getFont(const std::string &name, int size)
{
    FontKey key{ name, size };
    auto it = fontCache.find(key);
    if (it != fontCache.end()) return it->second;

    TTF_Font *f = TTF_OpenFont(name.c_str(), size);
    if (!f) {
        std::cerr << "TTF_OpenFont failed (" << name << " " << size << "): " << TTF_GetError() << "\n";
        return nullptr;
    }
    fontCache[key] = f;
    return f;
}

void Engine::drawText(const std::string &text, int x, int y,
                      const std::string &fontName, int fontSize,
                      SDL_Color color, bool centered)
{
    if (text.empty()) return;

    TTF_Font *font = getFont(fontName, fontSize);
    if (!font) return;

    SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text.c_str(), color);
    if (!surf) return;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    if (!tex) { SDL_FreeSurface(surf); return; }

    SDL_Rect dst{ x, y, surf->w, surf->h };
    if (centered) {
        dst.x = x - surf->w / 2;
        dst.y = y - surf->h / 2;
    }

    SDL_RenderCopy(renderer, tex, nullptr, &dst);

    SDL_DestroyTexture(tex);
    SDL_FreeSurface(surf);
}

void Engine::calculateAll()
{
    for (Object *obj : ordered_objects) {
        obj->calculate();
    }
    processCollisions();
    flushDestroyQueue();
}

void Engine::renderAll()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // depth: maior primeiro (menor fica no topo, pois desenha por último)
    std::stable_sort(ordered_objects.begin(), ordered_objects.end(),
        [](Object *a, Object *b){ return a->depth > b->depth; });

    for (Object *obj : ordered_objects) {
        if (obj->visible) drawObject(obj);
    }

    SDL_RenderPresent(renderer);
    SDL_Delay(16);    
}

void Engine::clear()
{
    ordered_objects.clear();
    objects.clear();
}

int Engine::getW() { return w; }
int Engine::getH() { return h; }

bool Engine::checkCollision(const Object &a, const Object &b)
{
    return (a.x < b.x + b.w) &&
           (a.x + a.w > b.x) &&
           (a.y < b.y + b.h) &&
           (a.y + a.h > b.y);
}

std::string Engine::padzero(int n, int width)
{
    std::string s = std::to_string(n);
    if ((int)s.size() < width) s.insert(0, width - s.size(), '0');
    return s;
}

// ======================
// Colisões: Grid simples
// ======================

struct Engine_CellKey {
    int gx, gy;
    bool operator==(const Engine_CellKey& o) const { return gx == o.gx && gy == o.gy; }
};
struct Engine_CellKeyHash {
    size_t operator()(const Engine_CellKey& k) const {
        return (uint64_t(uint32_t(k.gx)) << 32) ^ uint32_t(k.gy);
    }
};
static constexpr int ENGINE_COLL_CELL = 64;

static inline bool Engine_rectOverlap(const Object* a, const Object* b) {
    return (a->x < b->x + b->w) &&
           (a->x + a->w > b->x) &&
           (a->y < b->y + b->h) &&
           (a->y + a->h > b->y);
}

static inline void Engine_putInCells(
    std::unordered_map<Engine_CellKey, std::vector<Object*>, Engine_CellKeyHash>& grid,
    Object* o)
{
    int x0 = int(std::floor(o->x)) / ENGINE_COLL_CELL;
    int y0 = int(std::floor(o->y)) / ENGINE_COLL_CELL;
    int x1 = int(std::floor(o->x + o->w - 1)) / ENGINE_COLL_CELL;
    int y1 = int(std::floor(o->y + o->h - 1)) / ENGINE_COLL_CELL;

    for (int gy = y0; gy <= y1; ++gy) {
        for (int gx = x0; gx <= x1; ++gx) {
            grid[{gx, gy}].push_back(o);
        }
    }
}

void Engine::processCollisions()
{
    std::unordered_map<Engine_CellKey, std::vector<Object*>, Engine_CellKeyHash> grid;
    grid.reserve(ordered_objects.size() * 2);

    // 1) distribui objetos visíveis nas células
    for (Object* o : ordered_objects) {
        if (!o || !o->visible) continue;
        Engine_putInCells(grid, o);
    }

    // 2) testa pares por célula
    for (auto& kv : grid) {
        auto& v = kv.second;
        const size_t n = v.size();
        for (size_t i = 0; i < n; ++i) {
            Object* a = v[i];
            for (size_t j = i + 1; j < n; ++j) {
                Object* b = v[j];
                if (Engine_rectOverlap(a, b)) {
                    if (a->onCollision) a->onCollision(a, b);
                    if (b->onCollision) b->onCollision(b, a);
                }
            }
        }
    }
}

int Engine::countObjectTypes(int type)
{
    int i = 0;
    for (Object* o : ordered_objects) {
        if (o->type == type) i++;
    }
    return i;
}   

int Engine::countObjectTags(int tag)
{
    int i = 0;
    for (Object* o : ordered_objects) {
        if (o->tag == tag) i++;
    }
    return i;
}


void Engine::requestDestroy(Object* obj) {
    if (!obj) return;

    if (find(destroy_queue.begin(), destroy_queue.end(), obj) == destroy_queue.end()) {
        destroy_queue.push_back(obj);
    }
}

void Engine::flushDestroyQueue() {
    // destrói de fato (fora de colisão/desenho)
    for (Object* obj : destroy_queue) {
        if (!obj) continue;
        // só destrói se ainda estiver vivo no Engine
        auto it = std::find(ordered_objects.begin(), ordered_objects.end(), obj);
        if (it != ordered_objects.end()) {
            destroyObject(obj); // usa sua função atual
        }
    }
    destroy_queue.clear();
}

