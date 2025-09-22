# 🎯 The Targets

Um jogo simples estilo *space shooter*, feito em **C++** com **SDL2**, para fins de aprendizado e diversão.
Esse projeto pode ser usado como base para criação de outros joguinhos do tipo, já que a classe Engine é independente de projeto. Inclusive a classe TargetsGame, que base pra esse game, poderá ser adaptada facilmente.

## 🚀 Sobre o jogo (em construção)

Você controla uma nave espacial que deve destruir inimigos enquanto desvia deles.  
O jogo possui:

- Tela de título com "Push Space Key"
- Nave controlável pelo teclado
- Estrelas no fundo se movendo
- Inimigos com padrões diferentes de movimento
- Tiros da nave (limitados)
- Efeitos sonoros de disparo e explosão
- Tela de **Game Over** (em desenvolvimento)

## 🎮 Controles

- **Setas direcionais** → mover a nave  
- **Espaço** → atirar / iniciar o jogo  
- **Esc** → sair do jogo  

## 📂 Estrutura do projeto

targets/
│── assets/ # Imagens e sons do jogo
│ ├── nave.png
│ ├── nave_tiro.png
│ ├── alien_1.png ... alien_7.png
│ ├── estrela.png
│ ├── title.png
│ ├── push_space_key.png
│ ├── game_over.png
│ ├── nave_tiro.wav
│ └── inimigo_explode.wav
│
│── src/
│ ├── main.cpp # Ponto de entrada
│ ├── game.h / game.cpp # Lógica principal do jogo
│ ├── engine.h / engine.cpp # Engine simples (SDL wrapper)
│
│── CMakeLists.txt
│── README.md



## 🔧 Dependências

- [SDL2](https://www.libsdl.org/)  
- [SDL2_image](https://www.libsdl.org/projects/SDL_image/)  
- [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)  

No Ubuntu/Debian/Mint, instale com:

```bash
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
