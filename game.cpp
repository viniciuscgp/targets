
#include "game.h"
#include <cstdlib>
#include <ctime>

const int TYPE_TIRO_NAVE  = 1;
const int TYPE_INIM       = 2;
const int TYPE_ENERGY     = 3;
const int TYPE_HUD        = 4;
const int TYPE_STAR       = 5;
const int TYPE_BACKGROUND = 6;

const int MAX_ENERGY = 100;

// 35                               1     2     3    4     5      6     7     8     9     10    11    12    13    14    15    16    17    18    19    20    21     22     23     24     25     26     27     28     29     30     31     32      33    34     35
const float INIM_FORCEX[] = {0.0f, 0.0f, 1.0f, 1.0f, 2.0f, 2.0f, 3.0f, 3.0f, 4.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 6.0f, 6.0f, 6.0f, 6.0f, 12.0f, 12.0f, 12.0f, 12.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f, 11.0f};
const float INIM_FORCEY[] = {3.0f, 3.0f, 3.0f, 3.0f, 1.0f, 1.0f, 3.0f, 3.0f, 4.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 3.0f, 2.0f, 2.0f, 2.0f, 2.0f, 3.0f, 3.0f, 3.0f, 3.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f, 4.0f};

// 21
const int WAVE_ENEMY_NUMBER[] = {5, 6, 7, 7, 7, 8, 9, 9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 14, 15, 16, 17};
const int WAVE_ENERGY[]       = {5, 5, 5, 10, 15, 15, 15, 25, 25, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30};
const float WAVE_SCALE[]      = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
const int WAVE_KILLS[]        = {10, 15, 30, 30, 30, 30, 30, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 50, 50, 50, 50};

const vector<vector<int>> WAVE_ENEMY_TYPE = {
    {1, 2},
    {3, 4},
    {4, 4, 5},
    {3, 4, 5},
    {3, 4, 6},
    {4, 4, 6},
    {6, 7, 8},
    {7, 8, 9},
    {9, 10, 11},
    {11, 12, 13},
    {12, 13, 14},
    {14, 15, 16},
    {17, 18, 19},
    {20, 21, 22},
    {23, 24, 25},
    {26, 26, 27},
    {28, 29, 30},
    {31, 32, 33},
    {33, 33, 34},
    {34, 35, 35},
    {34, 35, 35}};

const int MAX_SHIP_FIRE = 4;
const int TOTAL_STARS   = 60;
const int TOTAL_ENEMY   = 35;
const int EXPL_SPLIT    = 6;

// Estados
static const int ST_TITLE    = 1;
static const int ST_HISTORY  = 2;
static const int ST_WAVE     = 3;
static const int ST_PLAYING  = 4;
static const int ST_GAMEOVER = 5;

void TargetsGame::carregaRecursos()
{
    // Images
    g.loadImage("assets/images/title.png",            "title");
    g.loadImage("assets/images/game_over.png",        "gover");
    g.loadImage("assets/images/push_space_key2.png",  "push");
    g.loadImage("assets/images/nave_1.png",           "nave_1");
    g.loadImage("assets/images/nave_2.png",           "nave_2");
    g.loadImage("assets/images/nave_tiro.png",        "tiro");
    g.loadImage("assets/images/estrela.png",          "estrela");
    g.loadImage("assets/images/energy_drop_1.png",    "energy");
    g.loadImage("assets/images/game_over_alien1.png", "game_over_alien1");

    g.loadImage("assets/images/background.png",       "background");
    g.loadImage("assets/images/history.png",          "history");

    for (int i = 0; i < TOTAL_ENEMY; i++)
    {
        string file = "assets/images/alien_" + to_string(i + 1) + ".png";
        string key = "alien_" + to_string(i + 1);
        g.loadImage(file, key);
        g.splitImage(key, EXPL_SPLIT, key + "explode");
    }

    // Sound effects
    g.loadSound("assets/sounds/nave_tiro.wav",        "tiro");
    g.loadSound("assets/sounds/inimigo_explode.wav",  "explosao");
    g.loadSound("assets/sounds/impact1.wav",          "impact1");
    g.loadSound("assets/sounds/push_space.ogg",       "push_space");
    g.loadSound("assets/sounds/game_over_voice3.wav", "game_over");
    g.loadSound("assets/sounds/energy_get.wav",       "energy_get");

    // Musics
    g.loadMusic("assets/musics/title_music.wav",      "title_music");
    g.loadMusic("assets/musics/playing_music.wav",    "playing_music");
    g.loadMusic("assets/musics/game_over_music.wav",  "game_over_music");
}

