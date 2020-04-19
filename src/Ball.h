#ifndef BALL_H_INCLUDED
#define BALL_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define BALL_SIZE		    8
#define BALL_VELOCITY		6

class Ball
{
    public:

    	SDL_Rect dim;
        SDL_Point vel;

    ///Constructor Function
    Ball(){

    	dim.h = BALL_SIZE;
    	dim.w = BALL_SIZE;
    	dim.x = SCREEN_WIDTH/2 - BALL_SIZE/2;
    	dim.y = SCREEN_HEIGHT - BALL_SIZE * 10;

        vel.x = BALL_VELOCITY;
        vel.y = -BALL_VELOCITY;
    }

    Ball(SDL_Rect d, SDL_Point v){
        dim = d;
        vel = v;
    }

    ///Deconstructor
    ~Ball(){
        printf("Ball Object Deconstructing...\n");


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

    bool checkCollN(SDL_Rect foreignObj){
        int result = false;

        if (dim.y < ( foreignObj.y + foreignObj.h ) )
            result = true;

        return result;
    }

    bool checkCollS(SDL_Rect foreignObj){
        int result = false;

        if (dim.y + dim.h > foreignObj.y)
            result = true;

        return result;
    }

    bool checkCollE(SDL_Rect foreignObj){
        int result = false;

        if (dim.x + dim.w > foreignObj.x)
            result = true;

        return result;
    }

    bool checkCollW(SDL_Rect foreignObj){
        int result = false;

        if (dim.x < foreignObj.x + foreignObj.w)
            result = true;
        
        return result;
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

    void hBounce(){
        vel.x *= -1;
    }

    void vBounce(){
        vel.y *= -1;
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
#endif // BALL_H_INCLUDED