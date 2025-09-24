#pragma once

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

using namespace std;

struct Color {
    uint8_t r{255}, g{255}, b{255}, a{255}; // default branco
};

class Object
{
public:
    // Posicao e força
    float x, y;         // posicao na tela cano superior esquerdo = 0,0      
    float force_x;      // sera adicionado ao x
    float force_y;      // sera adicionado ao y
    float friction;     // sera diminuido das forças
    int w, h;           // tamanho do retangulo de colisao

    int  type;           // pode ser usado pra qualquer coisa
    int  depth;          // ordem que a imagem sera desenhada < mais na frente
    bool visible;       // mostrar ou não
    
    // Rotação
    float angle;        // angulo que a imagem será mostrad
    float angle_speed;  // soma ao angulo em cada step

    // Refencias de imagem 
    vector<string> images; // referencia de imagens assossiadas a esse objeto
    float image_index;  // imagem mostrada no momento de 0 a images.size()
    float image_speed;  // velocidade de transição para a proxima imagem

    string font_name;
    int    font_size;
    Color  font_color;
    string text;
      
    // eventos
    function<void(Object*)> onAnimationEnd;
    function<void(Object*)> onBeforeDraw;
    function<void(Object*)> onBeforeCalculate;
    function<void(Object*)> onAfterCalculate;

    ~Object();

    Object(int x, int y, int w, int h,  int type = 0, int depth = 0): x(x), y(y), w(w), h(h), type(type), depth(depth)
    {
        visible  = true;
        
        force_x  = 0;
        force_y  = 0;
        friction = 0;
        
        angle       = 0;
        angle_speed = 0;

        image_index = 0;
        image_speed = 0;
        font_color  = {255, 255, 255, 255};
    }

    Object(int x, int y, int w, int h, string image, int type = 0, int depth = 0):Object(x, y, w, h, type, depth)
    {
        addImageRef(image);
    }

    void addImageRef(string image);   
    void calculate();
    string getCurrentImageRef(); 
};
