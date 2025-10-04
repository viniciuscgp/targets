# 🎯 The Targets

Um jogo simples estilo *space shooter*, feito em **C++** com **SDL2**, para fins de aprendizado e diversão. A engine do jogo é independente e pode ser reutilizada em outros projetos.

# 👽 Plot

No ano de 2147, a Terra é atacada por uma frota de UFOs bizarros: cada nave inimiga tem a forma da própria cara dos alienígenas que a pilotam, como se fossem monstros voadores atravessando o espaço.

Esses seres, conhecidos como Morkons, querem transformar a Terra em sua nova colônia, drenando tudo o que encontram pelo caminho.

Você é o piloto da última nave de defesa da humanidade.
Sua missão é clara: atravessar enxames de UFOs-caras, enfrentar os líderes alienígenas e impedir a invasão antes que seja tarde demais.

A nave utiliza muita energia, por isso você vai precisar recolher toda energia que puder pra se manter operando. Boa sorte !

## 🎮 Sobre o Jogo

Você controla uma nave espacial que deve destruir inimigos enquanto desvia deles.  
**Características:**
- Tela de título com "Push Space Key"
- Nave controlável pelo teclado
- Estrelas no fundo se movendo
- Inimigos com padrões diferentes de movimento
- Tiros da nave (limitados)
- Efeitos sonoros de disparo e explosão
- Tela de **Game Over**

## ⌨️ Controles

- **Setas direcionais** → Mover a nave  
- **Espaço** → Atirar / Iniciar jogo  
- **Esc** → Sair do jogo  

## 📦 Resources
- Sounds: https://freesound.org/
- Imagens: https://chatgpt.com/

## 🛠️ Build e Instalação

**Pré-requisitos**
- CMake 3.10 ou superior
- Compilador C++ com suporte a C++17
- SDL2, SDL2_image e SDL2_mixer

### **🐧 Linux (Ubuntu/Debian)**

```bash
# Instalar dependências
sudo apt-get update
sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev cmake build-essential

# Clonar e compilar o projeto
git clone [seu-repositorio]
cd the-targets
mkdir build && cd build
cmake ..
make
```

## Executar
```bash
./targets
```

## 🪟 Windows

### Instalar vcpkg
```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\\bootstrap-vcpkg.bat

# Instalar dependências
.\\vcpkg install sdl2 sdl2-image sdl2-mixer

# Compilar o projeto
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[caminho-para-vcpkg]\\scripts\\buildsystems\\vcpkg.cmake ..
cmake --build .
```
## Executar
```bash
targets.exe
```

### 🍎 macOS

## Instalar dependências via Homebrew
```bash
brew install sdl2 sdl2_image sdl2_mixer cmake
```
## Compilar o projeto
```bash
mkdir build && cd build
cmake ..
make
```

# Executar
```bash
./targets 
```

## 📝 Licença
Este projeto é open-source e está sob a licença MIT.

## 🤝 Contribuições
Contribuições são bem-vindas! Sinta-se à vontade para:

Reportar bugs

Sugerir novas funcionalidades

Enviar pull requests

Prompt da musica:
-----------------
Aliens are invading the earth. You are the last earth defence with your ship. 
Looped music, short, less than 1 minute, medium velocity.