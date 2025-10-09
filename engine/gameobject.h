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

struct Alarm {
    int frames_backup; // para restartar
    int frames;   // tempo restante em frames/ticks
    int id;       // identificação
};

enum class FxBlend {
    Normal,   // alpha blend padrão
    Add,      // aditivo (neon/laser)
    Mod,      // multiplicar por cor (MOD)
    Mul,      // multiply (se SDL < 2.0.18, engine.cpp faz fallback pra MOD)
    Screen    // aproximação de "screen" via custom blend
};

struct FxParams {
    FxBlend   blend       = FxBlend::Normal;

    // 255 mantém a imagem original (sem “tint”)
    uint8_t   tint_r      = 255;
    uint8_t   tint_g      = 255;
    uint8_t   tint_b      = 255;

    uint8_t   alpha       = 255;            // 255 = opaco

    int       glowRadius  = 0;              // 0 = sem glow
    uint8_t   glow_r      = 0;
    uint8_t   glow_g      = 255;
    uint8_t   glow_b      = 255;
    uint8_t   glow_a      = 255;

    bool      centerRotate = true;          // rotacionar em torno do centro
};

enum ImageCycle { LOOP, ONCE };

class Engine;

class Object
{
private:
    float x, y;             // posicao na tela cano superior esquerdo = 0,0
    int   w, h;             // tamanho do retangulo de colisao
    float x_start;          // valor inicial de x
    float y_start;          // valor inicial de y
    float x_prev;           // x anterior
    float y_prev;           // y anterior
    float x_scale;          // escala do objeto horizontal
    float y_scale;          // escala do objeto vertical;

    // Vetor principal da força aplicada no objeto
    float force_x;           // sera adicionado ao x
    float force_y;           // sera adicionado ao y
    float force_friction;    // sera diminuido das forças
    float gravity;           // força gravitacional

    // Vetor de Inpulso é um vetor adicional que será adicionado ao movimento
    float impulse_x;         // vetor secundario de impulso
    float impulse_y;         // vetor secundario de impulso
    float impulse_friction;  // atrito do impulso

    // campos comuns em muitos games
    float energy;            // energia desse objeto
    float shield;            // escudo generico
    float attack;            // ao colidir vai remover de shield e depois de energy

    int   type;              // pode ser usado pra qualquer coisa
    int   tag;               // pode ser usado pra qualquer coisa

    bool wraph;              // auto wrap horizontal
    bool wrapv;              // auto wrap vertical;

    int depth;               // ordem que a imagem sera desenhada < mais na frente
    int collision_group;     // -1 nao colide, 0 colide com todos, >= colide com iguais

    bool defunct;            // sera eliminado
    bool visible;            // mostrar ou não
    Engine *engine;          // apontador pra engine
    Object *parent;          // objeto pai

    // Angulo que a imagem será mostrada e se vai girar automaticamente
    float angle;             // angulo que a imagem será mostrada
    float angle_speed;       // soma ao angulo em cada step

    // Imagens associadas ao objeto
    float image_index;       // imagem mostrada no momento de 0 a images.size()
    float image_speed;       // velocidade de transição para a proxima imagem
    bool centered;           // vai centralizar a imagem
    ImageCycle image_cycle;  // ao terminar o ciclo de imagens o que fazer

    // Texto associado ao objeto
    string font_name;        // Nome da fonte
    Color  font_color;       // cor da fonte
    int    font_size;        // tamanho da fonte
    string text;             // se definido será mostrado na fonte acima

    vector<Alarm> alarms;    // alarmes, ao finalizar, gera um evento

public:
    static constexpr Color COLOR_WHITE       = {255, 255, 255, 255};
    static constexpr Color COLOR_BLACK       = {0, 0, 0, 255};
    static constexpr Color COLOR_RED         = {255, 0, 0, 255};
    static constexpr Color COLOR_GREEN       = {0, 255, 0, 255};
    static constexpr Color COLOR_BLUE        = {0, 0, 255, 255};
    static constexpr Color COLOR_YELLOW      = {255, 255, 0, 255};
    static constexpr Color COLOR_CYAN        = {0, 255, 255, 255};
    static constexpr Color COLOR_MAGENTA     = {255, 0, 255, 255};
    static constexpr Color COLOR_GRAY        = {128, 128, 128, 255};
    static constexpr Color COLOR_TRANSPARENT = {0, 0, 0, 0};

