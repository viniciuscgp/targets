#include "game.h"
#include <cstdlib>
#include <ctime>

const int TAG_TIRO_NAVE = 1;
const int TAG_INIM = 2;

void TargetsGame::controlaJogador(Object *jogador)
{
    if (g.keyHeld(SDL_SCANCODE_RIGHT))
    {
        jogador->x += 5;
        if (jogador->x > jogador->engine->getW())
            jogador->x = -jogador->w;
    }
    if (g.keyHeld(SDL_SCANCODE_LEFT))
    {
        jogador->x -= 5;
        if (jogador->x < -jogador->w)
            jogador->x = jogador->engine->getW();
    }
}


void TargetsGame::inimigoExplosao(Object o)
{
    string base = o.images[0] + "explode";
    for (int i = 0; i < EXPL_SPLIT; i++)
    {
        Object *go = g.createObject(o.x, o.y, o.w / 3, o.h / 3, base + to_string(i), 0, 2);
        go->image_speed = 0.03;
        go->angle_speed = g.choose({8, 15, 20});
        go->force_x = g.choose({-1, 1, -2, 2, -3, 3});
        go->force_y = g.choose({-1, 1, -2, 2, -3, 3});
        go->onAnimationEnd = [this](Object *self)
        {
            g.requestDestroy(self);
        };
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

    if (estado_mudar == ST_TITLE)
    {
        criaObjetos("title");
        criaObjetos("push");
    }

    if (estado_mudar == ST_WAVE)
    {
        g.requestDestroy(title);
        g.requestDestroy(push);
        criaObjetos("wave");
    }

    if (estado_mudar == ST_PLAYING)
    {
        g.requestDestroy(display_wave);
        criaObjetos("score");
        criaObjetos("nave");
    }

    if (estado_mudar == ST_GAMEOVER)
    {
        g.requestDestroy(nave);
        criaObjetos("gameover");
    }
    state = estado_mudar;
}

void TargetsGame::carregaRecursos()
{
    g.loadImage("assets/title.png",          "title");
    g.loadImage("assets/game_over.png",      "gover");
    g.loadImage("assets/push_space_key.png", "push");
    g.loadImage("assets/nave_1.png",         "nave_1");
    g.loadImage("assets/nave_2.png",         "nave_2");
    g.loadImage("assets/nave_tiro.png",      "tiro");
    g.loadImage("assets/estrela.png",        "estrela");

    for (int i = 0; i < TOTAL_ENEMY; i++)
    {
        string file = "assets/alien_" + to_string(i + 1) + ".png";
        string key = "alien_" + to_string(i + 1);
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
    }

    if (qual == "push")
    {
        push = g.createObject(0, g.getH() - 180, 0, 0, "push", 0, 0);
        g.centerXObject(push);
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
    }

    if (qual == "nave")
    {
        nave = g.createObject(400, 400, 64, 64, "nave_1", 0, 5);
        nave->images.push_back("nave_2");
        nave->image_speed = 0.2;
        nave->image_cicle = Object::LOOP;
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
    }

    if (qual == "tironave") 
    {
        Object *o   = g.createObject((int)nave->x + nave->w / 2 - 2,
                                    (int)nave->y,
                                    12, 16,
                                    "tiro",
                                    0, 3);
        o->tag = TAG_TIRO_NAVE;
        o->force_y = -8;           
        o->onAfterCalculate=[](Object *self){if (self->y < -8) self->engine->requestDestroy(self);};                                         
        o->onCollision=[this](Object *me, Object *other){
            if (other->tag != TAG_INIM) return;
            me->requestDestroy();
            other->requestDestroy();
            inimigoExplosao(*other);
            g.playSound("explosao");
            score += 10;
            hi    += 10;                                
        };
        g.playSound("tiro");        
    }

    if (qual == "inimigos")
    {
        g.log("Criando inimigos...","");
        for (int i = 0; i < 10; i++)
        {
            int tipo = g.choose({T_INIMIGO_1, T_INIMIGO_2, T_INIMIGO_3});
            int r = rand() % TOTAL_ENEMY;
            string key = "alien_" + to_string(r + 1);

            Object *o = g.createObject(i * 80, -300 + (tipo * 70), 64, 64, key, tipo, 1);
            o->setForce(g.choose(2, 3, 4, 5), g.choose(0.8f, 1.2f));
            o->setWrap(true, true);
            o->tag = TAG_INIM;
        }
    }
    if (qual == "estrelas") 
    {
        for (int i = 0; i < TOTAL_STARS; i++)
        {
            Object *o = g.createObject(g.RANDOM_X, g.RANDOM_Y, 8, 8, "estrela", 0, 1);
            o->force_y = g.choose({1, 2, 3});
            o->setWrap(true, true);
        }
    }
}

// =========================================
// Loop principal
// =========================================
int TargetsGame::run()
{
    srand(time(nullptr));

    if (!g.init("Targets", 800, 600))
        return 1;

    carregaRecursos();
    criaObjetos("estrelas");

    SDL_Event e;
    bool rodando = true;
    mudaEstado(ST_TITLE);

    while (g.running)
    {
        if (g.keyPressed(SDL_SCANCODE_SPACE))
        {
            if (state == ST_PLAYING)
            {
                if (g.countObjectTags(TAG_TIRO_NAVE) < MAX_SHIP_FIRE)
                {
                    criaObjetos("tironave");
                }
            }
            else
            {
                mudaEstado(0);
            }
        }

        if (state == ST_PLAYING)
        {
            controlaJogador(nave);
            if (g.countObjectTags(TAG_INIM) == 0)
            {
                criaObjetos("inimigos");
            }
        }
        g.calculateAndRender();
    }

    return 0;
}
