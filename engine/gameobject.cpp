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

void Object::calculate()
{
    if (onBeforeCalculate)
    {
        onBeforeCalculate(this);
    }

    y += force_y;
    x += force_x;

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

    force_x = applyFriction(force_x, friction);
    force_y = applyFriction(force_y, friction);

    // Mudança de imagem
    image_index += image_speed;
    if (((int)image_index) >= images.size())
    {
        if (image_cicle == ONCE)
            image_index = images.size() - 1;

        if (image_cicle == LOOP)
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
    force_x = fx;
    force_y = fy;
}

void Object::requestDestroy()
{
    engine->requestDestroy(this);
}
