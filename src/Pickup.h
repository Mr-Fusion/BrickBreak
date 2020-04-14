#ifndef PICKUP_H_INCLUDED
#define PICKUP_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define PICKUP_SIZE		    16
#define PICKUP_VELOCITY		4

class Pickup
{
    public:

    	SDL_Rect dim;
        SDL_Point vel;

    ///Constructor Function
    Pickup(int x,int y){

    	dim.h = PICKUP_SIZE;
    	dim.w = PICKUP_SIZE;
    	dim.x = x;
    	dim.y = y;

        vel.x = 0;
        vel.y = PICKUP_VELOCITY;
    }

    ///Deconstructor
    ~Pickup(){
        printf("Pickup Object Deconstructing...\n");


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

    SDL_Point getVel() {
        return vel;
    }

    void setPos(int x, int y){
        dim.x = x;
        dim.y = y;
    }

    void setVel(int x, int y){
        vel.x = x;
        vel.y = y;
    }

    void update(){
        dim.x += vel.x;
        dim.y += vel.y;
    }

    void render(){

    	SDL_RenderFillRect(gRenderer, &dim);
    }

};
#endif // PICKUP_H_INCLUDED