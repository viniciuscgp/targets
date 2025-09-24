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
                inimigoExplosao(*inimigos[j]);
                g.destroyObject(inimigos[j]);
                inimigos.erase(inimigos.begin() + j);

                g.destroyObject(tiro);
                nave_tiros.erase(nave_tiros.begin() + i);
                atingiu = true;
                g.playSound("explosao");

                score += 10;
                hi += 10;

                break;
            }
        }

        if (!atingiu)
            i++;
    }
}

void TargetsGame::inimigoExplosao(Object o)
{
    string base = o.images[0] + "explode";
    for (int i = 0; i < EXPL_SPLIT; i++)
    {
        Object *go = g.createObject(o.x, o.y, o.w / 2, o.h / 2, base + to_string(i), 0, 2);
        go->image_speed = 0.02;
        go->angle_speed = g.choose({5, 8, 10});
        go->force_x = g.choose({-1, 1, -2, 2, -3, 3});
        go->force_y = g.choose({-1, 1, -2, 2, -3, 3});
        go->onAnimationEnd = [this](Object *self)
        {
            g.destroyObject(self);
        };
    }
}

void TargetsGame::controlaInimigos()
{
    for (auto *obj : inimigos)
    {
        if (obj->x > g.getW())
        {
            obj->x = -obj->w;
            obj->y += 32;
        }
        if (obj->y > g.getH())
        {
            obj->y = -obj->h;
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

        Object *obj = g.createObject(i * 80, -300 + (tipo * 70), 64, 64, key, tipo, 1);
        obj->force_x = g.choose(2, 3, 4, 5);
        obj->force_y = g.choose(0.8f, 1.2f);

        inimigos.push_back(obj);
    }
}

void TargetsGame::mudaEstado(int estado_mudar)
{
    if (estado_mudar == 0)
    {
        switch (state)
        {
        case ST_TITLE:
            estado_mudar = ST_WAVE;
            break;
        case ST_WAVE:
            estado_mudar = ST_PLAYING;
            break;
        case ST_PLAYING:
            estado_mudar = ST_GAMEOVER;
            break;
        case ST_GAMEOVER:
            estado_mudar = ST_TITLE;
            break;
        default:
            break;
        }
    }
    cerr << "estado atual:" << STATES[state]  << endl;      
    cerr << "estado mudar:" << STATES[estado_mudar]  << endl;      

    if (estado_mudar == ST_TITLE)
    {
        criaObjetos("title");
        criaObjetos("push");
    }

    if (estado_mudar == ST_WAVE)
    {
        g.destroyObject(title);
        g.destroyObject(push);
        criaObjetos("wave");
    }

    if (estado_mudar == ST_PLAYING)
    {
        g.destroyObject(display_wave);
        criaObjetos("score");
        criaObjetos("nave");
    }

    if (estado_mudar == ST_GAMEOVER)
    {
        g.destroyObject(nave);
        criaObjetos("gameover");
    }
    state = estado_mudar;
}

void TargetsGame::carregaRecursos()
{
    g.loadImage("assets/title.png", "title");
    g.loadImage("assets/game_over.png", "gover");
    g.loadImage("assets/push_space_key.png", "push");
    g.loadImage("assets/nave_1.png", "nave_1");
    g.loadImage("assets/nave_2.png", "nave_2");
    g.loadImage("assets/nave_tiro.png", "tiro");
    g.loadImage("assets/estrela.png", "estrela");

    for (int i = 0; i < TOTAL_ENEMY; i++)
    {
        std::string file = "assets/alien_" + std::to_string(i + 1) + ".png";
        std::string key = "alien_" + std::to_string(i + 1);
        g.loadImage(file, key);
        g.splitImage(key, EXPL_SPLIT, key + "explode");
    }

    g.loadSound("assets/nave_tiro.wav", "tiro");
    g.loadSound("assets/inimigo_explode.wav", "explosao");
}

void TargetsGame::criaObjetos(string qual)
{
    if (qual == "title")
    {
        title = g.createObject(0, 0, g.getW() / 2, g.getW() / 2, "title", 0, 0);
        g.centerObject(title);
        g.log("criado: ", qual);
    }

    if (qual == "push")
    {
        push = g.createObject(0, g.getH() - 180, 0, 0, "push", 0, 0);
        g.centerXObject(push);
        g.log("criado: ", qual);
    }

    if (qual == "wave")
    {
        display_wave = g.createObject(g.getW() / 2 - 32, g.getH() / 2 + 90, 64, 64, "", 0, 0);
        display_wave->setFont("FontdinerSwanky-Regular.ttf", 48, Object::COLOR_WHITE);
        display_wave->alarmSet(180);
        display_wave->onBeforeDraw = [this](Object *self)
        {
            self->text = "WAVE " + g.padzero(wave, 2);
        };
        display_wave->onAlarmFinished = [this](Object *self, int index)
        {
            this->mudaEstado(0);
        };
        g.log("criado: ", qual);
    }
    
    if (qual == "gameover")
    {
        gover = g.createObject(0, 0, g.getW() / 2, g.getW() / 2, "gover", 0, 0);
        gover->alarmSet(240);
        gover->onAlarmFinished = [this](Object *self, int index)
        {
            self->visible = false;
            this->mudaEstado(ST_TITLE);
        };
        g.centerObject(gover);
        g.log("criado: ", qual);
    }

    if (qual == "nave")
    {
        nave = g.createObject(400, 400, 64, 64, "nave_1", 0, 5);
        nave->images.push_back("nave_2");
        nave->image_speed = 0.2;
        nave->onAnimationEnd = [](Object *self){self->image_index = 0;};
        g.log("criado: ", qual);
    }

    if (qual == "score")
    {
        display_score = g.createObject(40, 60, 64, 64, "", 0, 0);
        display_score->setFont("Roboto_Condensed-Black.ttf", 24, Object::COLOR_YELLOW);
        display_score->onBeforeDraw = [this](Object *self)
        {
            self->text = "SCORE: " + g.padzero(score, 4);
        };

        display_hi = g.createObject(g.getW() - 40 - 60, 60, 64, 64, "", 0, 0);
        display_hi->setFont("Roboto_Condensed-Black.ttf", 24, Object::COLOR_YELLOW);
        display_hi->onBeforeDraw = [this](Object *self)
        {
            self->text = "HI: " + g.padzero(score, 4);
        };

        g.log("criado: ", qual);
    }
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

    for (int i = 0; i < TOTAL_STARS; i++)
    {
        Object *o = g.createObject(g.RANDOM_X, g.RANDOM_Y, 8, 8, "estrela", 0, 1);
        o->force_y = g.choose({1, 2, 3});
        o->setWrap(true, true);
        estrelas.push_back(o);
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
                                                    0, 3);
                        nave_tiros.push_back(go);
                        g.playSound("tiro");
                    }
                }
                else
                {
                    mudaEstado(0);
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
        g.calculateAll();
        g.renderAll();
        SDL_Delay(16);
    }

    return 0;
}