void TargetsGame::inimigoExplosao(Object o)
{
    string base = o.images[0] + "explode";
    for (int i = 0; i < EXPL_SPLIT; i++)
    {
        Object *go = g.createObject(o.x, o.y, o.getW() / 3, o.getH() / 3, base + to_string(i), 0, 2);
        go->image_speed = 0.05; // so pra destruir
        go->angle_speed = g.choose({4, 8, 12});
        go->setForce(o.x_force / 2, o.y_force / 2);
        go->setImpulseDirection(g.choose(20, 40, 60, 80, 110, 130, 150, 170),  g.choose({3, 4, 5}));
        go->onAnimationEnd = [this](Object *self)
        {
            g.requestDestroy(self);
        };
    }
}

void TargetsGame::mudaEstado(int estado_mudar)
{
    if (estado_mudar == ST_TITLE)
    {
        g.requestDestroyAllTypeBut(TYPE_STAR);
        criaObjetos("background");
        criaObjetos("title");
        criaObjetos("push");
    }

    if (estado_mudar == ST_HISTORY)
    {
        criaObjetos("history");
    }

    if (estado_mudar == ST_WAVE)
    {
        g.requestDestroyAllTypeBut(TYPE_STAR);
        criaObjetos("wave");
    }

    if (estado_mudar == ST_PLAYING)
    {
        g.requestDestroyAllTypeBut(TYPE_STAR);
        criaObjetos("hud");
        criaObjetos("debug");
        criaObjetos("nave");
    }

    if (estado_mudar == ST_GAMEOVER)
    {
        g.requestDestroyAllTypeBut(TYPE_STAR);
        criaObjetos("gameover");
    }
    state = estado_mudar;
}

