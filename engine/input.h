#pragma once
#include <SDL2/SDL.h>
#include <vector>
#include <array>
#include <cstdint>

using namespace std;

class Input {
public:
    Input();
    ~Input();

    // Chamar 1x por frame, ANTES de ler estados
    void beginFrame();

    // --- Quit / janela ---
    bool quitRequested() const { return quitFlag; }

    // --- Teclado (usar SDL_Scancode, ex.: SDL_SCANCODE_SPACE) ---
    bool keyHeld(SDL_Scancode sc) const;
    bool keyPressed(SDL_Scancode sc) const;   // transição: foi pressionada neste frame
    bool keyReleased(SDL_Scancode sc) const;  // transição: foi liberada neste frame

    // --- Gamepads (SDL_GameController) ---
    // Abra pelo índice enumerado pelo SDL (0,1,2...)
    bool openGamepad(int index = 0);
    void closeGamepads();

    bool padHeld(SDL_GameControllerButton btn, int padIndex = 0) const;
    bool padPressed(SDL_GameControllerButton btn, int padIndex = 0) const;
    bool padReleased(SDL_GameControllerButton btn, int padIndex = 0) const;

    // Eixos no range [-32768, 32767]
    Sint16 padAxis(SDL_GameControllerAxis axis, int padIndex = 0) const;

private:
    // --- Teclado ---
    vector<Uint8> kb_now;
    vector<Uint8> kb_prev;

    // --- Gamepads ---
    struct Pad {
        SDL_GameController* gc = nullptr;
        array<Uint8, SDL_CONTROLLER_BUTTON_MAX> now{};
        array<Uint8, SDL_CONTROLLER_BUTTON_MAX> prev{};
        array<Sint16, SDL_CONTROLLER_AXIS_MAX>  axes{};
        bool connected = false;
    };
    vector<Pad> pads;

    // --- Eventos / janela ---
    bool quitFlag = false;
};
