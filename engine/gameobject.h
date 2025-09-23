#include <string>

class GameObject
{
public:
    float x, y;
    float force_x, force_y;
    float friction;
    int w, h;
    std::string texture;
    int type;
    int depth;
    bool visible;

    GameObject(int x, int y, int w, int h, std::string texture, int type = 0, int depth = 0)
        : x(x), y(y), w(w), h(h), texture(texture), type(type), depth(depth), visible(true)
    {
        force_x = 0;
        force_y = 0;
        friction = 0;
    }

    void calculate();
};
