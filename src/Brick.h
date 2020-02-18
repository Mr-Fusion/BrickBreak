#ifndef BRICK_H_INCLUDED
#define BRICK_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
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
    BRICK_WHITE = 0,
    BRICK_GRAY = 1,
    BRICK_DARK = 2,
    BRICK_RED = 3,
    BRICK_ORANGE = 4,
    BRICK_YELLOW = 5,
    BRICK_GREEN = 6,
    BRICK_BLUE = 7,
    BRICK_PURPLE = 8,
    BRICK_PINK = 9,
    BRICK_TYPE = 10
};

bool loadBrickTextures(){

    bool success = true;

    if (!brickSheet.hasTexture()){
        if( !brickSheet.loadFromFile( "../assets/brickSheet.png") )
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

class Brick
{
    public:

    	SDL_Rect dim;
        int color;

    ///Constructor Function
    Brick(int x, int y, int c = 0){

    	dim.h = BRICK_HEIGHT;
    	dim.w = BRICK_WIDTH;
    	dim.x = x;
    	dim.y = y;
        color = c;
        if (color >= BRICK_TYPE )
            color = 0;

        loadBrickTextures();
    }

    ///Deconstructor
    ~Brick(){
        printf("Gamestate Object Deconstructing...\n");


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

    int getType(){
        return color;
    }

    void setType(int c){
        color = c;
    }

    SDL_Rect getDim() {
        return dim;
    }

    void render(){

    	//SDL_RenderFillRect(gRenderer, &dim);
        brickSheet.render(dim.x, dim.y, &brickClips[color]);
    }

};
#endif // BRICK_H_INCLUDED