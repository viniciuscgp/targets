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

    y += y_force;
    x += x_force;

    x_force = applyFriction(x_force, friction);
    y_force = applyFriction(y_force, friction);

    y = applyGravity(y, gravity);

    // wrap horizontal
    if (wraph)
    {
        if (x + w > engine->getW()) x = -w;
        if (x < -w) x = engine->getW();
    }
    //wrap vertical
    if (wrapv)
    {
        if (y + h > engine->getH()) y = -h;
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
    angle += angle_speed;
    if (angle > 360)
    {
        angle = 0;
    }

    // Alarmes
    for (int i = alarms.size() - 1; i >= 0; --i)
    {
        if (--alarms[i] <= 0)
        {
            if (onAlarmFinished)
                onAlarmFinished(this, i);
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

bool Object::destroyIfLow()
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

void Object::alarmSet(int frames)
{
    alarms.push_back(frames);
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

void Object::requestDestroy()
{
    engine->requestDestroy(this);
}
