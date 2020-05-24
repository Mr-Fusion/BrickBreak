#ifndef PADDLE_H_INCLUDED
#define PADDLE_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "GameEntity.h"
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define PADDLE_HEIGHT		8
#define PADDLE_WIDTH		80
#define PADDLE_VELOCITY		6

class Paddle : public GameEntity
{
    public:

    ///Constructor Function
    Paddle(){

    	dim.h = PADDLE_HEIGHT;
    	dim.w = PADDLE_WIDTH;
    	dim.x = SCREEN_WIDTH/2 - PADDLE_WIDTH/2;
    	dim.y = SCREEN_HEIGHT - PADDLE_HEIGHT * 2;
    }

    ///Deconstructor
    ~Paddle(){
        printf("Paddle Object Deconstructing...\n");
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

};
#endif // PADDLE_H_INCLUDED