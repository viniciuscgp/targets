#pragma once
#include "engine/engine.h"
#include <vector>
#include <string>
#include <unordered_map>

class TargetsGame {
public:
    Engine g;

    Object *title        = nullptr;
    Object *background   = nullptr;
    Object *history      = nullptr;
    Object *push         = nullptr;
    Object *nave         = nullptr;
    Object *gover        = nullptr;
    Object *hud_score    = nullptr;
    Object *hud_hi       = nullptr;
    Object *hud_wave     = nullptr;
    Object *hud_energy   = nullptr;
    Object *hud_debug    = nullptr;

    Object *display_wave = nullptr;

    int score  = 0;
    int hi     = 0;
    int lifes  = 2;
    int state  = 0;
    int wave   = 1;
    int kills  = 0;
    int energy = 0;
    int music_volume = 128 / 2; // 128 é o máximo

    void inimigoExplosao(Object o);
    void mudaEstado(int estado);
    void carregaRecursos();
    void criaObjetos(string_view qual);

    TargetsGame() = default;
    int run();
};
