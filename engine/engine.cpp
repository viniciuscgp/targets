#include "engine.h"

Engine::~Engine()
{
    for (auto &[key, res] : resources)
    {
        if (res.type == GameResource::TEXTURE)
        {
            SDL_DestroyTexture(res.texture);
            res.texture = nullptr;
        }
        if (res.type == GameResource::SOUND)
        {
            Mix_FreeChunk(res.sound);
            res.sound = nullptr;
        }
    }

    resources.clear();

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

    // Inicializa solicitando suporte a PNG
    int initialized_flags = IMG_Init(IMG_INIT_PNG);

    // Verifica se o PNG foi realmente inicializado
    if ((initialized_flags & IMG_INIT_PNG) != IMG_INIT_PNG)
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

void Engine::drawImage(std::string image, int x, int y, int w, int h)
{
    SDL_Rect dst;
    dst.x = x;
    dst.y = y;
    dst.w = w;
    dst.h = h;

    SDL_RenderCopy(renderer, resources[TEXTURE_PREFIX + image].texture, nullptr, &dst);
}

void Engine::drawObject(Object *go)
{
    drawImage(go->getCurrentImageRef(), go->x, go->y, go->w, go->h);
}

void Engine::loadImage(std::string path, std::string tag)
{
    SDL_Surface *surface = IMG_Load(path.c_str());
    if (!surface)
    {
        std::cerr << "Erro IMG_Load: " << IMG_GetError() << std::endl;
        return;
    }
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    resources[TEXTURE_PREFIX + tag] = GameResource::CreateTexture(texture);
}

void Engine::playSound(std::string snd)
{
    Mix_Chunk *sound = resources[SOUND_PREFIX + snd].sound;
    Mix_PlayChannel(-1, sound, 0);
}

void Engine::loadSound(std::string path, std::string tag)
{
    Mix_Chunk *sound = Mix_LoadWAV(path.c_str());
    if (!sound)
    {
        std::cerr << "Erro Mix_LoadWAV: " << Mix_GetError() << std::endl;
        return;
    }
    resources[SOUND_PREFIX + tag] = GameResource::CreateSound(sound);
}

Object *Engine::createObject(int x, int y, int w, int h, std::string texture, int type, int depth)
{
    auto text = resources[texture].texture;

    int texW, texH;
    SDL_QueryTexture(text, nullptr, nullptr, &texW, &texH);
    if (w == 0)
        w = texW;
    if (h == 0)
        h = texH;

    auto obj = std::make_unique<Object>(x, y, w, h, texture, type, depth);
    Object *ptr = obj.get();
    objects.push_back(std::move(obj));
    ordered_objects.push_back(ptr);
    return ptr;
}

void Engine::centerXObject(Object *go)
{
    go->x = (w - go->w) / 2;
}

void Engine::centerYObject(Object *go)
{
    go->y = (h - go->h) / 2;
}

void Engine::centerObject(Object *go)
{
    go->x = (w - go->w) / 2;
    go->y = (h - go->h) / 2;
}

void Engine::destroyObject(Object *obj)
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
    for (Object *obj : ordered_objects)
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
              [](Object *a, Object *b)
              { return a->depth < b->depth; });

    // Renderiza
    for (Object *obj : ordered_objects)
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

bool Engine::checkCollision(const Object &a, const Object &b)
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
