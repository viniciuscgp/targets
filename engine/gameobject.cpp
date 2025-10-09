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
    force_y += gravity;

    y += force_y;
    x += force_x;

    force_x = applyFriction(force_x, force_friction);
    force_y = applyFriction(force_y, force_friction);

    //-----------------------------------------------------

    y += impulse_y;
    x += impulse_x;

    impulse_x = applyFriction(impulse_x, force_friction);
    impulse_y = applyFriction(impulse_y, force_friction);

    //------------------------------------------------------

    if (wraph)
    {
        if (x > engine->getW()) x = -getW();
        if (x < -w) x = engine->getW();
    }

    if (wrapv)
    {
        if (y > engine->getH()) y = -getH();
        if (y < -h) y = engine->getH();
    }

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
    shield -= other->attack;
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
    alarms.push_back({frames, frames, id});
}

void Object::finishAlarm(int id)
{
    for (auto it = alarms.begin(); it != alarms.end(); ++it)
    {
        if (it->id == id)
        {
             it->frames = 0;
             break;
        }
    }
}

void Object::restartAlarm(int id)
{
    for (auto it = alarms.begin(); it != alarms.end(); ++it)
    {
        if (it->id == id)
        {
             it->frames = it->frames_backup;
             break;
        }
    }
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

void Object::setDirection(float dir, float f) 
{
    float rad = dir * (M_PI / 180.0f);

    force_x =  cos(rad) * f;
    force_y = -sin(rad) * f;    
}

float Object::getDirection() const 
{ 
    float direction = atan2(force_y, force_x) * 180.0f / M_PI;
    if (direction < 0) direction += 360.0f;
    return direction;
}

void Object::setImpulse(float fx, float fy)
{
    impulse_x = fx;
    impulse_y = fy;
}

// Seta a direacao e a forca do vetor de impulso
void Object::setImpulseDirection(float dir, float f) 
{
   float rad = dir * (M_PI / 180.0f);

   impulse_x = cos(rad) * f;
   impulse_y = -sin(rad) * f;    
}

float Object::getImpulseDirection() 
{
    float direction = atan2(impulse_y, impulse_x) * 180.0f / M_PI;
    if (direction < 0) direction += 360.0f;
    return direction;
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

void Object::setAngle(float angle, float angle_speed)
{
    this->angle = angle;
    this->angle_speed = angle_speed;
}

void Object::requestDestroy()
{
    engine->requestDestroy(this);
}

float Object::getFinalDirection() const
{
    // vetor resultante = força + impulso
    float vx = force_x + impulse_x;
    float vy = force_y + impulse_y;

    // se não houver movimento, retorna o direction atual
    if (vx == 0.0f && vy == 0.0f)
        return 0;

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

Object *Object::getParent() const {
    return this->parent;
}

void Object::setParent(Object *p) {
    this->parent = p;
}


float Object::getX() const { return x; }
void Object::setX(float x) { this->x = x; }
void Object::addX(float x) { this->x = this->x + x; }
void Object::centerX() { this->engine->centerXObject(this); }

float Object::getY() const { return y; }
void Object::setY(float y) {this->y = y; } 
void Object::addY(float y) {this->y = this->y + y; } 
void Object::centerY() { this->engine->centerYObject(this); }

void Object::center() { this->engine->centerObject(this); }

int Object::getW() const { return w * x_scale; }
int Object::getH() const { return h * y_scale; }

float Object::getXPrev() const { return x_prev; }
float Object::getYPrev() const { return y_prev; }
float Object::getXStart() const { return x_start; }
float Object::getYStart() const { return y_start; }

float Object::getXScale() const { return x_scale; }
float Object::getYScale() const { return y_scale; }

float Object::getForceX() const { return force_x; }
void  Object::setForceX(float force) { this->force_x = force; }

float Object::getForceY() const { return force_y; }
void  Object::setForceY(float force) { this->force_y = force; }

float Object::getForceFriction() const { return force_friction; }
float Object::getGravity() const { return gravity; }
void  Object::setGravity(float g) { this->gravity = g; }

float Object::getImpulseX() const { return impulse_x; }
float Object::getImpulseY() const { return impulse_y; }

float Object::getImpulseFriction() const { return impulse_friction; }
void Object::setImpulseFriction(float impulseFriction) { this->impulse_friction = impulseFriction; }

float Object::getEnergy() const { return energy; }
void Object::setEnergy(float energy) { this->energy = energy; }

float Object::getShield() const { return shield; }
void Object::setShield(float shield) { this->shield = shield; }

float Object::getAtack() const { return attack; }
void Object::setAtack(float atack) { this->attack = atack; }

int Object::getType() const { return type; }
void Object::setType(int type) { this->type = type; }

int Object::getTag() const { return tag; }
void Object::setTag(int tag) { this->tag = tag; }

bool Object::getWrapH() const { return wraph; }
bool Object::getWrapV() const { return wrapv; }

int Object::getDepth() const { return depth; }
void Object::setDepth(int depth) { this->depth = depth; }

int Object::getCollisionGroup() const { return collision_group; }
void Object::setCollisionGroup(int collisionGroup) { this->collision_group = collision_group; }

bool Object::isDefunct() const { return defunct; }
void Object::setDefunct(bool b) { this->defunct = b; }

bool Object::isVisible() const { return visible; }
void Object::setVisible(bool visible) { this->visible = visible; }

float Object::getAngle() const { return angle; }
void Object::setAngle(float angle) { this->angle = angle; }

float Object::getAngleSpeed() const { return angle_speed; }
void  Object::setAngleSpeed(float angleSpeed) { this->angle_speed = angleSpeed; }

float Object::getImageIndex() const { return image_index; }
float Object::getImageSpeed() const { return image_speed; }
void Object::setImageSpeed(float speed) {this->image_speed = speed;}

ImageCycle Object::getImageCycle() const { return image_cycle; }
void Object::setImageCycle(ImageCycle imageCycle) {this->image_cycle = imageCycle; }

bool Object::isCentered() const { return centered; }
void Object::setCentered(bool centered) { this->centered = centered; }

string Object::getFontName() const { return font_name; }
Color Object::getFontColor() const { return font_color; }
int Object::getFontSize() const { return font_size; }

string Object::getText() const { return text; }
void Object::setText(string text) { this->text = text; }


Engine* Object::getEngine() const { return engine; }
void Object::setEngine(Engine *engine) { this->engine = engine;}

FxParams Object::getFx() const { return fx; }
