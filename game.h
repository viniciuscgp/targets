#pragma once
#include "engine.h"
#include <vector>
#include <string>

class TargetsGame {
private:
    Engine g;

    GameObject* title = nullptr;
    GameObject* push = nullptr;
    GameObject* nave = nullptr;
    GameObject* over = nullptr;

    std::vector<GameObject*> nave_tiros;
    std::vector<GameObject*> inimigos;
    std::vector<GameObject*> estrelas;
    std::vector<int> inimigos_texturas;

    int explosao_sn = -1;
    int tiro_tx     = -1;
    int tiro_sn     = -1;
    int score       =  0;
    int lifes       =  2;

    int state = 0;
    const int MAX_NAVE_TIRO = 4;
    const int TOTAL_ESTRELA = 30;

    // Estados
    const int STATE_TITLE    = 0;
    const int STATE_PLAYING  = 1;
    const int STATE_GAMEOVER = 2;

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

public:
    TargetsGame() = default;
    int run();
};
