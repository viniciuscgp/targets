#pragma once

#include <string>
#include <vector>

using namespace std;

class Object
{
public:
    float x, y;
    float force_x, force_y;
    float friction;
    int w, h;
    int type;
    int depth;
    bool visible;
    float image_index;
    float image_speed;
    vector<string> images;

    ~Object();

    Object(int x, int y, int w, int h,  int type = 0, int depth = 0): x(x), y(y), w(w), h(h), type(type), depth(depth)
    {
        visible = true;
        force_x = 0;
        force_y = 0;
        friction = 0;
        image_index = 0;
        image_speed = 0;
    }

    Object(int x, int y, int w, int h, string image, int type = 0, int depth = 0):Object(x, y, w, h, type, depth)
    {
        addImageRef(image);
    }

    void addImageRef(string image);   
    void calculate();
    string getCurrentImageRef(); 
};
