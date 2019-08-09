#include <iostream>
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

namespace drawing{
    SDL_Renderer* renderer;
    SDL_Window* window;

    void clear(){
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
    }

    void present(){
        SDL_RenderPresent(renderer);
    }
}

class vec2 {
public:
    float x = 0.f;
    float y = 0.f;
};

class drawable {
public:
    vec2 position;
    vec2 size; 

    virtual void draw(){}
protected:
    const SDL_FRect get_destination(){
        int winW, winH;
        SDL_GetRendererOutputSize(drawing::renderer, &winW, &winH);

        SDL_FRect destination;
        destination.w = size.x;
        destination.h = size.y;
        destination.x =  position.x + winW / 2 - destination.w / 2;
        destination.y = -position.y + winH / 2 - destination.h / 2;

        return destination;
    }
};

class square : public drawable{
public:
    SDL_Color color = { 0, 0, 0, 255 };

    void draw() override {
        auto destination = get_destination();
        SDL_SetRenderDrawColor(drawing::renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRectF(drawing::renderer, &destination);
    }
};

class image : public drawable{
public:
    SDL_Texture* texture;

    void draw () override {
        auto destination = get_destination();
        SDL_RenderCopyF(drawing::renderer, texture, NULL, &destination);
    }
};

int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    drawing::window = SDL_CreateWindow("Ubuntu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, NULL);
    drawing::renderer = SDL_CreateRenderer(drawing::window, -1, SDL_RENDERER_ACCELERATED);

    square my_square;
    my_square.size.x = 50;
    my_square.size.y = 50;
    my_square.color.r = 255;

    image my_image;
    my_image.size.x = 300;
    my_image.size.y = 200;
    my_image.texture = IMG_LoadTexture(drawing::renderer, "./my_image.png");
    
    bool quit = false;
    while(!quit){
        drawing::clear();
        my_square.draw();
        my_image.draw();
        drawing::present();

        SDL_Event e;
        while(SDL_PollEvent(&e)){
            switch(e.type){
                case SDL_QUIT:
                    quit = true; 
                    break;
                case SDL_KEYDOWN:
                    switch(e.key.keysym.sym){
                        case SDLK_d:
                            my_square.position.x += 10.f;
                            break;
                        case SDLK_a:
                            my_square.position.x -= 10.f;
                            break;
                    }
                    break;
            }
        }
    }
    return 0;
}