void TargetsGame::criaObjetos(string_view qual)
{
    if (qual == "title")
    {
        score  = 0;
        lifes  = 2;
        state  = 0;
        wave   = 1;
        g.setMusicVolume(music_volume);
        energy = MAX_ENERGY;

        title = g.createObject(0, 0, "title");
        title->setScale(0.4f);
        title->y = 20 + title->getH() / 2;
        title->setAlarm(100, 1); // mostra a historia
        title->setNeon(100, 100, 100, 1, 30);
        title->onAlarmFinished = [this](Object *title, int id)
        {
            if (id == 1) {
                mudaEstado(ST_HISTORY);    
            }
        };

        g.centerXObject(title);
        g.playMusic("title_music", 1);

        return;
    }

    if (qual == "background")
    {
        background = g.createObject(0, 0, g.getW(), g.getH(), "background", TYPE_BACKGROUND, 100);
        background->centered = false;
        return;
    }

    if (qual == "history")
    {
        history = g.createObject(g.getW() / 2, g.getH() * 2 - 120, 0, 0, "history", TYPE_HUD, -1);
        history->setScale(0.8);
        history->setForce(0, -0.8);
        history->centered = true;
        history->setNeon(110, 80, 80, 2, 255);
        history->onAfterCalculate=[this](Object *o){
            if (o->y + o->getH() / 2 < 0) {
                mudaEstado(ST_TITLE);
            }
            if (g.keyPressed(SDL_SCANCODE_SPACE))
            {
                o->visible = false;
            }
        };

        return;
    }

   if (qual == "push")
    {
        push = g.createObject(0, g.getH() - 100, 0, 0, "push", 0, 0);
        push->setScale(0.5);
        push->tag = 8;
        push->clearFx();
        push->setGlow(240, 240, 240, 4, 10);
        push->visible = true;
        g.centerXObject(push);
        
        push->onAfterCalculate = [this](Object *push)
        {
            Uint32 ms = SDL_GetTicks();
            float wave = 0.5f * (sinf(ms * 0.010f) + 1.0f);
            push->fx.glow_a = (Uint8)(150 + 105 * wave);

            if (g.keyPressed(SDL_SCANCODE_SPACE))
            {
                push->setAlarm(8, 1);
                g.playSound("push_space");
            }
        };

        push->onAlarmFinished = [this](Object *push, int id)
        {
            if (id == 1)
            {
                push->tag -= 1;
                if (push->tag <= 0)
                {
                    push->clearFx();
                    g.playMusic("playing_music", 1000);
                    mudaEstado(ST_WAVE);
                }
                else
                {
                    push->setAlarm(8, 1);
                    push->visible = !push->visible;
                }
            }
        };
        
        return;
    }    

    if (qual == "nave")
    {
        nave = g.createObject(400, 450, 64, 64, "nave_1", 0, 5);
        nave->images.push_back("nave_2");
        nave->image_speed = 0.2;
        nave->image_cycle = Object::LOOP;
        nave->setWrap(true, true);
        nave->onAfterCalculate = [this](Object *nave)
        {
            if (g.keyHeld(SDL_SCANCODE_RIGHT))
                nave->x += 5;
            if (g.keyHeld(SDL_SCANCODE_LEFT))
                nave->x -= 5;
            if (g.keyHeld(SDL_SCANCODE_UP))
                nave->y -= 5;
            if (g.keyHeld(SDL_SCANCODE_DOWN))
                nave->y += 5;
        };

        nave->collision_group = -1; // nao colide
        nave->setAlarm(7, 1);       // pisca enquanto imune
        nave->setAlarm(20, 2);      // gasta energia
        nave->tag = 10;
        nave->onAlarmFinished = [this](Object *nave, int id)
        {
            if (id == 1)
            {
                nave->tag--;
                if (nave->tag <= 0)
                {
                    nave->visible = true;
                    nave->collision_group = 0;
                }
                else
                {
                    nave->setAlarm(7, 1);
                    nave->visible = !nave->visible;
                }
            }
            else if (id == 2)
            {
                energy -= 1;
                if (energy > 0)
                    nave->setAlarm(20, 2);
                else
                {
                    mudaEstado(ST_GAMEOVER);
                }
            }
        };
        nave->onCollision = [this](Object *me, Object *other)
        {
            if (other->type == TYPE_ENERGY)
            {
                this->energy = min(this->energy + 10, MAX_ENERGY);
                other->requestDestroy();
                g.playSound("energy_get");
            }
        };

        return;
    }

    if (qual == "wave")
    {
        kills = 0;
        display_wave = g.createObject(0, 0, 64, 64, "", 0, 0);
        display_wave->setFont("FontdinerSwanky-Regular.ttf", 48, Object::COLOR_WHITE);
        display_wave->setAlarm(180, 0);
        g.centerObject(display_wave);
        display_wave->onBeforeDraw = [this](Object *self)
        {
            self->text = "WAVE " + g.padzero(wave, 2);
        };
        display_wave->onAlarmFinished = [this](Object *self, int id)
        {
            this->mudaEstado(ST_PLAYING);
        };
        return;
    }

    if (qual == "gameover")
    {
        Object *alien = g.createObject(0, 0, 0, 0, "game_over_alien1", TYPE_HUD, 0);
        alien->setScale(0.3);
        g.centerObject(alien);
        alien->y = alien->getH() / 2 + 10;

        gover = g.createObject(0, 0, 0, 0, "gover", TYPE_HUD, 0);
        gover->setAlarm(440, 0);
        gover->setScale(0.5);
        gover->onAlarmFinished = [this, alien](Object *self, int id)
        {
            self->requestDestroy();
            g.requestDestroyByType(TYPE_INIM);
            this->mudaEstado(ST_TITLE);
            alien->requestDestroy();
        };
        g.centerObject(gover);
        gover->y = gover->y + alien->getH() / 2;

        g.playSound("game_over");
        g.playMusic("game_over_music", 1);
        return;
    }

    if (qual == "hud")
    {
        hud_score = g.createObject(20, 10, 64, 64, "", TYPE_HUD, -5);
        hud_score->setFont("Roboto_Condensed-Black.ttf", 24, hud_score->withAlpha(Object::COLOR_YELLOW, 140));
        hud_score->centered = false;
        hud_score->onBeforeDraw = [this](Object *self)
        {
            self->text = "SCORE: " + g.padzero(score, 4);
        };

        hud_wave = g.createObject(160, 10, 64, 64, "", TYPE_HUD, -5);
        hud_wave->setFont("Roboto_Condensed-Black.ttf", 24, hud_score->withAlpha(Object::COLOR_WHITE, 170));
        hud_wave->centered = false;
        hud_wave->onBeforeDraw = [this](Object *self)
        {
            self->text = "W: " + to_string(wave);
        };

        hud_hi = g.createObject(g.getW() - 40 - 60, 10, 64, 64, "", TYPE_HUD, -5);
        hud_hi->setFont("Roboto_Condensed-Black.ttf", 24, hud_score->withAlpha(Object::COLOR_YELLOW, 140));
        hud_hi->centered = false;
        hud_hi->onBeforeDraw = [this](Object *self)
        {
            self->text = "HI: " + g.padzero(hi, 4);
        };

        hud_energy = g.createObject(20, g.getH() - 40, 8, 8, "", TYPE_HUD, -5);
        hud_energy->setFont("Roboto_Condensed-Black.ttf", 24, hud_score->withAlpha(Object::COLOR_YELLOW, 140));
        hud_energy->centered = false;
        hud_energy->onBeforeDraw = [this](Object *self)
        {
            int x = self->x + 130;
            int y = self->y + 5;
            self->text = "Ship Energy: ";
            g.drawRect(x, y, MAX_ENERGY, 20, Object::COLOR_WHITE, false);
            g.drawRect(x, y, energy, 20, Object::COLOR_CYAN, true);
        };

        return;
    }

    if (qual == "debug")
    {
        hud_debug = g.createObject(g.getW() - 80, g.getH() - 40, 64, 64, "", TYPE_HUD, -5);
        hud_debug->setFont("Roboto_Condensed-Black.ttf", 24, hud_debug->withAlpha(Object::COLOR_YELLOW, 140));
        hud_debug->centered = false;
        hud_debug->onBeforeDraw = [this](Object *self)
        {
            self->text = "c: " + std::to_string(g.countObject());
        };
        return;
    }

    if (qual == "tironave")
    {
        Object *o = g.createObject((int)nave->x, (int)nave->y, 12, 16, "tiro", TYPE_TIRO_NAVE, 3);
        o->setGlow(255, 0, 255, 5, 220);
        o->y_force = -8;
        o->atack = 5;
        o->onAfterCalculate = [](Object *self)
        {
            if (self->y < -8)
                self->requestDestroy();
            static uint8_t a = 160;
            a = (uint8_t)(160 + (std::sin(SDL_GetTicks() * 0.02) * 80)); // 160..240
            self->fx.glow_a = a;
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
                hi += 10;
                kills += 1;

                if (g.choose(1, 1, 2) == 2)
                {
                    Object *energy = g.createObject(other->x, other->y, "energy");
                    energy->type = TYPE_ENERGY;
                    energy->setScale(0.08f);
                    energy->setAngle(0, 1);
                    energy->setAlarm(5, 1);   // giro
                    energy->setAlarm(5, 2);   // muda direcao
                    energy->setAlarm(150, 3); // destroi
                    energy->setNeon(240, 230, 240, 5, 100);
                    energy->centered = true;
                    energy->onAlarmFinished = [this](Object *energy, int id)
                    {
                        if (id == 3)
                        {
                            energy->requestDestroy();
                        }
                        else if (id == 2)
                        {
                            energy->setDirection(rand() % 360, g.choose(1.0f, 2.0f));
                            energy->setAlarm(55, 2);
                        }
                        else
                        {
                            energy->setScale(energy->x_scale + 0.01f);
                            if (energy->x_scale > 0.2f)
                            {
                                energy->setScale(0.1f);
                            }
                            energy->setAlarm(5, 1);
                        }
                    };
                }
                if (kills >= WAVE_KILLS[wave] && g.countObjectTypes(TYPE_INIM) == 0)
                {
                    wave++;
                    nave->requestDestroy();
                    hud_score->requestDestroy();
                    hud_hi->requestDestroy();
                    hud_wave->requestDestroy();
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
        int in, x, y;
        float forcex, forcey, scale;
        string image;
        vector<int> usados;
        bool valido;
        int quantos = WAVE_ENEMY_NUMBER[wave - 1];
        for (int i = 0; i < quantos; i++)
        {
            bool valido = false;
            while (!valido)
            {
                x = g.randRangeInt(1, (g.getW() / 48) - 1) * 48;
                valido = true;

                for (int col : usados)
                {
                    if (col == x)
                    {
                        valido = false;
                        break;
                    }
                }
            }
            usados.push_back(x);
            y      = -g.choose(10, 20, 30, 40, 50, 60);
            in     = g.choose(WAVE_ENEMY_TYPE[wave - 1]) - 1; // 0 - x escolhe o tipo de inimigo
            forcex = g.choose(INIM_FORCEX[in]);
            forcey = g.choose(INIM_FORCEY[in], INIM_FORCEY[in], INIM_FORCEY[in] + 1, INIM_FORCEY[in] + 2);
            scale  = g.choose(WAVE_SCALE[wave], WAVE_SCALE[wave], 0.8f);

            image = "alien_" + to_string(in + 1); // obtem a imagem do inimigo
            Object *o = g.createObject(x, y, 48, 48, image);

            o->type  = TYPE_INIM;
            o->depth = 1;
            o->setForce(forcex, forcey);
            o->setScale(scale);
            o->setWrap(true, true);
            o->energy = WAVE_ENERGY[wave];
        }
        return;
    }

    if (qual == "estrelas")
    {
        for (int i = 0; i < TOTAL_STARS; i++)
        {
            Object *o = g.createObject(g.RANDOM_X, g.RANDOM_Y, 8, 8, "estrela", TYPE_STAR, 1);
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
