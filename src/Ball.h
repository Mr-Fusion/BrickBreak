#ifndef BALL_H_INCLUDED
#define BALL_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "GameEntity.h"
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define BALL_SIZE		    8
#define BALL_VELOCITY		6

class Ball : public GameEntity
{
    public:

        SDL_Point velStored;
        bool stuck;
        int offset;

    ///Constructor Function
    Ball(){

    	dim.h = BALL_SIZE;
    	dim.w = BALL_SIZE;
    	dim.x = SCREEN_WIDTH/2 - BALL_SIZE/2;
    	dim.y = SCREEN_HEIGHT - BALL_SIZE * 10;

        vel.x = velStored.x = BALL_VELOCITY/4;
        vel.y = velStored.y = -BALL_VELOCITY;

        offset = 0;
        stuck = false;
    }

    Ball(SDL_Rect d, SDL_Point v){
        dim = d;
        vel = v;

        velStored.x = BALL_VELOCITY;
        velStored.y = -BALL_VELOCITY;

        offset = 0;
        stuck = false;
    }

    ///Deconstructor
    ~Ball(){
        printf("Ball Object Deconstructing...\n");
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

    bool checkStuck() {
        return stuck;
    }

    void setStuck( bool s) {
        stuck = s;
    }

    int setOffset( int o) {
        offset = o;
        return offset;
    }

    int getOffset() {
        return offset;
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

    void storeVel(int x, int y){
        velStored.x = x;
        velStored.y = y;
    }

    SDL_Point getStoreVel(){
        return velStored;
    }

    void releaseVel() {
        vel = velStored;
    }

};
#endif // BALL_H_INCLUDED