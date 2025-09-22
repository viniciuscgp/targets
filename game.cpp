#include "game.h"
#include <cstdlib>
#include <ctime>

// =========================================
// Controle de objetos
// =========================================
void TargetsGame::controlaJogador(GameObject *jogador)
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
        GameObject *tiro = nave_tiros[i];
        tiro->y -= 8;

        if (tiro->y < 0)
        {
            g.destroyGameObject(tiro);
            nave_tiros.erase(nave_tiros.begin() + i);
            continue;
        }

        bool atingiu = false;
        for (size_t j = 0; j < inimigos.size(); j++)
        {
            if (g.checkCollision(*tiro, *inimigos[j]))
            {
                g.destroyGameObject(inimigos[j]);
                inimigos.erase(inimigos.begin() + j);

                g.destroyGameObject(tiro);
                nave_tiros.erase(nave_tiros.begin() + i);
                atingiu = true;
                g.playSound(explosao_sn);
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

        if (obj->type == T_INIMIGO_1) obj->x += 2;
        if (obj->type == T_INIMIGO_2) obj->x += 3;
        if (obj->type == T_INIMIGO_3) obj->x += 4;

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
        int r = std::rand() % inimigos_texturas.size();
        inimigos.push_back(g.createGameObject(i * 80, -300 + (tipo * 70),
                                                   64, 64, inimigos_texturas[r], tipo, 1));
    }
}

void TargetsGame::mudaEstado(int estado_mudar)
{
    title->visible = false;
    over->visible = false;
    push->visible = false;
    nave->visible = false;

    if (estado_mudar == STATE_PLAYING)
    {
        nave->visible = true;
    }
    if (estado_mudar == STATE_TITLE)
    {
        title->visible = true;
        push->visible = true;
    }
    if (estado_mudar == STATE_GAMEOVER)
    {
        over->visible = true;
        push->visible = true;
    }
    state = estado_mudar;
}

// =========================================
// Loop principal
// =========================================
int TargetsGame::run()
{
    std::srand(std::time(nullptr));

    if (!g.init("Targets", 800, 600))
        return 1;

    // Recursos
    title = g.createGameObject(0, 0, g.getW() / 2, g.getW() / 2, "assets/title.png", 0, 0);
    over = g.createGameObject(0, 0, g.getW() / 2, g.getW() / 2, "assets/game_over.png", 0, 0);
    push = g.createGameObject(0, g.getH() - 180, 0, 0, "assets/push_space_key.png", 0, 0);
    nave = g.createGameObject(400, 500, 64, 64, "assets/nave.png", T_NAVE, 5);

    g.centerGameObject(title);
    g.centerXGameObject(push);
    g.centerGameObject(over);

    tiro_tx = g.loadTexture("assets/nave_tiro.png");
    tiro_sn = g.loadSound("assets/nave_tiro.wav");
    explosao_sn = g.loadSound("assets/inimigo_explode.wav");

    for (int i = 0; i < 7; i++)
    {
        std::string file = "assets/alien_" + std::to_string(i + 1) + ".png";
        inimigos_texturas.push_back(g.loadTexture(file.c_str()));
    }

    int estrela_tx = g.loadTexture("assets/estrela.png");
    for (int i = 0; i < TOTAL_ESTRELA; i++)
    {
        int x = std::rand() % g.getW();
        int y = std::rand() % g.getH();
        GameObject* go = g.createGameObject(x, y, 8, 8, estrela_tx, T_ESTRELA, 0);
        go->force_y = g.choose({1, 2, 3});
        estrelas.push_back(go);
    }

    SDL_Event e;
    bool rodando = true;
    mudaEstado(STATE_TITLE);

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
                if (state == STATE_PLAYING)
                {
                    if (nave_tiros.size() < MAX_NAVE_TIRO)
                    {
                        GameObject* go = g.createGameObject((int)nave->x + nave->w / 2 - 2, (int)nave->y,12, 16, tiro_tx, T_TIRO, 2);
                        nave_tiros.push_back(go);
                        g.playSound(tiro_sn);
                    }
                }
                else
                {
                    mudaEstado(STATE_PLAYING);
                }
            }
        }

        if (state == STATE_PLAYING)
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