    vector<string> images;   // referencias de imagens assossiadas a esse objeto

    // ---------- FX por-objeto (defaults neutros) ----------
    FxParams fx;

    // eventos
    function<void(Object *)> onAnimationEnd;
    function<void(Object *)> onBeforeDraw;
    function<void(Object *)> onAfterDraw;
    function<void(Object *)> onBeforeCalculate;
    function<void(Object *)> onAfterCalculate;
    function<void(Object *, int id)> onAlarmFinished;
    function<void(Object *, Object *)> onCollision;

    ~Object();

    Object(int x, int y, int w, int h, int type = 0, int depth = 0) : x(x), y(y), w(w), h(h), type(type), depth(depth)
    {
        visible = true;

        x_start = x;
        y_start = y;
        x_prev  = x;
        y_prev  = y;

        x_scale = 1.0f;
        y_scale = 1.0f;

        force_x = 0;
        force_y = 0;
        force_friction = 0;
        gravity = 0;

        impulse_x = 0;
        impulse_y = 0;
        impulse_friction = 0;

        energy = 10;
        attack = 10;
        shield = 0;
        collision_group = 0;

        wraph = false;
        wrapv = false;

        angle = 0;
        angle_speed = 0;

        image_index = 0;
        image_speed = 0;
        image_cycle = LOOP;
        centered    = true;

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
    float getImpulseDirection();

    void setAngle(float angle, float angle_speed);

    void requestDestroy();
    void applyImpact(Object *other);
    bool destroyIfLowEnergy();
    float getFinalDirection() const;
    string getCurrentImageRef();

    void setAlarm(int frames, int id);
    void finishAlarm(int id);
    void restartAlarm(int id);

    Color withAlpha(const Color& base, uint8_t alpha);
    Object *getParent() const;
    void setParent(Object *p);

    int getW() const;
    int getH() const;

    float getX() const;
    void setX(float x);
    void addX(float x);
    
    float getY() const;
    void setY(float y);
    void addY(float y);
    
    void centerY();
    void centerX();
    void center();

    float getXPrev() const;
    float getYPrev() const;
    float getXStart() const;
    float getYStart() const;

    float getXScale() const;
    float getYScale() const;

    float getDirection() const;
    float getForce() const;

    float getForceX() const;
    void  setForceX(float force);

    float getForceY() const;
    void  setForceY(float force);
    float getForceFriction() const;

    float getGravity() const;
    void  setGravity(float g);

    float getImpulseDirection() const;
    float getImpulseForce() const;
    
    float getImpulseX() const;
    float getImpulseY() const;

    float getImpulseFriction() const;
    void  setImpulseFriction(float impulseFriction);

    float getEnergy() const;
    void  setEnergy(float energy);

    float getShield() const;
    void  setShield(float shield);

    float getAtack() const;
    void  setAtack(float atack);

    int  getType() const;
    void setType(int type);

    int  getTag() const;
    void setTag(int tag);

    bool getWrapH() const;
    bool getWrapV() const;

    int  getDepth() const;
    void setDepth(int depth);


    int  getCollisionGroup() const;
    void setCollisionGroup(int collisionGroup);

    bool isDefunct() const;
    void setDefunct(bool b);

    bool isVisible() const;
    void setVisible(bool visible);

    float getAngle() const;
    void  setAngle(float angle);

    float getAngleSpeed() const;
    void  setAngleSpeed(float angleSpeed);

    float getImageIndex() const;
    float getImageSpeed() const;
    void  setImageSpeed(float speed);

    ImageCycle getImageCycle() const;
    void setImageCycle(ImageCycle imageCycle);

    bool isCentered() const;
    void setCentered(bool centered);

    string getFontName() const;
    Color getFontColor() const;
    int getFontSize() const;

    string getText() const;
    void setText(string text);

    Engine* getEngine() const;
    void setEngine(Engine *engine);

    FxParams getFx() const;    

    // -----------------------
    // Helpers de FX (presets)
    // -----------------------

    void clearFx() { fx = FxParams{}; }

    void setTint(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255, FxBlend blend = FxBlend::Normal) {
        fx.blend  = blend;
        fx.tint_r = r; 
        fx.tint_g = g; 
        fx.tint_b = b;
        fx.alpha  = a;
    }

    void setGlow(uint8_t r, uint8_t g, uint8_t b, int radius, uint8_t glowAlpha = 255) {
        fx.blend       = FxBlend::Normal;
        fx.tint_r      = 255; 
        fx.tint_g      = 255; 
        fx.tint_b      = 255;
        fx.alpha       = 255;

        fx.glowRadius  = radius;
        fx.glow_r      = r;
        fx.glow_g      = g;
        fx.glow_b      = b;
        fx.glow_a      = glowAlpha;
    }

    void setNeon(uint8_t r, uint8_t g, uint8_t b, int radius = 4, uint8_t alpha = 200) {
        fx.blend  = FxBlend::Add;
        fx.tint_r = r; 
        fx.tint_g = g; 
        fx.tint_b = b;
        fx.alpha  = alpha;

        fx.glowRadius = radius;
        fx.glow_r = r; 
        fx.glow_g = g; 
        fx.glow_b = b; 
        fx.glow_a = alpha;
    }

    void setScreen(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        fx.blend  = FxBlend::Screen;
        fx.tint_r = r; 
        fx.tint_g = g; 
        fx.tint_b = b;
        fx.alpha  = a;
        fx.glowRadius = 0;
    }

    void setMultiply(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
        fx.blend  = FxBlend::Mul;
        fx.tint_r = r; 
        fx.tint_g = g; 
        fx.tint_b = b;
        fx.alpha  = a;
        fx.glowRadius = 0;
    }    

    // ======================================================
    // Helpers para encadear alarm sem perder handler atual
    // ======================================================
    void chainAlarmHandler(int watchId, std::function<void(Object*)> fn) {
        auto prev = onAlarmFinished;
        onAlarmFinished = [prev, watchId, fn](Object* o, int id){
            if (prev) prev(o, id);     // preserva handler antigo
            if (id == watchId) fn(o);  // executa ação nova
        };
    }

    // ============================
    // FLASH HELPERS (autoclear FX)
    // ============================

    // Neon temporário: aplica e limpa após 'frames'
    void flashNeon(uint8_t r, uint8_t g, uint8_t b, int radius, uint8_t alpha,
                   int frames, int alarmId = 9001) {
        setNeon(r, g, b, radius, alpha);
        setAlarm(frames, alarmId);
        chainAlarmHandler(alarmId, [](Object* o){ o->clearFx(); });
    }

    // Glow temporário
    void flashGlow(uint8_t r, uint8_t g, uint8_t b, int radius, uint8_t glowAlpha,
                   int frames, int alarmId = 9002) {
        setGlow(r, g, b, radius, glowAlpha);
        setAlarm(frames, alarmId);
        chainAlarmHandler(alarmId, [](Object* o){ o->clearFx(); });
    }

    // Tint temporário (com blend à escolha)
    void flashTint(uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                   FxBlend blend, int frames, int alarmId = 9003) {
        setTint(r, g, b, a, blend);
        setAlarm(frames, alarmId);
        chainAlarmHandler(alarmId, [](Object* o){ o->clearFx(); });
    }

    // Screen temporário (clareia suave)
    void flashScreen(uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                     int frames, int alarmId = 9004) {
        setScreen(r, g, b, a);
        setAlarm(frames, alarmId);
        chainAlarmHandler(alarmId, [](Object* o){ o->clearFx(); });
    }

    // Multiply temporário (escurece por cor)
    void flashMultiply(uint8_t r, uint8_t g, uint8_t b, uint8_t a,
                       int frames, int alarmId = 9005) {
        setMultiply(r, g, b, a);
        setAlarm(frames, alarmId);
        chainAlarmHandler(alarmId, [](Object* o){ o->clearFx(); });
    }
};

