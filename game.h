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
    Object* hud_score = nullptr;
    Object* hud_hi    = nullptr;
    Object* display_wave = nullptr;

    int score  = 0;
    int hi     = 0;
    int lifes  = 2;
    int state  = 0;
    int wave   = 1;
    int kills  = 0;

    void inimigoExplosao(Object o);
    void mudaEstado(int estado);
    void carregaRecursos();
    void criaObjetos(string_view qual);

    TargetsGame() = default;
    int run();
};
