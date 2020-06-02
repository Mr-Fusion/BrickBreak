#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <vector>
#include "Const.h"
#include "GameState.h"
#include "GameEntity.h"
#include "LTimer.h"
#include "LTexture.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"
#include "Pickup.h"
#include "Bullet.h"

#define DEFAULT_LIVES       3
#define NUM_ROWS            18
#define NUM_COLS            11
#define NUM_BRICKS          NUM_ROWS * NUM_COLS
#define DEFAULT_LUCK        8

#define PADDLE_WIDTH_MOD    20
#define PADDLE_HIT_DIVIDER  6//10

#define SPEED_INDEX_MAX     6//4
#define SPEED_INDEX_MIN     0//-2
#define SPEED_INDEX_DEFAULT 3

#define XVEL_MIN            1
#define XVEL_MAX            8

#define SCORE_LIFEUP        5000

#define SCOREBOARD_WIDTH    SCREEN_WIDTH
#define SCOREBOARD_HEIGHT   60

#define MAX_LEVEL           34
#define DELAY_TIME          2400

class GameLoop : public GameState
{
    public:

    // Gameplay Stats/Values
    int currentLev = 0;
    int lives = 0;
    int score = 0;
    int hitCount = 0;
    int paddleHitDiv = 10;
    int speedIndex = SPEED_INDEX_DEFAULT;
    int speedupThresh = 15;
    int yLaunchVel = BALL_VELOCITY + speedIndex;

    // Gameplay Event Flags
    bool f_ShowInfo = false;
    bool f_InfoFade = false;
    bool f_GameOver = false;
    bool f_LevelComplete = false;
    bool f_paused = false;

    // Option Flags
    bool f_soundEnable = true;
    bool f_scoreEnable = true;
    bool f_keepPwrUps = false;
    bool f_infiniteLives = false;
    bool f_multiMode = false;
    bool f_shuffleLvls = false;

    // PowerUp status flags
    bool stuck, piercing, catching, lasers;

    // Player Input Control Flags
    bool lInput,rInput,uInput,dInput,spInput;

    // Background color
    int bgR = 0x00; int bgG = 0x00; int bgB = 0x00;

    // Scoreboard color
    int scR = 0x3F; int scG = 0x3F; int scB = 0x3F;

    // Sprite color
    int spR = 0xFF; int spG = 0xFF; int spB = 0xFF;

    // Info Text Color
    int inR = 0xFF; int inG = 0xFF; int inB = 0xFF;

    // Temporary Color
    int tmpR, tmpG, tmpB;

    SDL_Color textColor = { spR, spG, spB, 0xFF};

    // In memory text stream
    std::stringstream msgText;

    // Scene textures
    LTexture livesTextTexture;
    LTexture infoTextTexture;
    LTexture scoreTextTexture;

    // Timers for flag handling
    LTimer delayTimer;

    // Sound Effects
    Mix_Chunk *sfx_pwrPierce = NULL;
    Mix_Chunk *sfx_pwrShrink = NULL;
    Mix_Chunk *sfx_pwrPnts = NULL;
    Mix_Chunk *sfx_paddleHit = NULL;
    Mix_Chunk *sfx_wallHit = NULL;
    Mix_Chunk *sfx_brickHit = NULL;
    Mix_Chunk *sfx_brickDestroy = NULL;
    Mix_Chunk *sfx_laserShot = NULL;
    Mix_Chunk *sfx_ballMiss = NULL;
    Mix_Chunk *sfx_lastBallMiss = NULL;
    Mix_Chunk *sfx_pauseIn = NULL;
    Mix_Chunk *sfx_pauseOut = NULL;
    Mix_Chunk *sfx_pwrSpeedUp = NULL;
    Mix_Chunk *sfx_pwrSpeedDown = NULL;
    Mix_Chunk *sfx_lifeUp = NULL;

    Mix_Chunk *sfx_pwrCatch = NULL;
    Mix_Chunk *sfx_pwrMulti = NULL;
    Mix_Chunk *sfx_pwrShoot = NULL;
    Mix_Chunk *sfx_pwrGrow = NULL;

    // Game Entities
    SDL_Rect field;
    SDL_Rect scoreBoard;
    Paddle player;
    Pickup *pickup = NULL;
    Bullet *laserA = NULL;
    Bullet *laserB = NULL;

    std::vector<Brick*> wall;
    std::vector<Ball*> balls;
    std::vector<int> levelArray;

    // Temp variables
    int offsetPB;

    int pickupRate = 0;
    int brickMap[NUM_BRICKS];
    SDL_Rect tempDim;

