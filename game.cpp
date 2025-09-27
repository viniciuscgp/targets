#include "game.h"
#include <cstdlib>
#include <ctime>

const int TYPE_TIRO_NAVE = 1;
const int TYPE_INIM = 2;

const int TAG_INIMIGO_1 = 1;
const int TAG_INIMIGO_2 = 2;
const int TAG_INIMIGO_3 = 3;

const int   WAVE_ENERGY[] = {   5,    5,    5,   10,   15,   15,  15,   25,   25,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,   30,    30};
const int   WAVE_ENEMY[]  = {   3,    3,    4,    4,    5,    8,   9,    9,   10,   10,   10,   11,   11,   12,   12,   12,   13,   14,   15,   16,    17};
const float WAVE_SCALE[]  = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
const int   WAVA_KILLS[]  = {  10,   15,   30,   30,   30,   30,   30,   40,   40,   40,   40,   40,   40,   40,   40,   40,   40,   50,   50,   50,   50};

void TargetsGame::inimigoExplosao(Object o)
{
    string base = o.images[0] + "explode";
    for (int i = 0; i < EXPL_SPLIT; i++)
    {
        Object *go = g.createObject(o.x, o.y, o.getW() / 3, o.getH() / 3, base + to_string(i), 0, 2);
        go->image_speed = 0.03;
        go->angle_speed = g.choose({8, 15, 20});
        go->x_force = g.choose({-1, 1, -2, 2, -3, 3});
        go->y_force = g.choose({-1, 1, -2, 2, -3, 3});
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
    g.loadImage("assets/title.png", "title");
    g.loadImage("assets/game_over.png", "gover");
    g.loadImage("assets/push_space_key.png", "push");
    g.loadImage("assets/nave_1.png", "nave_1");
    g.loadImage("assets/nave_2.png", "nave_2");
    g.loadImage("assets/nave_tiro.png", "tiro");
    g.loadImage("assets/estrela.png", "estrela");

    for (int i = 0; i < TOTAL_ENEMY; i++)
    {
        string file = "assets/alien_" + to_string(i + 1) + ".png";
        string key = "alien_" + to_string(i + 1);
        g.loadImage(file, key);
        g.splitImage(key, EXPL_SPLIT, key + "explode");
    }

    g.loadSound("assets/nave_tiro.wav", "tiro");
    g.loadSound("assets/inimigo_explode.wav", "explosao");
    g.loadSound("assets/impact1.wav", "impact1");
}

void TargetsGame::criaObjetos(string_view qual)
{
    if (qual == "nave")
    {
        nave = g.createObject(400, 450, 64, 64, "nave_1", 0, 5);
        nave->images.push_back("nave_2");
        nave->image_speed = 0.2;
        nave->image_cycle = Object::LOOP;
        nave->setWrap(true, true);
        nave->onAfterCalculate = [this](Object *nave)
        {
            if (g.keyHeld(SDL_SCANCODE_RIGHT))  nave->x += 5;
            if (g.keyHeld(SDL_SCANCODE_LEFT))   nave->x -= 5;
            if (g.keyHeld(SDL_SCANCODE_UP))     nave->y -= 5;
            if (g.keyHeld(SDL_SCANCODE_DOWN))   nave->y += 5;
        };

        nave->collision_group = -1; // nao colide
        nave->setAlarm(7);
        nave->tag = 10;
        nave->onAlarmFinished = [](Object *nave, int index) {
            nave->tag--;
            if (nave->tag <= 0) 
            {
                nave->visible = true;
                nave->collision_group = 0;
            } else {
                nave->setAlarm(7);
                nave->visible = !nave->visible;
            }
        };

        return;
    }

    if (qual == "title")
    {    
        score  = 0;
        lifes  = 2;
        state  = 0;
        wave   = 1;

        title = g.createObject(0, 0, "title");
        title->setScale(0.4f);
        g.centerObject(title);
        return;
    }

    if (qual == "push")
    {
        push = g.createObject(0, g.getH() - 180, 0, 0, "push", 0, 0);
        g.centerXObject(push);
        push->onAfterCalculate = [this](Object *push) {
            if (g.keyPressed(SDL_SCANCODE_SPACE)) 
            {
                mudaEstado(0);
            }
        };
        return;
    }

    if (qual == "wave")
    {
        kills  = 0;
        display_wave = g.createObject(g.getW() / 2 - 32, g.getH() / 2 + 90, 64, 64, "", 0, 0);
        display_wave->setFont("FontdinerSwanky-Regular.ttf", 48, Object::COLOR_WHITE);
        display_wave->setAlarm(180);
        display_wave->onBeforeDraw = [this](Object *self)
        {
            self->text = "WAVE " + g.padzero(wave, 2);
        };
        display_wave->onAlarmFinished = [this](Object *self, int index)
        {
            this->mudaEstado(0);
        };
        return;
    }

    if (qual == "gameover")
    {
        gover = g.createObject(0, 0, g.getW() / 2, g.getW() / 2, "gover", 0, 0);
        gover->setAlarm(240);
        gover->onAlarmFinished = [this](Object *self, int index)
        {
            self->requestDestroy();
            this->mudaEstado(ST_TITLE);
        };
        g.centerObject(gover);
        return;
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
            self->text = "HI: " + g.padzero(hi, 4);
        };
        return;
    }

    if (qual == "tironave")
    {
        Object *o = g.createObject((int)nave->x + nave->getW() / 2 - 2,
                                   (int)nave->y, 12, 16, "tiro", TYPE_TIRO_NAVE, 3);
        o->y_force = -8;
        o->atack = 5;
        o->onAfterCalculate = [](Object *self)
        {
            if (self->y < -8) self->requestDestroy(); 
        };
        o->onCollision = [this](Object *me, Object *other)
        {
            if (other->type != TYPE_INIM)
                return;

            me->applyImpact(other);
            me->destroyIfLowEnergy();

            other->applyImpact(me);
            if (other->destroyIfLowEnergy())
            {
                inimigoExplosao(*other);
                g.playSound("explosao");
                score += 10;
                hi    += 10;
                kills += 1;
                if (kills >= WAVA_KILLS[wave] && g.countObjectTypes(TYPE_INIM) == 0) 
                {
                    wave++;
                    nave->requestDestroy();
                    display_score->requestDestroy();
                    display_hi->requestDestroy();
                    mudaEstado(ST_WAVE);
                }
            }
            else
            {
                other->angle_speed = g.choose(-4, -6, 8, 4, 6, 8);
                other->setImpulseDirection(90, g.choose(4, 5, 6));
                other->friction_impulse = 0.05;

                g.playSound("impact1");
            }
        };
        g.playSound("tiro");
        return;
    }

    if (qual == "inimigos")
    {
        for (int i = 0; i < WAVE_ENEMY[wave]; i++)
        {
            int tag = g.choose({TAG_INIMIGO_1, TAG_INIMIGO_2, TAG_INIMIGO_3});
            int r = (rand() % (i + 1)) % TOTAL_ENEMY;
            string image = "alien_" + to_string(r + 1);

            Object *o = g.createObject(i * 80, -300 + (tag * 70), 64, 64, image, TYPE_INIM, 1);

            o->setForce(g.choose(2, 3, 4, 5), g.choose(2.2f, 2.8f));
            o->setScale(g.choose(WAVE_SCALE[wave], WAVE_SCALE[wave], 0.5));
            o->setWrap(true, true);
            o->energy = WAVE_ENERGY[wave];
        }
        return;
    }
    if (qual == "estrelas")
    {
        for (int i = 0; i < TOTAL_STARS; i++)
        {
            Object *o = g.createObject(g.RANDOM_X, g.RANDOM_Y, 8, 8, "estrela", 0, 1);
            o->y_force = g.choose({1, 2, 3});
            o->collision_group = -1;
            o->setWrap(true, true);
        }
        return;
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
    
    mudaEstado(ST_TITLE);
    
    hi = 0;
    while (g.running)
    {
        if (state == ST_PLAYING)
        {
            if (g.keyPressed(SDL_SCANCODE_SPACE))
            {
                if (g.countObjectTypes(TYPE_TIRO_NAVE) < MAX_SHIP_FIRE)
                {
                    criaObjetos("tironave");
                }
            }
            if (g.countObjectTypes(TYPE_INIM) == 0)
            {
                criaObjetos("inimigos");
            }
        }
        g.calculateAndRender();
    }

    return 0;
}
