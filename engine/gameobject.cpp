#include "gameobject.h"
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

    force_x = applyFriction(force_x, friction);
    force_y = applyFriction(force_y, friction);

    // Mudança de imagem
    image_index += image_speed;
    if (((int)image_index) >= images.size())
    {
        image_index = images.size() - 1;
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
