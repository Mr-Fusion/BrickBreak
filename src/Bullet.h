#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
//#include <sstream>
#include "Const.h"
//#include "GameState.h"
#include "LTimer.h"
#include "LTexture.h"


#define BULLET_HEIGHT       8
#define BULLET_WIDTH        4
#define VELOCITY            -10

class Bullet
{
    public:

    	SDL_Rect dim;
        SDL_Point vel;

        bool alive;

    ///Constructor Function
    Bullet(int x, int y){

        dim.x = x;
        dim.y = y;
        dim.w = BULLET_WIDTH;
        dim.h = BULLET_HEIGHT;

        vel.x = 0;
        vel.y = VELOCITY;

        alive = true;

    }

    ///Deconstructor
    ~Bullet(){
        printf("Bullet Object Deconstructing...\n");


    }

    SDL_Rect getDim() {
        return dim;
    }

    void hit() {
        //play sound effect or trigger more logic here
    }

    void setAlive(bool a) {
        alive = a;
    }

    bool checkAlive() {
        return alive;
    }

    bool offScreen() {
        if (dim.y + dim.h < 0)
            return true;
        if (dim.y > SCREEN_HEIGHT)
            return true;
        if (dim.x + dim.w < 0)
            return true;
        if (dim.x > SCREEN_WIDTH)
            return true;
        return false;
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

    void move(){

        dim.y += vel.y;
        dim.x += vel.x;

    }

    void render(){

    	SDL_RenderFillRect(gRenderer, &dim);
    }

};
#endif // BULLET_H_INCLUDED