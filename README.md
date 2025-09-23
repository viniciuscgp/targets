# 🎯 The Targets

Um jogo simples estilo *space shooter*, feito em **C++** com **SDL2**, para fins de aprendizado e diversão. A engine do jogo é independente e pode ser reutilizada em outros projetos.

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

## 🛠️ Build e Instalação

**Pré-requisitos**
- CMake 3.10 ou superior
- Compilador C++ com suporte a C++17
- SDL2, SDL2_image e SDL2_mixer

---

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

