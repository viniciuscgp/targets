#pragma once
#include "engine.h"
#include <vector>
#include <string>
#include <unordered_map>

class TargetsGame {
private:
    Engine g;

    GameObject* title = nullptr;
    GameObject* push  = nullptr;
    GameObject* nave  = nullptr;
    GameObject* gover = nullptr;

    std::vector<GameObject*> nave_tiros;
    std::vector<GameObject*> inimigos;
    std::vector<GameObject*> estrelas;

    int score  = 0;
    int lifes  = 2;
    int state  = 0;

    const int MAX_SHIP_FIRE = 4;
    const int TOTAL_STARS   = 30;
    const int TOTAL_ENEMY   = 7;

    // Estados
    const int ST_TITLE    = 0;
    const int ST_PLAYING  = 1;
    const int ST_GAMEOVER = 2;

    // Tipos
    const int T_NAVE      = 0;
    const int T_INIMIGO_1 = 1;
    const int T_INIMIGO_2 = 2;
    const int T_INIMIGO_3 = 3;
    const int T_ESTRELA   = 100;
    const int T_TIRO      = 200;

    void controlaJogador(GameObject* jogador);
    void controlaTirosJogador();
    void controlaInimigos();
    void controlaEstrelas();
    void criaInimigos();
    void mudaEstado(int estado);
    void carregaRecursos();

public:
    TargetsGame() = default;
    int run();
};
