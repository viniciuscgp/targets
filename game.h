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

    vector<Object*> nave_tiros;
    vector<Object*> inimigos;
    vector<Object*> estrelas;

    int score  = 0;
    int lifes  = 2;
    int state  = 0;
    int wave   = 1;

    const int MAX_SHIP_FIRE = 4;
    const int TOTAL_STARS   = 30;
    const int TOTAL_ENEMY   = 7;
    const int EXPL_SPLIT    = 6;

    // Estados
    const int ST_TITLE    = 0;
    const int ST_WAVE     = 1;
    const int ST_PLAYING  = 2;
    const int ST_GAMEOVER = 3;

    // Tipos
    const int T_INIMIGO_1 = 1;
    const int T_INIMIGO_2 = 2;
    const int T_INIMIGO_3 = 3;

    void controlaJogador(Object* jogador);
    void controlaTirosJogador();
    void inimigoExplosao(Object o);
    void controlaInimigos();
    void controlaEstrelas();
    void criaInimigos();
    void mudaEstado(int estado);
    void carregaRecursos();

    TargetsGame() = default;
    int run();
};
