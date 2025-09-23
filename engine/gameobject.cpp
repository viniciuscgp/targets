#include "gameobject.h"
#include <iostream>

void Object::calculate()
{
    y += force_y;
    x += force_x;

    image_index += image_speed;
    if ((int) image_index >= images.size()) 
        image_index = images.size() - 1;
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
    if (images.size() == 0) return "";
    return images[image_index];
}
