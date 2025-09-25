#include "input.h"
#include <cstring> // memcpy
#include <algorithm>

Input::Input() {}
Input::~Input() { closeGamepads(); }

void Input::beginFrame()
{
    // 1) drena fila de eventos e captura quit
    quitFlag = false;
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) quitFlag = true;

        // (opcional) trate aqui DEVICEADDED/REMOVED hot-plug dos pads:
        // if (e.type == SDL_CONTROLLERDEVICEADDED)  openGamepad(e.cdevice.which);
        // if (e.type == SDL_CONTROLLERDEVICEREMOVED){ /* fechar correspondente */ }
    }

    kb_prev = kb_now;
    int nkeys = 0;
    const Uint8* state = SDL_GetKeyboardState(&nkeys);
    kb_now.resize(nkeys);
    std::memcpy(kb_now.data(), state, nkeys);

    // 3) Gamepads: snapshot de botões e eixos
    for (auto& p : pads) {
        if (!p.gc) continue;
        p.prev = p.now; // copy anterior
        for (int b = 0; b < SDL_CONTROLLER_BUTTON_MAX; ++b) {
            p.now[b] = SDL_GameControllerGetButton(p.gc, (SDL_GameControllerButton)b);
        }
        for (int a = 0; a < SDL_CONTROLLER_AXIS_MAX; ++a) {
            p.axes[a] = SDL_GameControllerGetAxis(p.gc, (SDL_GameControllerAxis)a);
        }
    }
}

// --- Teclado ---
bool Input::keyHeld(SDL_Scancode sc) const {
    return (int)sc < (int)kb_now.size() && kb_now[sc];
}
bool Input::keyPressed(SDL_Scancode sc) const {
    const bool now = (int)sc < (int)kb_now.size() && kb_now[sc];
    const bool was = (int)sc < (int)kb_prev.size() && kb_prev[sc];
    return now && !was;
}
bool Input::keyReleased(SDL_Scancode sc) const {
    const bool now = (int)sc < (int)kb_now.size() && kb_now[sc];
    const bool was = (int)sc < (int)kb_prev.size() && kb_prev[sc];
    return !now && was;
}

// --- Gamepad ---
bool Input::openGamepad(int index)
{
    if (!SDL_IsGameController(index)) return false;
    SDL_GameController* gc = SDL_GameControllerOpen(index);
    if (!gc) return false;

    if ((int)pads.size() <= index) pads.resize(index + 1);
    pads[index].gc = gc;
    pads[index].connected = true;
    pads[index].now.fill(0);
    pads[index].prev.fill(0);
    pads[index].axes.fill(0);
    return true;
}

void Input::closeGamepads()
{
    for (auto& p : pads) {
        if (p.gc) { SDL_GameControllerClose(p.gc); p.gc = nullptr; }
        p.connected = false;
        p.now.fill(0);
        p.prev.fill(0);
        p.axes.fill(0);
    }
}

bool Input::padHeld(SDL_GameControllerButton btn, int i) const {
    return i >= 0 && i < (int)pads.size() && pads[i].gc && pads[i].now[btn];
}
bool Input::padPressed(SDL_GameControllerButton btn, int i) const {
    return i >= 0 && i < (int)pads.size() && pads[i].gc &&
           pads[i].now[btn] && !pads[i].prev[btn];
}
bool Input::padReleased(SDL_GameControllerButton btn, int i) const {
    return i >= 0 && i < (int)pads.size() && pads[i].gc &&
           !pads[i].now[btn] && pads[i].prev[btn];
}
Sint16 Input::padAxis(SDL_GameControllerAxis axis, int i) const {
    return (i >= 0 && i < (int)pads.size() && pads[i].gc) ? pads[i].axes[axis] : 0;
}
