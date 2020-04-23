#ifndef PICKUP_H_INCLUDED
#define PICKUP_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define PICKUP_SIZE		    16
#define PICKUP_VELOCITY		4

enum {
    PICKUP_POINT    = 0,    //White
    PICKUP_CATCH    = 1,    //Cyan
    PICKUP_MULTI    = 2,    //Magenta
    PICKUP_PIERCE   = 3,    //Purple
    PICKUP_SHOOT    = 4,    //Orange
    PICKUP_GROW     = 5,    //Blue
    PICKUP_SHRINK   = 6,    //Red
    PICKUP_FAST     = 7,    //Green
    PICKUP_SLOW     = 8,    //Yellow
    PICKUP_LIFE     = 9,    //..Multicolor?
    PICKUP_TYPE     = 10
};

class Pickup
{
    public:

    	SDL_Rect dim;
        SDL_Point vel;
        int type;
        int r, g, b;

    ///Constructor Function
    Pickup(int x,int y){

    	dim.h = PICKUP_SIZE;
    	dim.w = PICKUP_SIZE;
    	dim.x = x;
    	dim.y = y;

        vel.x = 0;
        vel.y = PICKUP_VELOCITY;

        type = PICKUP_POINT;

        r = g = b = 0xFF;

        setRandomType();
    }

    ///Deconstructor
    ~Pickup(){
        printf("Pickup Object Deconstructing...\n");


    }

    void setRandomType(){
        
        type = rand() % PICKUP_TYPE;

        switch (type) {
            case PICKUP_POINT:
                r = g = b = 0xFF;
            break;
            case PICKUP_CATCH:
                // CYAN
                r = 0x37;
                g = b = 0xFF;
            break;
            case PICKUP_MULTI:
                // MAGENTA
                g = 0x37;
                r = b = 0xFF;
            break;
            case PICKUP_PIERCE:
                // PURPLE
                r = 0x9B;
                g = 0x37;
                b = 0xFF;
            break;
            case PICKUP_SHOOT:
                //ORANGE
                r = 0xFF;
                g = 0x9B;
                b = 0x37;
            break;
            case PICKUP_GROW:
                // BLUE
                b = 0xFF;
                r = g = 0x37;
            break;
            case PICKUP_SHRINK:
                // RED
                r = 0xFF;
                g = b = 0x37;
            break;
            case PICKUP_FAST:
                // GREEN
                g = 0xFF;
                r = b = 0x37;
            break;
            case PICKUP_SLOW:
                // YELLOW
                b = 0x37;
                r = g = 0xFF;
            break;
            case PICKUP_LIFE:
                // BROWN... for now
                r = 0xA0;
                g = 0x63;
                b = 0x00;
            break;
            default:
                r = g = b = 0xFF;
        }
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