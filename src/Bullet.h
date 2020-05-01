#ifndef BULLET_H_INCLUDED
#define BULLET_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "GameEntity.h"
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"


#define BULLET_HEIGHT       8
#define BULLET_WIDTH        4
#define VELOCITY            -10

class Bullet : public GameEntity
{
    public:
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

    void hit() {
        //play sound effect or trigger more logic here
    }

    void setAlive(bool a) {
        alive = a;
    }

    bool checkAlive() {
        return alive;
    }

};
#endif // BULLET_H_INCLUDED