#pragma once
#include "engine/engine.h"
#include <vector>
#include <string>
#include <unordered_map>

class TargetsGame {
public:
    Engine g;

    Object* title = nullptr;
    Object* push  = nullptr;
    Object* nave  = nullptr;
    Object* gover = nullptr;
    Object* display_score = nullptr;
    Object* display_hi = nullptr;
    Object* display_wave = nullptr;

    int score  = 0;
    int hi     = 0;
    int lifes  = 2;
    int state  = 0;
    int wave   = 1;
    int kills  = 0;

    const int MAX_SHIP_FIRE = 4;
    const int TOTAL_STARS   = 30;
    const int TOTAL_ENEMY   = 7;
    const int EXPL_SPLIT    = 6;

    // Estados
    static const int ST_TITLE    = 1;
    static const int ST_WAVE     = 2;
    static const int ST_PLAYING  = 3;
    static const int ST_GAMEOVER = 4;
    static constexpr const char *STATES[5]  = {"INDEFINIDO", "TITLE", "WAVE", "PLAYING", "GAMEOVER"};

    void inimigoExplosao(Object o);
    void mudaEstado(int estado);
    void carregaRecursos();
    void criaObjetos(string_view qual);

    TargetsGame() = default;
    int run();
};
