#ifndef PICKUP_H_INCLUDED
#define PICKUP_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "GameEntity.h"
#include "WeightedTable.h"
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define PICKUP_SIZE		    16
#define PICKUP_VELOCITY		4

#define WEIGHT_DEFAULT   10

#define WEIGHT_MULTI     15
#define WEIGHT_PIERCE    5
#define WEIGHT_POINT     18
#define WEIGHT_LIFE      2

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
    PICKUP_NUM      = 10
};

class Pickup : public GameEntity
{
    public:

        int type;
        int r, g, b;
        WeightedTable *bonusTable = NULL;

    ///Constructor Function
    Pickup(int x,int y, int t = -1){

    	dim.h = PICKUP_SIZE;
    	dim.w = PICKUP_SIZE;
    	dim.x = x;
    	dim.y = y;

        vel.x = 0;
        vel.y = PICKUP_VELOCITY;

        type = t;

        r = g = b = 0xFF;

        bonusTable = new WeightedTable(PICKUP_NUM, WEIGHT_DEFAULT);

        bonusTable->setWeight(PICKUP_MULTI, WEIGHT_MULTI);
        bonusTable->setWeight(PICKUP_PIERCE, WEIGHT_PIERCE);
        bonusTable->setWeight(PICKUP_POINT, WEIGHT_POINT);
        bonusTable->setWeight(PICKUP_LIFE, WEIGHT_LIFE);

        bonusTable->print();

        if (type == -1)
            type = bonusTable->roll();

        setColor();
    }

    ///Deconstructor
    ~Pickup(){
        printf("Pickup Object Deconstructing...\n");

        if (bonusTable != NULL)
            delete bonusTable;
    }

    void setColor(){

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

    void randomizeColor(){
        r = rand() % 0xFF;
        g = rand() % 0xFF;
        b = rand() % 0xFF;
    }

};
#endif // PICKUP_H_INCLUDED