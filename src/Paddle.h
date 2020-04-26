#ifndef PADDLE_H_INCLUDED
#define PADDLE_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define PADDLE_HEIGHT		8
#define PADDLE_WIDTH		80
#define PADDLE_VELOCITY		6

#define PADDLE_WIDTH_MOD    20

#define PADDLE_HIT_DIVIDER  10

class Paddle
{
    public:

    	SDL_Rect dim;

    ///Constructor Function
    Paddle(){

    	dim.h = PADDLE_HEIGHT;
    	dim.w = PADDLE_WIDTH;
    	dim.x = SCREEN_WIDTH/2 - PADDLE_WIDTH/2;
    	dim.y = SCREEN_HEIGHT - PADDLE_HEIGHT * 2;
    }

    ///Deconstructor
    ~Paddle(){
        printf("Gamestate Object Deconstructing...\n");


    }

    bool checkCollision( SDL_Rect foreignObj){
        if (dim.y >= ( foreignObj.y + foreignObj.h ) )
            return false;

        if (dim.y + dim.h <= foreignObj.y)
            return false;
                
        if (dim.x + dim.w <= foreignObj.x)
            return false;
                    
        if (dim.x >= foreignObj.x + foreignObj.w)
            return false;

        return true;

    }

    SDL_Rect getDim() {
        return dim;
    }

    void setDim(SDL_Rect d) {
        dim = d;
    }

    void moveLeft(){
    	dim.x -= PADDLE_VELOCITY;
    	if (dim.x < 0)
    		dim.x = 0;
    }

    void moveRight() {
        dim.x += PADDLE_VELOCITY;
        if (dim.x > SCREEN_WIDTH - dim.w)
            dim.x = SCREEN_WIDTH - dim.w;
    }

    void render(){
        SDL_RenderFillRect(gRenderer, &dim);
    }

};
#endif // PADDLE_H_INCLUDED