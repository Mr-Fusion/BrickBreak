#ifndef PICKUP_H_INCLUDED
#define PICKUP_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "GameEntity.h"
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define PICKUP_SIZE		    16
#define PICKUP_VELOCITY		4

#define WEIGHT_CATCH    10
#define WEIGHT_MULTI    15
#define WEIGHT_PIERCE   5
#define WEIGHT_SHOOT    10

#define WEIGHT_GROW     10
#define WEIGHT_SHRINK   10
#define WEIGHT_FAST     10
#define WEIGHT_SLOW     10

#define WEIGHT_LIFE     1


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

class Pickup : public GameEntity
{
    public:

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

        type = ( rand() % PICKUP_TYPE);

        switch (type) {
            case PICKUP_POINT:
                r = g = b = 0xFF;
            break;
            case PICKUP_CATCH:
                // CYAN
                r = 0x37; g = 0xFF; b = 0xFF;
            break;
            case PICKUP_MULTI:
                // MAGENTA
                r = 0xFF; g = 0x37; b = 0xFF;
            break;
            case PICKUP_PIERCE:
                // PURPLE
                r = 0x9B; g = 0x37; b = 0xFF;
            break;
            case PICKUP_SHOOT:
                //ORANGE
                r = 0xFF; g = 0x9B; b = 0x37;
            break;
            case PICKUP_GROW:
                // BLUE
                r = 0x37; g = 0x37; b = 0xFF; 
            break;
            case PICKUP_SHRINK:
                // RED
                r = 0xFF; g = 0x37; b = 0x37;
            break;
            case PICKUP_FAST:
                // GREEN
                r =  0x37; g = 0xFF; b = 0x37;
            break;
            case PICKUP_SLOW:
                // YELLOW
                r = 0xFF; b = 0x37; g = 0xFF;
            break;
            case PICKUP_LIFE:
                // BROWN... for now
                r = 0xA0; g = 0x63; b = 0x00;
            break;
            default:
                r = g = b = 0xFF;
        }
    }

};
#endif // PICKUP_H_INCLUDED