    ///Constructor Function
    GameLoop(settings s){

        SDL_ShowCursor(SDL_DISABLE);
        
        lInput = rInput = uInput = dInput = spInput = false;
        stuck = piercing = catching = lasers = false;

        f_soundEnable = s.sfxEnable;//true;

        if (s.difficulty == DIFFY_FREE){
            f_scoreEnable = false;
            f_infiniteLives = true;
            f_keepPwrUps = true;
        }

        if (s.difficulty == DIFFY_EASY){
            speedupThresh = 20;
            f_keepPwrUps = true;
        }

        if (s.shuffleEnable)
            f_shuffleLvls = true;

        if (s.multiEnable)
            f_multiMode = true;

        field.x = 0;
        field.y = SCOREBOARD_HEIGHT;
        field.w = SCREEN_WIDTH;
        field.h = SCREEN_HEIGHT - field.y;

        scoreBoard.x = 0;
        scoreBoard.y = 0;
        scoreBoard.w = SCOREBOARD_WIDTH;
        scoreBoard.h = SCOREBOARD_HEIGHT;

        for (int i = 0; i < NUM_BRICKS; i++)
            brickMap[i] = 0;

        balls.push_back(new Ball());

        for (int i = 0; i < balls.size(); i++)
            resetBall(balls[i]);

        for (int i = 0; i < MAX_LEVEL; i++)
            levelArray.push_back(i+1);
        

        if (f_shuffleLvls) {
            int tempVal;
            int tempIndex;

            for (int j = 0; j < MAX_LEVEL; j++) {
                for (int i = 0; i < MAX_LEVEL; i++) {
                    tempIndex = rand() % MAX_LEVEL;
                    tempVal = levelArray[tempIndex];
                    levelArray[tempIndex] = levelArray[i];
                    levelArray[i] = tempVal;
                }
            }
        }

        for (int i = 0; i < MAX_LEVEL; i++)
            printf("Level %d is Round %d\n",i+1,levelArray[i]);

        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Initialize playing field dimensions, difficulty, and appearance
            lives = DEFAULT_LIVES;
            score = 0;
            pickupRate = DEFAULT_LUCK;

            if (!f_infiniteLives)
                livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
            else
                livesTextTexture.loadFromRenderedText( updateText("Freeplay"), textColor);

            if (f_scoreEnable)
                scoreTextTexture.loadFromRenderedText( updateText("Score: ", score), textColor);
            else
                scoreTextTexture.loadFromRenderedText( updateText("Freeplay"), textColor);
            goNextLevel();

            //Initialize and display graphical interface
            SDL_SetWindowSize(gWindow,SCREEN_WIDTH,SCREEN_HEIGHT);
        }
    }

    ///Deconstructor
    ~GameLoop(){
        printf("Gamestate Object Deconstructing...\n");
        delayTimer.stop();

        //Free the sound effects
        Mix_FreeChunk( sfx_pwrPierce );
        sfx_pwrPierce = NULL;
        Mix_FreeChunk( sfx_pwrShrink );
        sfx_pwrShrink = NULL;
        Mix_FreeChunk( sfx_pwrPnts );
        sfx_pwrPnts = NULL;
        Mix_FreeChunk( sfx_paddleHit );
        sfx_paddleHit = NULL;
        Mix_FreeChunk( sfx_wallHit );
        sfx_wallHit = NULL;
        Mix_FreeChunk( sfx_brickHit );
        sfx_brickHit = NULL;
        Mix_FreeChunk( sfx_brickDestroy );
        sfx_brickDestroy = NULL;
        Mix_FreeChunk( sfx_laserShot );
        sfx_laserShot = NULL;
        Mix_FreeChunk( sfx_ballMiss );
        sfx_ballMiss = NULL;
        Mix_FreeChunk( sfx_lastBallMiss );
        sfx_lastBallMiss = NULL;
        Mix_FreeChunk( sfx_pauseIn );
        sfx_pauseIn = NULL;
        Mix_FreeChunk( sfx_pauseOut );
        sfx_pauseOut = NULL;

        Mix_FreeChunk( sfx_pwrSpeedUp );
        sfx_pwrSpeedUp = NULL;
        Mix_FreeChunk( sfx_pwrSpeedDown );
        sfx_pwrSpeedDown = NULL;
        Mix_FreeChunk( sfx_lifeUp );
        sfx_lifeUp = NULL;

        Mix_FreeChunk( sfx_pwrCatch );
        sfx_pwrCatch = NULL;
        Mix_FreeChunk( sfx_pwrMulti );
        sfx_pwrMulti = NULL;
        Mix_FreeChunk( sfx_pwrShoot );
        sfx_pwrShoot = NULL;
        Mix_FreeChunk( sfx_pwrGrow );
        sfx_pwrGrow = NULL;

        //Free loaded image
        livesTextTexture.free();
        infoTextTexture.free();
        scoreTextTexture.free();

        //delete all pointers in the wall vector and clear all elements
        for (int i = wall.size() - 1; i >= 0; i--){
            delete wall[i];
        }

        for (int i = balls.size() - 1; i >= 0; i--){
            delete balls[i];
        }

        if (laserA != NULL)
            delete laserA;

        if (laserB != NULL)
            delete laserB;

        wall.clear();
        balls.clear();
    }

    void CSVread(int level)
    {
        std::ifstream fin;
        std::string line;
        std::stringstream result;

        result.str( "" );
        result << "../assets/round" << level << ".csv";

        // Open an existing file
        fin.open(result.str());

        // Clear "result" in preparation for reading the file
        result.clear();
        result.str( "" );
        
        while(!fin.eof()){
            fin>>line;
            result<<line<<" ";
        }
        result<<"\0";
        std::string myString = result.str();

        int i = 0;
        int j = 0;
        char myChar = NULL;
        int value = 0;
        while (myString[i] != '\0'){
            if (myString[i] >= '0' && myString[i] <= '9'){
                myChar = myString[i];
                brickMap[j] = myChar - 48; 
                j++;
            }
            i++;
        }

        printf("CSVRead Complete\n");
    }

    //TODO: Can we streamline the sprite sheet creation into a function?
    bool loadMedia()
    {
        //Loading success flag
        bool success = true;

        //Load sound effects
        if (f_soundEnable == true) {
            sfx_pwrPnts = Mix_LoadWAV( "../assets/sfx_coin_double4.wav" );
            if( sfx_pwrPnts == NULL )
            {
                printf( "Failed to load sound effect 1! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

            sfx_ballMiss = Mix_LoadWAV( "../assets/sfx_damage_hit3.wav" );
            if( sfx_ballMiss == NULL )
            {
                printf( "Failed to load sound effect 2! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

            sfx_lastBallMiss = Mix_LoadWAV( "../assets/sfx_exp_various6.wav" );
            if( sfx_lastBallMiss == NULL )
            {
                printf( "Failed to load sound effect 3! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

            sfx_brickHit = Mix_LoadWAV( "../assets/sfx_sounds_Blip2.wav" );
            if( sfx_brickHit == NULL )
            {
                printf( "Failed to load sound effect 4! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

            sfx_wallHit = Mix_LoadWAV( "../assets/sfx_sounds_Blip7.wav" );
            if( sfx_wallHit == NULL )
            {
                printf( "Failed to load sound effect 5! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_paddleHit = Mix_LoadWAV( "../assets/sfx_sounds_Blip9.wav" );
            if( sfx_paddleHit == NULL )
            {
                printf( "Failed to load sound effect 6! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_brickDestroy = Mix_LoadWAV( "../assets/sfx_sounds_Blip7.wav" ); //Blip11
            if( sfx_brickDestroy == NULL )
            {
                printf( "Failed to load sound effect 7! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pwrShrink = Mix_LoadWAV( "../assets/sfx_shift_down.wav" );
            if( sfx_pwrShrink == NULL )
            {
                printf( "Failed to load sound effect 8! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pauseIn = Mix_LoadWAV( "../assets/sfx_sounds_pause1_in.wav" );
            if( sfx_pauseIn == NULL )
            {
                printf( "Failed to load sound effect 9! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pauseOut = Mix_LoadWAV( "../assets/sfx_sounds_pause1_out.wav" );
            if( sfx_pauseOut == NULL )
            {
                printf( "Failed to load sound effect 10! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pwrPierce = Mix_LoadWAV( "../assets/sfx_shift_up.wav" );
            if( sfx_pwrPierce == NULL )
            {
                printf( "Failed to load sound effect 11! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_laserShot = Mix_LoadWAV( "../assets/sfx_laser.wav" );
            if( sfx_laserShot == NULL )
            {
                printf( "Failed to load sound effect 12! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

            sfx_pwrSpeedUp = Mix_LoadWAV( "../assets/sfx_speed_up.wav" );
            if( sfx_pwrSpeedUp == NULL )
            {
                printf( "Failed to load sound effect 13! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pwrSpeedDown = Mix_LoadWAV( "../assets/sfx_speed_down.wav" );
            if( sfx_pwrSpeedDown == NULL )
            {
                printf( "Failed to load sound effect 14! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_lifeUp = Mix_LoadWAV( "../assets/sfx_life_up.wav" );
            if( sfx_lifeUp == NULL )
            {
                printf( "Failed to load sound effect 15! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }

            sfx_pwrCatch = Mix_LoadWAV( "../assets/sfx_catch.wav" );
            if( sfx_pwrCatch == NULL )
            {
                printf( "Failed to load sound effect 16! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pwrGrow = Mix_LoadWAV( "../assets/sfx_grow.wav" );
            if( sfx_pwrGrow == NULL )
            {
                printf( "Failed to load sound effect 17! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pwrShoot = Mix_LoadWAV( "../assets/sfx_shoot.wav" );
            if( sfx_lifeUp == NULL )
            {
                printf( "Failed to load sound effect 18! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
            sfx_pwrMulti = Mix_LoadWAV( "../assets/sfx_multi.wav" );
            if( sfx_lifeUp == NULL )
            {
                printf( "Failed to load sound effect 19! SDL_mixer Error: %s\n", Mix_GetError() );
                success = false;
            }
        }

        //Render text
        livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);

        infoTextTexture.loadFromRenderedText( updateText("Round: ", currentLev), textColor);

        return success;
    }

    void playSound(int channel, Mix_Chunk *sound, int loops){
        if (sound != NULL)
            Mix_PlayChannel( channel, sound , loops );
    }

    std::string updateText(std::string text, int num = -1){
        std::stringstream result;

        result.str("");
        result << text;

        if (num != -1)
            result << num;

        return result.str();
    }

    void goNextLevel(){

        // Increment and load next level
        if (currentLev > MAX_LEVEL)
            currentLev = 0;
        CSVread(levelArray[currentLev]);
        currentLev++;

        // Clear all balls
        for (int i = balls.size() - 1; i >= 0 ; i--){
            delete balls[i];
            balls.erase(balls.begin()+i);
        }

        // Serve new ball
        balls.push_back(new Ball());
        resetBall(balls[0]);

        if (!f_keepPwrUps){
            piercing = false;
            catching = false;
            lasers = false;
        }

        int k = 0;
        for (int j = 0; j < NUM_ROWS; j++){
            for (int i = 0; i < NUM_COLS; i++){
                if ( brickMap[k] != 0)
                    wall.push_back(new Brick(0 + BRICK_WIDTH * i, SCOREBOARD_HEIGHT + BRICK_HEIGHT * j, brickMap[k]));
                k++;
            }
        }

        // Update level string and display text on screen
        infoTextTexture.loadFromRenderedText( updateText("Round: ", currentLev), textColor);
        inR = inG = inB = 0xFF;
        f_ShowInfo = true;

        // Set countdown to hide level text some time after level begins
        delayTimer.start();

    }

    void resetBall(Ball *thisBall){

        hitCount = 0;
        speedIndex = SPEED_INDEX_DEFAULT;
        paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;
        yLaunchVel  = BALL_VELOCITY + speedIndex;
        thisBall->storeVel(yLaunchVel/4,-yLaunchVel);

        thisBall->setStuck(true);
        //TODO: Revisit "Sticky" Functions
        tempDim = player.getDim();
        //offsetPB = tempDim.w/2;
        offsetPB = thisBall->setOffset(tempDim.w/2);
        thisBall->setPos(tempDim.x + offsetPB, tempDim.y - thisBall->getDim().h );
    }

    // Function to handle hit detection between a ball specified by a pointer, and all bricks on the playing field
    void hitDetection(Ball *thisBall){

        // Temp Variables
        SDL_Point   ballVel = thisBall->getVel();
        SDL_Rect    ballNextPos = thisBall->getDim();
        SDL_Rect    ballDim = thisBall->getDim();
        SDL_Rect    brickDim;
        int         hitCntX, hitCntY;

        hitCntX = hitCntY = 0;
        ballNextPos.x += ballVel.x;
        ballNextPos.y += ballVel.y;

        //--- Brick Hit Detection ---//

        for (int i = wall.size()-1; i >= 0 ; i--){

            // Determine if the ball is going to collide with one of the bricks on the playfield.
            // To simplify calculations, only the "leading" edges of the ball are checked against each brick.
            // Count the number of total horizontal and vertical collisions;
            // This will be used to determine the bounce outcome of the ball.
            if (wall[i]->checkCollision(ballNextPos) == true){

                // Horizontal collision detection
                if (ballVel.x >= 0){
                    if ( thisBall->checkCollE(wall[i]->getDim()) == false )
                        hitCntX++;
                }
                else {
                    if ( thisBall->checkCollW(wall[i]->getDim()) == false )
                        hitCntX++;
                }

                // Vertical collision detection
                if (ballVel.y >= 0){
                    if ( thisBall->checkCollS(wall[i]->getDim()) == false )
                        hitCntY++;
                }
                else {
                    if ( thisBall->checkCollN(wall[i]->getDim()) == false )
                        hitCntY++;
                }

                // Resolve outcome for the brick which was hit depending on its type
                wallHit(i);

                // Increment total Hit Count
                hitTracker(1);
            }
        }

        //--- Ball Bounce Handling ---//

        // Quick and dirty "algorithm" for determining bounce result after interacting with bricks.
        // Compare the number of horizonal and vertical collisions:
        // If more vertical than horizontal: Ball bounces vertically
        // If more horizontal than vertical: Ball bounces horizontally
        // If vertical and horizontal collisions are equal: Bounce in both directions (corner hit)
        if (!piercing){
            if (hitCntX > 0 || hitCntY > 0){
                if (hitCntY > hitCntX)
                    thisBall->vBounce();
                else if (hitCntY < hitCntX)
                    thisBall->hBounce();
                else {
                    thisBall->vBounce();
                    thisBall->hBounce();
                }
            }
        }


        // Top edge of field
        if ( ( ballDim.y < field.y ) && ( ballVel.y < 0 ) ) {
            playSound( -1, sfx_wallHit , 0 );
            thisBall->vBounce();
        }

        // Right edge of field
        if ( ( ballDim.x > SCREEN_WIDTH - ballDim.w ) && ( ballVel.x > 0 ) ) {
            playSound( -1, sfx_wallHit , 0 );
            thisBall->hBounce();
        }

        // Left edge of field
        if ( ( ballDim.x < 0 ) && ( ballVel.x < 0 ) ) {
            playSound( -1, sfx_wallHit , 0 );
            thisBall->hBounce();
        }

    }

    void wallHit(int index){

        SDL_Rect brickDim;
        bool destroy = false;

        switch (wall[index]->getType()){
            case BRICK_GRAY:
                if (piercing){
                    playSound( -1, sfx_brickDestroy , 0 );
                    destroy = true;
                    addPoints(20);
                }
                else {
                    playSound( -1, sfx_brickHit , 0 );
                    wall[index]->setType(BRICK_WHITE);
                    addPoints(10);
                }

            break;
            case BRICK_DARK:
                if (piercing){
                    playSound( -1, sfx_brickDestroy , 0 );
                    destroy = true;
                    addPoints(30);
                }
                else {
                    playSound( -1, sfx_brickHit , 0 );
                    wall[index]->setType(BRICK_GRAY);
                    addPoints(10);
                }
            break;
            default:
                playSound( -1, sfx_brickDestroy , 0 );
                destroy = true;
                addPoints(10);
            break;
        }

        if (destroy){
            brickDim = wall[index]->getDim();
            delete wall[index];
            wall.erase(wall.begin()+index);


            // Roll for spawning a pickup from eliminated brick
            // If the roll is unsuccessful, increase the likelihood for next time
            if ( ( rand() % pickupRate ) >= 10){
                if (pickup == NULL){
                    if (f_multiMode)
                        pickup = new Pickup(brickDim.x + brickDim.w/2 - PICKUP_SIZE/2,brickDim.y, PICKUP_MULTI);
                    else
                        pickup = new Pickup(brickDim.x + brickDim.w/2 - PICKUP_SIZE/2,brickDim.y);
                    
                    pickupRate = (rand() % DEFAULT_LUCK) + 1;
                }
            }
            else
                pickupRate++;

            // If all bricks have been elimated, flag the level as completed
            if (wall.size() == 0){
                f_LevelComplete = true;
                delayTimer.start();
            }
        }
    }

    void laserHandling(Bullet *laser){

        laser->move();

        SDL_Rect laserDim = laser->getDim();

        for (int i = wall.size()-1; i >= 0 ; i--){
            if (wall[i]->checkCollision(laserDim) == true) {

                // Resolve outcome for the brick which was hit depending on its type
                wallHit(i);

                if (!piercing)
                    laser->setAlive( false );
            }
        }

        if (laserDim.y < SCOREBOARD_HEIGHT)
            laser->setAlive( false );

    }

    void addPoints(int p){

        if (!f_scoreEnable)
            return;

        int tempScore = score;
        score += p;

        // Update Score Display texture
        scoreTextTexture.loadFromRenderedText( updateText("Score: ", score), textColor);

        if (f_infiniteLives)
            return;

        // If score increments past SCORE_LIFEUP threshold, add an extra live
        if ( (score / SCORE_LIFEUP) > (tempScore / SCORE_LIFEUP) ){
            lives++;
            playSound( -1, sfx_lifeUp , 0 );
            livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
        }
    }

    void hitTracker(int h){
        int tempCount = hitCount;
        hitCount += h;

        // If score increments past speedup threshold, increase speed
        if ( (hitCount / speedupThresh) > (tempCount / speedupThresh) )
            speedUp();

        printf("Hit Count is: %d\n",hitCount);
    }

    void speedUp(){
        SDL_Point tempVel;

        for (int i = balls.size() - 1; i >= 0 ; i--){
            tempVel = balls[i]->getVel();

            if ( tempVel.x > 1)
                tempVel.x++;
            else if ( tempVel.x < -1)
                tempVel.x--;

            if ( tempVel.y > 1)
                tempVel.y++;
            else if ( tempVel.y < -1)
                tempVel.y--;

            balls[i]->setVel(tempVel.x, tempVel.y);
        }

        if (speedIndex < SPEED_INDEX_MAX)
            speedIndex++;

        paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;
        yLaunchVel  = BALL_VELOCITY + speedIndex;

        printf("Speed up! Index is now %d\n",speedIndex);

    }

    void slowDown(){
        SDL_Point tempVel;

        for (int i = balls.size() - 1; i >= 0 ; i--){
            tempVel = balls[i]->getVel();

            if ( tempVel.x > 1)
                tempVel.x--;
            else if ( tempVel.x < -1)
                tempVel.x++;

            if ( tempVel.y > 1)
                tempVel.y--;
            else if ( tempVel.y < -1)
                tempVel.y++;

            balls[i]->setVel(tempVel.x, tempVel.y);
        }

        if (speedIndex > SPEED_INDEX_MIN)
            speedIndex--;

        paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;
        yLaunchVel  = BALL_VELOCITY + speedIndex;

        printf("Slow down! Index is now %d\n",speedIndex);
    }

    ///Handles Player input
    void handleEvent( SDL_Event* e){

        int x, y;
        SDL_Rect tempDim = player.getDim();
/*
        int x, y;

        //Get mouse position
        if( e->type == SDL_MOUSEMOTION ){
            SDL_GetMouseState( &x, &y );
            //lPaddle.x = x;
        lPaddle.y = y - (lPaddle.h/2);
            if ( y > SCREEN_HEIGHT - lPaddle.h/2 )
                lPaddle.y = SCREEN_HEIGHT - lPaddle.h;
            if ( y < lPaddle.h/2 )
                lPaddle.y = 0;
        }


        if( e->button.button == SDL_BUTTON_RIGHT && e->type == SDL_MOUSEBUTTONUP ){
            set_next_state(STATE_MENU);
        }
*/

        //Get mouse position
        if( e->type == SDL_MOUSEMOTION ){
            if (!f_paused){
                SDL_GetMouseState( &x, &y );
                
                tempDim.x = x - tempDim.w/2;

                if ( x > SCREEN_WIDTH - tempDim.w/2 )
                    tempDim.x = SCREEN_WIDTH - tempDim.w;
                if ( x < tempDim.w/2 )
                    tempDim.x = 0;

                player.setDim(tempDim);
            }
        }

        if( e->button.button == SDL_BUTTON_LEFT && e->type == SDL_MOUSEBUTTONDOWN ){
            spInput = true;
        }
        if( e->button.button == SDL_BUTTON_LEFT && e->type == SDL_MOUSEBUTTONUP ){
            spInput = false;
        }

        if( e->button.button == SDL_BUTTON_RIGHT && e->type == SDL_MOUSEBUTTONUP ){
            set_next_state(STATE_MENU);
        }

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_a:
                    lInput = true;
                break;

                case SDLK_d:
                    rInput = true;
                break;

                case SDLK_w:
                    uInput = true;
                break;

                case SDLK_s:
                    dInput = true;
                break;
                case SDLK_SPACE:
                    spInput = true;
                break;
                case SDLK_ESCAPE:
                    //
                break;
            }
        }

        if (e->type == SDL_KEYUP) {
            switch (e->key.keysym.sym) {
                case SDLK_a:
                    lInput = false;
                break;

                case SDLK_d:
                    rInput = false;
                break;

                case SDLK_w:
                    uInput = false;
                break;

                case SDLK_s:
                    dInput = false;
                break;
                case SDLK_SPACE:
                    spInput = false;
                break;
                case SDLK_ESCAPE:

                    if (f_GameOver) {
                        set_next_state(STATE_MENU);
                    }
                    else {
                        if (f_paused){
                            playSound( -1, sfx_pauseOut, 0 );
                            f_paused = false;
                            delayTimer.unpause();

                            f_ShowInfo = false;
                        }
                        else{
                            playSound( -1, sfx_pauseIn, 0 );
                            f_paused = true;
                            delayTimer.pause();

                            infoTextTexture.loadFromRenderedText( updateText("Paused"), textColor);
                            f_ShowInfo = true;
                        }
                    }

                break;
            }
        }
        
    }

    // Main Game Loop logic flow
    void logic(){

        if ( !f_GameOver && !f_paused){

            // Player Input/Control Logic
            if (lInput)
                player.moveLeft();

            if (rInput)
                player.moveRight();

            // Register player dimensions
            SDL_Rect playerDim = player.getDim();

            if (lasers) {
                if (spInput) {
                    if ( laserA == NULL && laserB == NULL ){
                        playSound( -1, sfx_laserShot , 0 );
                        laserA = new Bullet(playerDim.x, playerDim.y);
                        laserB = new Bullet(playerDim.x + playerDim.w - BULLET_WIDTH, playerDim.y);
                    }
                }
            }

            if (laserA != NULL) {
                laserHandling(laserA);

                if (laserA->checkAlive() == false) {
                    delete laserA;
                    laserA = NULL;
                }
            }

            if (laserB != NULL) {
                laserHandling(laserB);

                if (laserB->checkAlive() == false) {
                    delete laserB;
                    laserB = NULL;
                }
            }

            // Create structure for ball dimensions
            SDL_Rect ballDim;
            
            for (int i = 0; i < balls.size(); i++){

                // Handle collisions between ball(s) and brick(s)/playing field
                hitDetection(balls[i]);

                // Ball Logic
                if (!f_LevelComplete){
                     balls[i]->move();
                }

                // Register ball dimensions
                ballDim = balls[i]->getDim();

                // Handle collision between ball and paddle

                if (player.checkCollision(ballDim) && balls[i]->getVel().y > 0){
                    // TODO: Can offsetPB be moved?
                    /*offsetPB = */balls[i]->setOffset(ballDim.x - playerDim.x);

                    int hitSpeed = ballDim.x + ballDim.w/2 - ( playerDim.x + playerDim.w/2);

                    // If the paddle is "sticky", stop the ball and enable the "stuck" routine below
                    if (catching){

                        if ( hitSpeed> 0)
                            balls[i]->storeVel( ( hitSpeed + (playerDim.w / paddleHitDiv) ) / (playerDim.w / paddleHitDiv) , -yLaunchVel );
                        else
                            balls[i]->storeVel( ( hitSpeed - (playerDim.w / paddleHitDiv) ) / (playerDim.w / paddleHitDiv) , -yLaunchVel );

                        playSound( -1, sfx_wallHit , 0 );
                        balls[i]->setVel(0,0);
                        balls[i]->setStuck(true);
                    }
                    // Otherwise, calculate the new horizontal trajectory of the ball
                    // based on the offset between the middle of the paddle and the point of collision.
                    // A larger offset results in a wider horizontal trajectory
                    else {
                        
                        // TODO: condense these lines to a "LaunchBall" function
                        //hitSpeed = ballDim.x - ( playerDim.x + playerDim.w/2);

                        playSound( -1, sfx_paddleHit , 0 );

                        if ( hitSpeed> 0)
                            balls[i]->setVel( ( hitSpeed + (playerDim.w / paddleHitDiv) ) / (playerDim.w / paddleHitDiv) , -yLaunchVel );
                        else
                            balls[i]->setVel( ( hitSpeed - (playerDim.w / paddleHitDiv) ) / (playerDim.w / paddleHitDiv) , -yLaunchVel );
                    }
                    
                }

                // Ball Stuck Logic
                if ( balls[i]->checkStuck() ){
                    
                    // Update ball position so its relative position on the paddle remains constant
                    balls[i]->setPos(playerDim.x + balls[i]->getOffset(), playerDim.y - ballDim.h );

                    // Release the ball if space input is asserted.
                    // Trajectory is calculated in a similar manner as a regular paddle collision
                    if (spInput){
                        // TODO: condense these lines to a "LaunchBall" function
                        playSound( -1, sfx_paddleHit , 0 );
                        balls[i]->releaseVel();
                        balls[i]->setStuck(false);
                    }
                }

                // Missed ball logic
                if  ( ballDim.y > SCREEN_HEIGHT + 10 ) {
                    
                    delete balls[i];
                    balls.erase(balls.begin()+i);

                    if (balls.size() == 0){
                        playSound( -1, sfx_lastBallMiss , 0 );

                        if (pickup != NULL){
                            delete pickup;
                            pickup = NULL;
                        }

                        if (!f_infiniteLives){
                            lives--;
                            livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
                        }

                        if (lives == 0){
                            infoTextTexture.loadFromRenderedText( updateText("Game Over"), textColor);
                            f_GameOver = true;
                        }
                        else {
                            balls.push_back(new Ball());
                            resetBall(balls[0]);

                            // Reset Paddle Width
                            SDL_Rect tempDim = player.getDim();
                            tempDim.w = PADDLE_WIDTH;
                            player.setDim(tempDim);

                            // Clear Powerups
                            piercing = false;
                            catching = false;
                            lasers = false;
                        }
                    }
                    else //balls.size() != 0
                        playSound( -1, sfx_ballMiss , 0 );
                }
            }

            // Pickup Logic
            if (pickup != NULL){

                bool f_PickupDelete = false;
                pickup->move();
                SDL_Rect pickupDim = pickup->getDim();
                SDL_Point tempVel;

                if (pickupDim.y + pickupDim.h > playerDim.y){
                    if (pickup->checkCollision(playerDim)){


                        switch (pickup->type){
                            case PICKUP_POINT:
                                
                                addPoints(100);
                                playSound( -1, sfx_pwrPnts , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Bonus Points"), textColor);
                            break;

                            // TODO: Fix catch handling for multiple balls
                            case PICKUP_CATCH:

                                catching = true;
                                lasers = false;
                                playSound( -1, sfx_pwrCatch , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Sticky Paddle"), textColor);
                            break;

                            case PICKUP_MULTI:
                                for (int i = balls.size() - 1; i >= 0 ; i--){
                                    ballDim = balls[i]->getDim();
                                    tempVel = balls[i]->getVel();
                                    if ( tempVel.x > 0)
                                        tempVel.x++;
                                    else if ( tempVel.x < 0)
                                        tempVel.x--;

                                    if ( tempVel.y > 0)
                                        tempVel.y++;
                                    else if ( tempVel.y < 0)
                                        tempVel.y--;

                                    // Why does the first multiball pickup spawn a ball stuck on the paddle?
                                    balls.push_back(new Ball(ballDim, tempVel));

                                    // How to assign offset and stuck status to balls spawned while on paddle?
                                    int j = balls.size()-1;
                                    if (tempVel.x == 0) {
                                        tempVel = balls[i]->getStoreVel();
                                        balls[j]->setOffset(balls[i]->getOffset());
                                        balls[j]->setStuck(true);
                                        tempVel.y -= 2;
                                        balls[j]->storeVel(tempVel.x, tempVel.y);
                                    }
                                }

                                playSound( -1, sfx_pwrMulti , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Multiball"), textColor);
                            break;

                            case PICKUP_PIERCE:
                                piercing = true;

                                playSound( -1, sfx_pwrPierce , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Piercing"), textColor);
                            break;

                            case PICKUP_SHOOT:
                                lasers = true;
                                catching = false;

                                playSound( -1, sfx_pwrShoot , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Lasers"), textColor);
                            break;

                            case PICKUP_GROW: // TODO: Interactions with sticky

                                if (playerDim.w < PADDLE_WIDTH + 2 * PADDLE_WIDTH_MOD){
                                    playerDim.w += PADDLE_WIDTH_MOD;
                                    playerDim.x -= PADDLE_WIDTH_MOD / 2;

                                    if (playerDim.x < 0)
                                        playerDim.x = 0;
                                    if (playerDim.x > SCREEN_WIDTH - playerDim.w)
                                        playerDim.x = SCREEN_WIDTH - playerDim.w;
                                    
                                    player.setDim(playerDim);
                                }

                                playSound( -1, sfx_pwrGrow , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Paddle Grow"), textColor);
                            break;

                            case PICKUP_SHRINK: // TODO: Interactions with sticky

                                if (playerDim.w > PADDLE_WIDTH - 2 * PADDLE_WIDTH_MOD){
                                    playerDim.w -= PADDLE_WIDTH_MOD;
                                    playerDim.x += PADDLE_WIDTH_MOD / 2;

                                    if (playerDim.x < 0)
                                        playerDim.x = 0;
                                    if (playerDim.x > SCREEN_WIDTH - playerDim.w)
                                        playerDim.x = SCREEN_WIDTH - playerDim.w;

                                    player.setDim(playerDim);

                                    for (int i = balls.size() - 1; i >= 0; i--) {
                                        if ( balls[i]->checkStuck() == true ) {
                                            ballDim = balls[i]->getDim();

                                            if (ballDim.x < playerDim.x)
                                                ballDim.x = playerDim.x;

                                            if (ballDim.x + ballDim.w > playerDim.x + playerDim.w)
                                                ballDim.x = playerDim.x + playerDim.w - ballDim.w;
                                            
                                            balls[i]->setDim(ballDim);
                                        }
                                    }
                                }

                                playSound( -1, sfx_pwrShrink , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Paddle Shrink"), textColor);
                            break;

                            case PICKUP_FAST:

                                speedUp();
                                playSound( -1, sfx_pwrSpeedUp , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Ball Speed Up"), textColor);

                            break;

                            case PICKUP_SLOW:

                                slowDown();
                                playSound( -1, sfx_pwrSpeedDown , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Ball Speed Down"), textColor);

                            break;

                            case PICKUP_LIFE:

                                if (!f_infiniteLives){
                                    lives++;
                                    playSound( -1, sfx_lifeUp , 0 );
                                    livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
                                }

                                infoTextTexture.loadFromRenderedText( updateText("Extra Life"), textColor);
                            break;

                        }

                        inR = pickup->r; inG = pickup->g; inB = pickup->b;
                        delayTimer.start();
                        f_ShowInfo = true;
                        f_InfoFade = true;
                        f_PickupDelete = true;
                    }
                }

                if (pickupDim.y > SCREEN_HEIGHT + 10)
                    f_PickupDelete = true;

                if (f_PickupDelete){
                    delete pickup;
                    pickup = NULL;
                }
            }
        }

        //--- Gamestate flags ---//

        if (f_ShowInfo){

            if (f_InfoFade) {
                tmpR = inR - (inR * delayTimer.getTicks() / DELAY_TIME);
                tmpB = inB - (inB * delayTimer.getTicks() / DELAY_TIME);
                tmpG = inG - (inG * delayTimer.getTicks() / DELAY_TIME);
            }
            else {
                tmpR = inR;
                tmpB = inB;
                tmpG = inG;
            }

            if (delayTimer.getTicks() > DELAY_TIME){
                delayTimer.stop();
                f_ShowInfo = false;
                f_InfoFade = false;
            }
        }

        if (f_LevelComplete){
            if (delayTimer.getTicks() > 2000){
                delayTimer.stop();
                f_LevelComplete = false;
                goNextLevel();
            }
        }

    }

    void render(){


        // Set background color and fill
        SDL_SetRenderDrawColor( gRenderer, bgR, bgG, bgB, 0xFF );
        SDL_RenderFillRect(gRenderer, &field);

        // Set scoreboard color and fill
        SDL_SetRenderDrawColor( gRenderer, scR, scG, scB, 0xFF );
        SDL_RenderFillRect(gRenderer, &scoreBoard);

        // Set sprite color
        SDL_SetRenderDrawColor( gRenderer, spR, spG, spB, 0xFF );
        player.render();

        for (int i = 0; i < balls.size(); i++)
            balls[i]->render();

        for (int i = 0; i < wall.size(); i++)
            wall[i]->render();

        if ( laserA != NULL)
            laserA->render();

        if ( laserB != NULL)
            laserB->render();

        if (pickup != NULL){
            SDL_SetRenderDrawColor( gRenderer, pickup->r, pickup->g, pickup->b, 0xFF );
            pickup->render();
        }

        // Update Text color and render
        // BUGNOTE: Whichever texture is rendered last causes all gRenderer entities to flicker when updated


        scoreTextTexture.setColor(spR, spG, spB);
        scoreTextTexture.render(5, 1 );


        livesTextTexture.setColor(spR, spG, spB);
        livesTextTexture.render(SCREEN_WIDTH - livesTextTexture.getWidth(), 1 );

        if (f_ShowInfo) {
            infoTextTexture.setColor(tmpR, tmpG, tmpB);
            infoTextTexture.render(SCREEN_WIDTH/2 - infoTextTexture.getWidth()/2, SCREEN_HEIGHT - infoTextTexture.getHeight() * 2 );
        }

        if (f_GameOver || f_paused) {
            infoTextTexture.setColor(spR, spG, spB);
            infoTextTexture.render(SCREEN_WIDTH/2 - infoTextTexture.getWidth()/2, SCREEN_HEIGHT - infoTextTexture.getHeight() * 2 );
        }

    }

};

#endif // GAMELOOP_H_INCLUDED