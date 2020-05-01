#ifndef BRICK_H_INCLUDED
#define BRICK_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "GameEntity.h"
#include "Const.h"
#include "LTimer.h"
#include "LTexture.h"

#define BRICK_WIDTH		    48
#define BRICK_HEIGHT        24
#define BRICKSHEET_COLS     1
#define BRICKSHEET_ROWS     10

LTexture brickSheet;
SDL_Rect brickClips[BRICKSHEET_ROWS * BRICKSHEET_COLS];

enum {
    BRICK_NONE = 0,
    BRICK_WHITE = 1,
    BRICK_GRAY = 2,
    BRICK_DARK = 3,
    BRICK_RED = 4,
    BRICK_ORANGE = 5,
    BRICK_YELLOW = 6,
    BRICK_GREEN = 7,
    BRICK_BLUE = 8,
    BRICK_PINK = 9,
    BRICK_TYPE = 10
};

bool loadBrickTextures(){

    bool success = true;

    if (!brickSheet.hasTexture()){
        if( !brickSheet.loadFromFile( "../assets/brickSheet_2.png") )
        {
            printf( "Failed to load brick sprite sheet texture!\n" );
            success = false;
        }
    }

    int n = 0;

    for (int i = 0; i < BRICKSHEET_COLS; i++){
        for (int j = 0; j < BRICKSHEET_ROWS; j++){
            brickClips[ n ].x = BRICK_WIDTH * i;
            brickClips[ n ].y = BRICK_HEIGHT * j;
            brickClips[ n ].w = BRICK_WIDTH;
            brickClips[ n ].h = BRICK_HEIGHT;
            n++;
        }
    }

    return success;
}

class Brick : public GameEntity
{
    public:
        int type;

    ///Constructor Function
    Brick(int x, int y, int t = 0){

    	dim.h = BRICK_HEIGHT;
    	dim.w = BRICK_WIDTH;
    	dim.x = x;
    	dim.y = y;
        type = t;
        if (type >= BRICK_TYPE )
            type = 0;

        loadBrickTextures();
    }

    ///Deconstructor
    ~Brick(){
        printf("Brick Object Deconstructing...\n");
    }

    int getType(){
        return type;
    }

    void setType(int t){
        type = t;
    }

    void render(){
        brickSheet.render(dim.x, dim.y, &brickClips[type]);
    }

};
#endif // BRICK_H_INCLUDED