#include "game.h"
#include <cstdlib>
#include <ctime>

// =========================================
// Controle de objetos
// =========================================
void TargetsGame::controlaJogador(Object *jogador)
{
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_RIGHT])
        jogador->x += 5;
    if (state[SDL_SCANCODE_LEFT])
        jogador->x -= 5;
    // if (state[SDL_SCANCODE_DOWN])
    //     jogador->y += 5;
    // if (state[SDL_SCANCODE_UP])
    //     jogador->y -= 5;
}

void TargetsGame::controlaTirosJogador()
{
    size_t i = 0;
    while (i < nave_tiros.size())
    {
        Object *tiro = nave_tiros[i];
        tiro->y -= 8;

        if (tiro->y < 0)
        {
            g.destroyObject(tiro);
            nave_tiros.erase(nave_tiros.begin() + i);
            continue;
        }

        bool atingiu = false;
        for (size_t j = 0; j < inimigos.size(); j++)
        {
            if (g.checkCollision(*tiro, *inimigos[j]))
            {
                g.destroyObject(inimigos[j]);
                inimigos.erase(inimigos.begin() + j);

                g.destroyObject(tiro);
                nave_tiros.erase(nave_tiros.begin() + i);
                atingiu = true;
                g.playSound("explosao");
                break;
            }
        }

        if (!atingiu)
            i++;
    }
}

void TargetsGame::controlaInimigos()
{
    for (auto *obj : inimigos)
    {
        obj->y += 0.3;

        if (obj->type == T_INIMIGO_1)
            obj->x += 2;
        if (obj->type == T_INIMIGO_2)
            obj->x += 3;
        if (obj->type == T_INIMIGO_3)
            obj->x += 4;

        if (obj->x > g.getW())
        {
            obj->x = -64;
            obj->y += 32;
        }
        if (obj->y > g.getH())
        {
            obj->y = -80;
        }
    }
}

void TargetsGame::controlaEstrelas()
{
    for (auto *e : estrelas)
    {
        if (e->y > g.getH())
        {
            e->y = 0;
        }
    }
}

void TargetsGame::criaInimigos()
{
    for (int i = 0; i < 10; i++)
    {
        int tipo = g.choose({T_INIMIGO_1, T_INIMIGO_2, T_INIMIGO_3});
        int r = std::rand() % TOTAL_ENEMY;
        std::string key = "alien_" + std::to_string(r + 1);

        inimigos.push_back(g.createObject(i * 80, -300 + (tipo * 70),
                                              64, 64,
                                              key,
                                              tipo, 1));
    }
}

void TargetsGame::mudaEstado(int estado_mudar)
{
    title->visible = false;
    gover->visible = false;
    push->visible = false;
    nave->visible = false;

    if (estado_mudar == ST_PLAYING)
    {
        nave->visible = true;
    }
    if (estado_mudar == ST_TITLE)
    {
        title->visible = true;
        push->visible = true;
    }
    if (estado_mudar == ST_GAMEOVER)
    {
        gover->visible = true;
        push->visible = true;
    }
    state = estado_mudar;
}

void TargetsGame::carregaRecursos()
{
    g.loadImage("assets/title.png", "title");
    g.loadImage("assets/game_over.png", "gover");
    g.loadImage("assets/push_space_key.png", "push");
    g.loadImage("assets/nave.png", "nave");
    g.loadImage("assets/nave_tiro.png", "tiro");
    g.loadImage("assets/estrela.png", "estrela");

    for (int i = 0; i < TOTAL_ENEMY; i++)
    {
        std::string file = "assets/alien_" + std::to_string(i + 1) + ".png";
        std::string key = "alien_" + std::to_string(i + 1);
        g.loadImage(file, key);
        
    }

    g.loadSound("assets/nave_tiro.wav", "tiro");
    g.loadSound("assets/inimigo_explode.wav", "explosao");
}

// =========================================
// Loop principal
// =========================================
int TargetsGame::run()
{
    std::srand(std::time(nullptr));

    if (!g.init("Targets", 800, 600))
        return 1;

    carregaRecursos();

    // Cria os objetos --------------------

    title = g.createObject(0, 0, g.getW() / 2, g.getW() / 2, "title", 0, 0);
    gover = g.createObject(0, 0, g.getW() / 2, g.getW() / 2, "gover", 0, 0);
    push = g.createObject(0, g.getH() - 180, 0, 0, "push", 0, 0);
    nave = g.createObject(400, 500, 64, 64, "nave", T_NAVE, 5);

    g.centerObject(title);
    g.centerXObject(push);
    g.centerObject(gover);

    for (int i = 0; i < TOTAL_STARS; i++)
    {
        int x = std::rand() % g.getW();
        int y = std::rand() % g.getH();
        Object *go = g.createObject(x, y, 8, 8, "estrela", T_ESTRELA, 0);
        go->force_y = g.choose({1, 2, 3});
        estrelas.push_back(go);
    }

    SDL_Event e;
    bool rodando = true;
    mudaEstado(ST_TITLE);

    while (rodando)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                rodando = false;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
                rodando = false;

            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_SPACE)
            {
                if (state == ST_PLAYING)
                {
                    if (nave_tiros.size() < MAX_SHIP_FIRE)
                    {
                        Object *go = g.createObject((int)nave->x + nave->w / 2 - 2,
                                                            (int)nave->y,
                                                            12, 16,
                                                            "tiro",
                                                            T_TIRO, 2);
                        nave_tiros.push_back(go);
                        g.playSound("tiro");
                    }
                }
                else
                {
                    mudaEstado(ST_PLAYING);
                }
            }
        }

        if (state == ST_PLAYING)
        {
            controlaJogador(nave);
            controlaTirosJogador();
            controlaInimigos();
            if (inimigos.empty())
                criaInimigos();
        }
        controlaEstrelas();

        g.calculateAll();
        g.renderAll();
        SDL_Delay(16);
    }

    return 0;
}
