#include "gameobject.h"

void GameObject::calculate()
{
    y += force_y;
    x += force_x;
}