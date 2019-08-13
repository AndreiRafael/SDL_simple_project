#include <iostream>
#include <vector>
#include <unordered_map>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

class Vec2 {
public:
    float x = 0.f;
    float y = 0.f;
};

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

    const Vec2 screen_point_to_world(const Vec2& screen_point) {
        int winW, winH;
        SDL_GetRendererOutputSize(drawing::renderer, &winW, &winH);

        Vec2 out = screen_point;
        out.x -= winW / 2;
        out.y = winH / 2 - out.y;
        return out; 
    }

    const Vec2 touch_point_to_screen(const float x, const float y){
        int winW, winH;
        SDL_GetRendererOutputSize(drawing::renderer, &winW, &winH);

        Vec2 out;
        out.x = x * (float)winW;
        out.y = y * (float)winH;

        return out;
    }
}

class Drawable {
public:
    Vec2 position;
    Vec2 size;
    SDL_Color color = { 255, 255, 255, 255 };

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

class Square : public Drawable{
public:
    void draw() override {
        auto destination = get_destination();
        SDL_SetRenderDrawColor(drawing::renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRectF(drawing::renderer, &destination);
    }
};

class Image : public Drawable{
public:
    SDL_Texture* texture;

    void draw () override {
        auto destination = get_destination();
        SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
        SDL_SetTextureAlphaMod(texture, color.a);
        SDL_RenderCopyF(drawing::renderer, texture, NULL, &destination);
    }
};

class Text : public Image {
private:
    TTF_Font* font;
    std::string text;
public:
    void set_font(TTF_Font* font) {
        this->font = font;

        set_text(text);
    }

    void set_text(const std::string& text){
        if(font == nullptr) {
            std::cout << "TEXT ERROR: Font is null";
            return;
        }
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface *surf = TTF_RenderUTF8_Blended(font, text.c_str(), white);
        if(surf == nullptr){
            std::cout << "TEXT ERROR: " << TTF_GetError() << '\n';
            return;
        }
        texture = SDL_CreateTextureFromSurface(drawing::renderer, surf);
            if(texture == nullptr){
            std::cout << "TEXT ERROR: " << TTF_GetError() << '\n';
        }
    }
};

int main(int argc, char* argv[]){
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();

    SDL_SetHint(SDL_HINT_MOUSE_TOUCH_EVENTS, "1");

    drawing::window = SDL_CreateWindow("Ubuntu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, NULL);
    drawing::renderer = SDL_CreateRenderer(drawing::window, -1, SDL_RENDERER_ACCELERATED);

    Square my_square;
    my_square.size.x = 50;
    my_square.size.y = 50;
    my_square.color.r = 255;

    Image my_image;
    my_image.size.x = 300;
    my_image.size.y = 200;
    my_image.texture = IMG_LoadTexture(drawing::renderer, "./my_image.png");

    TTF_Font* font  = TTF_OpenFont("./BethEllen-Regular.ttf", 24);
    if(font == nullptr){
        std::cout << "FONT NOT LOADED: " << TTF_GetError() << '\n';
    }

    Text my_text;
    my_text.size.x = 400;
    my_text.size.y = 100;
    my_text.set_font(font);
    my_text.set_text("Olá mundo!");

    std::unordered_map<SDL_FingerID, Drawable*> to_move;
    
    bool quit = false;
    while(!quit){
        drawing::clear();
        my_square.draw();
        my_image.draw();
        my_text.draw();
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
                case SDL_FINGERDOWN:
                    if(e.tfinger.x < 0.33f){//pega cubo
                        to_move.insert_or_assign(e.tfinger.fingerId, &my_square);
                    } 
                    else if(e.tfinger.x < 0.66f){//pega imagem
                        to_move.insert_or_assign(e.tfinger.fingerId, &my_image);
                    }
                    else{//pega texto
                        to_move.insert_or_assign(e.tfinger.fingerId, &my_text);
                    }
                    break;
                case SDL_FINGERMOTION:{
                        auto it = to_move.find(e.tfinger.fingerId);
                        if(it != to_move.end()){
                            auto drawable = it->second;
                            drawable->position = drawing::screen_point_to_world(drawing::touch_point_to_screen(e.tfinger.x, e.tfinger.y));
                        }
                    }
                    break;
                case SDL_FINGERUP:
                    to_move.erase(e.tfinger.fingerId);
                    break;
            }
        }
    }
    return 0;
}