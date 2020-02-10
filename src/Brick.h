#ifndef BRICK_H_INCLUDED
#define BRICK_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define BRICK_WIDTH		    48
#define BRICK_HEIGHT        24

class Brick
{
    public:

    	SDL_Rect dim;

    ///Constructor Function
    Brick(int x, int y){

    	dim.h = BRICK_HEIGHT;
    	dim.w = BRICK_WIDTH;
    	dim.x = x;
    	dim.y = y;
    }

    ///Deconstructor
    ~Brick(){
        printf("Gamestate Object Deconstructing...\n");


    }

    bool checkCollision( SDL_Rect foreignObj){
        if (dim.y > ( foreignObj.y + foreignObj.h ) )
            return false;

        if (dim.y + dim.h < foreignObj.y)
            return false;
                
        if (dim.x + dim.w < foreignObj.x)
            return false;
                    
        if (dim.x > foreignObj.x + foreignObj.w)
            return false;

        return true;

    }

    SDL_Rect getDim() {
        return dim;
    }

    void render(){

    	SDL_RenderFillRect(gRenderer, &dim);
    }

};
#endif // BRICK_H_INCLUDED