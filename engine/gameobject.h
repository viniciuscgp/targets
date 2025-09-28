#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

using namespace std;

struct Color
{
    uint8_t r{255}, g{255}, b{255}, a{255}; // default branco
};

class Engine;

class Object
{
private:
    int w, h; // tamanho do retangulo de colisao

public:
    static constexpr Color COLOR_WHITE = {255, 255, 255, 255};
    static constexpr Color COLOR_BLACK = {0, 0, 0, 255};
    static constexpr Color COLOR_RED = {255, 0, 0, 255};
    static constexpr Color COLOR_GREEN = {0, 255, 0, 255};
    static constexpr Color COLOR_BLUE = {0, 0, 255, 255};
    static constexpr Color COLOR_YELLOW = {255, 255, 0, 255};
    static constexpr Color COLOR_CYAN = {0, 255, 255, 255};
    static constexpr Color COLOR_MAGENTA = {255, 0, 255, 255};
    static constexpr Color COLOR_GRAY = {128, 128, 128, 255};
    static constexpr Color COLOR_TRANSPARENT = {0, 0, 0, 0};

    enum ImageCycle
    {
        LOOP,
        ONCE
    };

    float x, y;    // posicao na tela cano superior esquerdo = 0,0
    float x_start; // valor inicial de x
    float y_start; // valor inicial de y
    float x_prev;  // x anterior
    float y_prev;  // y anterior
    float x_scale; // escala do objeto horizontal
    float y_scale; // escala do objeto vertical;

    float direction; // direcao em graus
    float force;     // força
    float x_force;   // sera adicionado ao x
    float y_force;   // sera adicionado ao y

    float friction_force; // sera diminuido das forças
    float gravity;        // força gravitacional

    float impulse_direction; // direcao em graus do impulso
    float impulse_force;     // forca de impulso
    float x_impulse;         // vetor secundario de impulso
    float y_impulse;         // vetor secundario de impulso
    float friction_impulse;  // atrito do impulso

    float energy; // energia desse objeto
    float shield; // escudo generico
    float atack;  // ao colidir vai remover de shield e depois de energy

    bool wraph; // auto wrap horizontal
    bool wrapv; // auto wrap vertical;

    int type;            // pode ser usado pra qualquer coisa
    int tag;             // pode ser usado pra qualquer coisa
    int depth;           // ordem que a imagem sera desenhada < mais na frente
    int collision_group; // -1 nao colide, 0 colide com todos, >= colide com iguais

    bool defunct; 
    bool visible;   // mostrar ou não
    Engine *engine; // apontador pra engine

    float angle;       // angulo que a imagem será mostrad
    float angle_speed; // soma ao angulo em cada step

    vector<string> images;  // referencia de imagens assossiadas a esse objeto
    float image_index;      // imagem mostrada no momento de 0 a images.size()
    float image_speed;      // velocidade de transição para a proxima imagem
    ImageCycle image_cycle; // ao terminar o ciclo de imagens o que fazer

    string font_name; // Nome da fonte
    Color font_color; // cor da fonte
    int font_size;    // tamanho da fonte
    string text;      // se definido será mostrado na fonte acima

    vector<int> alarms; // alarmes, ao finalizar, gera um evento

    // eventos
    function<void(Object *)> onAnimationEnd;
    function<void(Object *)> onBeforeDraw;
    function<void(Object *)> onBeforeCalculate;
    function<void(Object *)> onAfterCalculate;
    function<void(Object *, int index)> onAlarmFinished;
    function<void(Object *, Object *)> onCollision;

    ~Object();

    Object(int x, int y, int w, int h, int type = 0, int depth = 0) : x(x), y(y), w(w), h(h), type(type), depth(depth)
    {
        visible = true;

        x_start = x;
        y_start = y;
        x_prev = x;
        y_prev = y;

        x_scale = 1.0f;
        y_scale = 1.0f;

        x_force = 0;
        y_force = 0;
        friction_force = 0;
        gravity = 0;

        x_impulse = 0;
        y_impulse = 0;
        friction_impulse = 0;

        energy = 10;
        shield = 0;
        atack = 10;
        collision_group = 0;

        wraph = false;
        wrapv = false;

        angle = 0;
        angle_speed = 0;

        image_index = 0;
        image_speed = 0;
        image_cycle = LOOP;
        font_color = {255, 255, 255, 255};
        defunct = false;
    }

    Object(int x, int y, int w, int h, string image, int type = 0, int depth = 0) : Object(x, y, w, h, type, depth)
    {
        addImageRef(image);
    }
    void setScale(float sx, float sy);
    void setScale(float s);

    void addImageRef(string image);
    void calculate();
    void setFont(string name, int size, Color color);
    void setWrap(bool h, bool v);

    void setForce(float fx, float fy);
    void setDirection(float dir, float f);

    void setImpulse(float ix, float iy);
    void setImpulseDirection(float dir, float f);

    void requestDestroy();
    void applyImpact(Object *other);
    bool destroyIfLowEnergy();
    float getFinalDirection() const;
    string getCurrentImageRef();
    void setAlarm(int frames);
    Color withAlpha(const Color& base, uint8_t alpha);

    // getters
    int getW() const;
    int getH() const;
};
