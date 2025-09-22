#include "engine.h"

// =============================
// Engine
// =============================
const int RT_TEXTURE = 1;
const int RT_SOUND = 2;

void GameObject::calculate()
{
    y += force_y;
    x += force_x;
}

Engine::~Engine()
{
    for (GameResource res : resource_list)
    {
        if (res.type == RT_TEXTURE)
            SDL_DestroyTexture(res.texture);
    }
    resource_list.clear();

    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);

    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}

bool Engine::init(const char *title, int w, int h)
{
    this->w = w;
    this->h = h;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        std::cerr << "Erro SDL_Init: " << SDL_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow(title,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              w, h, SDL_WINDOW_SHOWN);
    if (!window)
    {
        std::cerr << "Erro CreateWindow: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer)
    {
        std::cerr << "Erro CreateRenderer: " << SDL_GetError() << std::endl;
        return false;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        std::cerr << "Erro IMG_Init: " << IMG_GetError() << std::endl;
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cerr << "Erro Mix_OpenAudio: " << Mix_GetError() << std::endl;
        return false;
    }

    return true;
}

SDL_Texture *Engine::loadImage(const char *path)
{
    SDL_Surface *surface = IMG_Load(path);
    if (!surface)
    {
        std::cerr << "Erro IMG_Load: " << IMG_GetError() << std::endl;
        return nullptr;
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return tex;
}

void Engine::drawImage(int texture_index, int x, int y, int w, int h)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(renderer, resource_list[texture_index].texture, nullptr, &dst);
}

void Engine::drawObject(GameObject *go)
{
    drawImage(go->texture_index, go->x, go->y, go->w, go->h);
}

bool Engine::checkCollision(const GameObject &a, const GameObject &b)
{
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

int Engine::choose(int count, ...)
{
    if (count <= 0)
        return -1;
    std::va_list args;
    va_start(args, count);
    int idx = std::rand() % count;
    int result = -1;
    for (int i = 0; i <= idx; ++i)
    {
        result = va_arg(args, int);
    }
    va_end(args);
    return result;
}

int Engine::choose(std::initializer_list<int> values)
{
    if (values.size() == 0)
        return -1;
    int idx = std::rand() % values.size();
    auto it = values.begin();
    std::advance(it, idx);
    return *it;
}

int Engine::loadTexture(const char *path, const char *tag)
{
    SDL_Texture *texture = loadImage(path);
    resource_list.push_back(GameResource(RT_TEXTURE, tag, texture, nullptr));
    return resource_list.size() - 1;
}

int Engine::loadTexture(const char *path)
{
    return loadTexture(path, "");
}

SDL_Texture *Engine::getTexture(int texture_index)
{
    return resource_list[texture_index].texture;
}

void Engine::playSound(int sound_index)
{
    Mix_Chunk *sound = resource_list[sound_index].sound;
    Mix_PlayChannel(-1, sound, 0);
}

int Engine::loadSound(const char *path, const char *tag)
{
    Mix_Chunk *sound = Mix_LoadWAV(path);
    if (!sound)
    {
        std::cerr << "Erro Mix_LoadWAV: " << Mix_GetError() << std::endl;
        return -1;
    }
    resource_list.push_back({RT_SOUND, tag, nullptr, sound});
    return resource_list.size() - 1;
}

int Engine::loadSound(const char *path)
{
    return loadSound(path, "");
}

GameObject *Engine::createGameObject(int x, int y, int w, int h, int texture_index, int type, int depth)
{
    auto text = getTexture(texture_index);

    int texW, texH;
    SDL_QueryTexture(resource_list[texture_index].texture, nullptr, nullptr, &texW, &texH);
    if (w == 0)
        w = texW;
    if (h == 0)
        h = texH;

    auto obj = std::make_unique<GameObject>(x, y, w, h, texture_index, type, depth);
    GameObject *ptr = obj.get();
    objects.push_back(std::move(obj));
    ordered_objects.push_back(ptr);
    return ptr;
}

GameObject *Engine::createGameObject(int x, int y, int w, int h, const char *texture_path, int type, int depth)
{
    int texture_index = loadTexture(texture_path, "");
    return createGameObject(x, y, w, h, texture_index, type, depth);
}

void Engine::centerXGameObject(GameObject *go)
{
    go->x = (w - go->w) / 2;
}

void Engine::centerYGameObject(GameObject *go)
{
    go->y = (h - go->h) / 2;
}

void Engine::centerGameObject(GameObject *go)
{
    go->x = (w - go->w) / 2;
    go->y = (h - go->h) / 2;
}

void Engine::destroyGameObject(GameObject *obj)
{
    ordered_objects.erase(std::remove(ordered_objects.begin(), ordered_objects.end(), obj), ordered_objects.end());

    for (auto it = objects.begin(); it != objects.end(); ++it)
    {
        if (it->get() == obj)
        {
            objects.erase(it);
            break;
        }
    }
}

void Engine::calculateAll()
{
    // calcula
    for (GameObject *obj : ordered_objects)
    {
        obj->calculate();
    }
}

void Engine::renderAll()
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Ordena
    std::sort(ordered_objects.begin(), ordered_objects.end(),
              [](GameObject *a, GameObject *b)
              { return a->depth < b->depth; });

    // Renderiza
    for (GameObject *obj : ordered_objects)
    {
        if (obj->visible)
            drawObject(obj);
    }

    SDL_RenderPresent(renderer);
}

void Engine::clear()
{
    ordered_objects.clear();
    objects.clear();
}

int Engine::getW()
{
    return w;
}

int Engine::getH()
{
    return h;
}
