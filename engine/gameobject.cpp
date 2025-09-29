#include "gameobject.h"
#include "engine.h"
#include <iostream>
#include <cmath>

inline float applyFriction(float f, float fric)
{
    if (fric <= 0.0f || f == 0.0f)
        return f;
    float mag = std::max(0.0f, std::fabs(f) - fric);
    return std::copysign(mag, f);
}

inline float applyGravity(float f, float gravity)
{
    return f + gravity;
}

void Object::calculate()
{
    if (onBeforeCalculate)
    {
        onBeforeCalculate(this);
    }
    x_prev = x;
    y_prev = y;

    //-----------------------------------------------------

    y += y_force;
    x += x_force;

    x_force = applyFriction(x_force, friction_force);
    y_force = applyFriction(y_force, friction_force);

    //-----------------------------------------------------

    y += y_impulse;
    x += x_impulse;

    x_impulse = applyFriction(x_impulse, friction_impulse);
    y_impulse = applyFriction(y_impulse, friction_impulse);

    //------------------------------------------------------

    y = applyGravity(y, gravity);

    // wrap horizontal
    if (wraph)
    {
        if (x > engine->getW()) x = -getW();
        if (x < -w) x = engine->getW();
    }
    //wrap vertical
    if (wrapv)
    {
        if (y > engine->getH()) y = -getH();
        if (y < -h) y = engine->getH();
    }

    // Mudança de imagem
    image_index += image_speed;
    if (((int)image_index) >= images.size())
    {
        if (image_cycle == ONCE)
            image_index = images.size() - 1;

        if (image_cycle == LOOP)
            image_index = 0;
        
        if (onAnimationEnd)
        {
            onAnimationEnd(this);
        }
    }

    // Calculo do angulo

    angle = fmod(angle + angle_speed, 360.0f);
    if (angle < 0) angle += 360.0f;    

    // Alarmes
    for (int i = alarms.size() - 1; i >= 0; --i)
    {
        if (--alarms[i].frames <= 0)
        {
            if (onAlarmFinished)
                onAlarmFinished(this, alarms[i].id); 
            alarms.erase(alarms.begin() + i);
        }
    }

    if (onAfterCalculate)
    {
        onAfterCalculate(this);
    }
}

void Object::applyImpact(Object *other)
{
    // energy = 100 shield = 50 atack = 60
    shield -= other->atack;
    if (shield < 0) energy -= (shield * -1);
}

bool Object::destroyIfLowEnergy()
{
    if (energy <= 0) {
        requestDestroy();
        return true;
    }
    return false;
}

Object::~Object()
{
}

void Object::addImageRef(string image)
{
    images.push_back(image);
}

string Object::getCurrentImageRef()
{
    if (images.size() == 0)
        return "";
    return images[(int)image_index];
}

void Object::setAlarm(int frames, int id)
{
    alarms.push_back({frames, id});
}

void Object::setFont(string name, int size, Color color)
{
    font_name = "assets/fonts/" + name;
    font_size = size;
    font_color = color;
}

void Object::setWrap(bool h, bool v)
{
    wraph = h;
    wrapv = v;
}

void Object::setForce(float fx, float fy)
{
    x_force = fx;
    y_force = fy;
}

void Object::setDirection(float dir, float f)
{
    // converter graus → radianos
    float rad = dir * (M_PI / 180.0f);

    // eixo X cresce para a direita normalmente
    x_force = std::cos(rad) * f;

    // eixo Y cresce para baixo, mas 90° deve ser para cima,
    // então precisamos inverter o sinal
    y_force = -std::sin(rad) * f;    
}

void Object::setImpulse(float fx, float fy)
{
    x_impulse = fx;
    y_impulse = fy;
}

void Object::setImpulseDirection(float dir, float f)
{
    // converter graus → radianos
    float rad = dir * (M_PI / 180.0f);

    // eixo X cresce para a direita
    x_impulse = std::cos(rad) * f;

    // eixo Y cresce para baixo, mas 90° deve ser para cima
    y_impulse = -std::sin(rad) * f;    
}

void Object::setScale(float sx, float sy)
{
    x_scale = sx;
    y_scale = sy;
}
void Object::setScale(float s)
{
    x_scale = s;
    y_scale = s;
}


void Object::requestDestroy()
{
    engine->requestDestroy(this);
}


float Object::getFinalDirection() const
{
    // vetor resultante = força + impulso
    float vx = x_force + x_impulse;
    float vy = y_force + y_impulse;

    // se não houver movimento, retorna o direction atual
    if (vx == 0.0f && vy == 0.0f)
        return direction; 

    // atan2 retorna radianos no sistema matemático
    float rad = std::atan2(-vy, vx); // invertido porque y cresce para baixo

    // converter para graus
    float deg = rad * (180.0f / M_PI);

    // normalizar para 0–360
    if (deg < 0) deg += 360.0f;

    return deg;
}

Color Object::withAlpha(const Color& base, uint8_t alpha) 
{
    return Color{ base.r, base.g, base.b, alpha };
}


// getters
int Object::getW() const {return w * x_scale;}
int Object::getH() const {return h * y_scale;}



