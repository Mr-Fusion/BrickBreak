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
#define DEFAULT_LUCK        8

#define NUM_ROWS            18
#define NUM_COLS            11
#define NUM_BRICKS          NUM_ROWS * NUM_COLS

#define PADDLE_WIDTH_MOD    20
#define PADDLE_HIT_DIVIDER  6

#define SPEED_INDEX_MAX     6
#define SPEED_INDEX_MIN     0
#define SPEED_INDEX_DEFAULT 3

#define BASE_POINTS         10
#define SCORE_LIFEUP_BASE   20000

#define SCOREBOARD_WIDTH    SCREEN_WIDTH
#define SCOREBOARD_HEIGHT   60

#define MAX_LEVEL           34
#define DELAY_TIME          2400

class GameLoop : public GameState
{
    public:

    // Gameplay Stats/Values
    int currentLev = 0;
    int lives = DEFAULT_LIVES;
    int score = 0;
    int hitCount = 0;
    int pickupRate = DEFAULT_LUCK;
    int baseSpeed = SPEED_INDEX_DEFAULT;
    int speedIndex = baseSpeed;
    int paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;;
    int speedupThresh = 15;
    int yLaunchVel = BALL_VELOCITY + speedIndex;
    int lifeUpThresh = SCORE_LIFEUP_BASE;

    // Gameplay Event Flags
    bool f_ShowInfo = false;
    bool f_InfoFade = false;
    bool f_GameOver = false;
    bool f_LevelComplete = false;
    bool f_paused = false;
    bool f_newServe = false;

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
    int brickMap[NUM_BRICKS];
    SDL_Rect tempDim;

    ///Constructor Function
    GameLoop(settings s){

        ///---Cursor, Player Input, and Powerup Initialization---///
        SDL_ShowCursor(SDL_DISABLE);
        lInput = rInput = uInput = dInput = spInput = false;
        stuck = piercing = catching = lasers = false;

        ///---Game Settings Initialization---///
        f_soundEnable = s.sfxEnable;

        if (s.difficulty == DIFFY_FREE){
            f_scoreEnable = false;
            f_infiniteLives = true;
            f_keepPwrUps = true;
        }

        if (s.difficulty == DIFFY_EASY){
            baseSpeed = 2;
            speedupThresh = 20;
            f_keepPwrUps = true;
        }

        if (s.shuffleEnable)
            f_shuffleLvls = true;

        if (s.multiEnable)
            f_multiMode = true;

        ///---Playfield, Level, and Ball Initialization---///
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
        
        ///---Shuffle the level order if enabled---///
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

        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Render text for scoreboard HUD
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

        //Free loaded images
        livesTextTexture.free();
        infoTextTexture.free();
        scoreTextTexture.free();

        //delete all pointers and vectors and clear all elements
        for (int i = wall.size() - 1; i >= 0; i--)
            delete wall[i];
        wall.clear();

        for (int i = balls.size() - 1; i >= 0; i--)
            delete balls[i];
        balls.clear();

        if (laserA != NULL)
            delete laserA;

        if (laserB != NULL)
            delete laserB;
    }

    // Function to read in level data from a CSV file and store the values in an array.
    // Levels in Brickbreaker are saved as CSV files in the assets subdirectory, named "roundX"
    // where X is an integer corresponding to the numerical ordering of the level.
    // Each value within the CSV file is expected to be an integer between 0 and 9,
    // corresponding to the BRICK_TYPE enumeration within Brick.h.
    // The spatial arrangement of each integer within the CSV file
    // corresponds with the desired layout of the bricks in the level to be loaded.
    // TODO: This function should be more flexible and robust. Look into exception and graceful error handling.
    void CSVread(int level)
    {
        std::ifstream fin;
        std::string line;
        std::stringstream result;

        // Construct the string of the filepath to be loaded, based on the "level" integer input
        result.str( "" );
        result << "../assets/round" << level << ".csv";

        // Open the specified file
        fin.open(result.str());

        // Clear "result" stream in preparation for reading/parsing the file
        result.clear();
        result.str( "" );
        
        // Read each line of the CSV as a string from the input stream
        // and write it to the "result" stream.
        // Each line is separated by spaces.
        while(!fin.eof()){
            fin>>line;
            result<<line<<" ";
        }

        // Append null terminator to stringstream and copy the result into myString
        result<<"\0";
        std::string myString = result.str();

        // Iterate through mystring and convert every character between '0' and '9' into an integer.
        // Store each integer in the brickmap array to be used in level loading
        int i = 0;
        int j = 0;
        char myChar = NULL;
        while (myString[i] != '\0'){
            if (myString[i] >= '0' && myString[i] <= '9'){
                myChar = myString[i];
                brickMap[j] = myChar - 48; 
                j++;
            }
            i++;
        }
    }

    // Function which streamlines the loading of sound chunks.
    bool loadSound(Mix_Chunk **chunk, std::string path){
        bool success = true;
        *chunk = Mix_LoadWAV(path.c_str());
        if ( chunk == NULL) {
            printf( "Failed to load %s! SDL_mixer Error: %s\n", path, Mix_GetError() );
            success = false;
        }
        return success;
    }

    bool loadMedia()
    {
        //Loading success flag
        bool success = true;

        //Load sound effects
        if (f_soundEnable == true) {
            if ( !loadSound(&sfx_pwrPnts,"../assets/sfx_coin_double4.wav" ) )
                success = false;
            if ( !loadSound(&sfx_ballMiss, "../assets/sfx_damage_hit3.wav" ) )
                success = false;
            if ( !loadSound(&sfx_lastBallMiss, "../assets/sfx_exp_various6.wav" ) )
                success = false;
            if ( !loadSound(&sfx_brickHit, "../assets/sfx_sounds_Blip2.wav" ) )
                success = false;
            if ( !loadSound(&sfx_wallHit, "../assets/sfx_sounds_Blip7.wav" ) )
                success = false;
            if ( !loadSound(&sfx_paddleHit, "../assets/sfx_sounds_Blip9.wav" ) )
                success = false;
            if ( !loadSound(&sfx_brickDestroy, "../assets/sfx_sounds_Blip7.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrShrink, "../assets/sfx_shift_down.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pauseIn, "../assets/sfx_sounds_pause1_in.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pauseOut, "../assets/sfx_sounds_pause1_out.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrPierce, "../assets/sfx_shift_up.wav" ) )
                success = false;
            if ( !loadSound(&sfx_laserShot, "../assets/sfx_laser.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrSpeedUp, "../assets/sfx_speed_up.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrSpeedDown, "../assets/sfx_speed_down.wav" ) )
                success = false;
            if ( !loadSound(&sfx_lifeUp, "../assets/sfx_life_up.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrCatch, "../assets/sfx_catch.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrGrow, "../assets/sfx_grow.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrShoot, "../assets/sfx_shoot.wav" ) )
                success = false;
            if ( !loadSound(&sfx_pwrMulti, "../assets/sfx_multi.wav" ) )
                success = false;
        }

        //Render initial text
        livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
        infoTextTexture.loadFromRenderedText( updateText("Round: ", currentLev), textColor);

        return success;
    }

    // Generic function for playing SDL_Mixer sound chunks
    void playSound(int channel, Mix_Chunk *sound, int loops){
        if (sound != NULL)
            Mix_PlayChannel( channel, sound , loops );
    }

    // Generic function for updating text strings with integers potentially appended at the end
    std::string updateText(std::string text, int num = -1){
        std::stringstream result;

        result.str("");
        result << text;

        if (num != -1)
            result << num;

        return result.str();
    }

    // Function routine for transitioning to next level
    void goNextLevel(){

        // Clear all balls
        for (int i = balls.size() - 1; i >= 0 ; i--){
            delete balls[i];
            balls.erase(balls.begin()+i);
        }

        // Serve new ball
        balls.push_back(new Ball());
        resetBall(balls[0]);

        // Reset all powerups unless defined otherwise in settings
        if (!f_keepPwrUps){
            piercing = false;
            catching = false;
            lasers = false;
        }

        // Load the next level and then increment "currentLev" for next time.
        // If the value of the next level exceeds max value, reset the value to 0 before loading.
        if (currentLev > MAX_LEVEL)
            currentLev = 0;
        CSVread(levelArray[currentLev]);
        currentLev++;

        // Initialize a vector of brick objects as a formation specified by CSVRead function
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

    // Function for initializing first ball after all balls are lost or level transitions
    void resetBall(Ball *thisBall){

        // Reset speed index and hit count for new ball/life
        hitCount = 0;
        speedIndex = baseSpeed;

        // Use the new index value to calculate/reset paddle hit divider and launch velocity
        paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;
        yLaunchVel  = BALL_VELOCITY + speedIndex;

        // Store the launch velocity and assign "stucky" status to ball for initial serve
        thisBall->storeVel(yLaunchVel/4,-yLaunchVel);
        thisBall->setStuck(true);

        // Obtain the current location of the paddle and set the ball's position at the middle
        tempDim = player.getDim();
        int offsetPB = thisBall->setOffset(tempDim.w/2);
        thisBall->setPos(tempDim.x + offsetPB, tempDim.y - thisBall->getDim().h );
    }

    // Function to handle hit detection between a ball specified by a pointer, and all bricks on the playing field
    void hitDetection(Ball *thisBall){

        // Temp Variables used for hit detection calculations
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

                // Increment total Hit Count for other purposes such as gradual speed increase
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

    // Function for resolving outcome of a specified brick on the playing field being hit.
    // Can be called for both ball and laser collisions
    void wallHit(int index){

        SDL_Rect brickDim;
        bool destroy = false;

        // Handler routine conditional on the type of brick which was hit.
        // Dark and grey bricks take "damage" by shifting to a lighter variant
        // All other bricks are marked for distruction on impact.
        // If the piercing powerup is active, the brick is marked to be destroyed
        // and points are awarded based on how many hits the brick type could sustain.
        switch (wall[index]->getType()){
            case BRICK_GRAY:
                if (piercing){
                    playSound( -1, sfx_brickDestroy , 0 );
                    destroy = true;
                    addPoints(BASE_POINTS * speedIndex * 2);
                }
                else {
                    playSound( -1, sfx_brickHit , 0 );
                    wall[index]->setType(BRICK_WHITE);
                    addPoints(BASE_POINTS * speedIndex);
                }
            break;
            case BRICK_DARK:
                if (piercing){
                    playSound( -1, sfx_brickDestroy , 0 );
                    destroy = true;
                    addPoints(BASE_POINTS * speedIndex * 3);
                }
                else {
                    playSound( -1, sfx_brickHit , 0 );
                    wall[index]->setType(BRICK_GRAY);
                    addPoints(BASE_POINTS * speedIndex);
                }
            break;
            default:
                playSound( -1, sfx_brickDestroy , 0 );
                destroy = true;
                addPoints(BASE_POINTS * speedIndex);
            break;
        }

        // Routine to handle brick destruction. Brick dimensions are saved for pickup spawning
        if (destroy){
            brickDim = wall[index]->getDim();
            delete wall[index];
            wall.erase(wall.begin()+index);


            // Roll for spawning a pickup from eliminated brick
            // If the roll is successful, spawn the pickup and randomly determine next initial pickup rate
            // If the roll is unsuccessful, gradually increase the likelihood for next time
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

    // Function for handling full behavior pattern of a laser object.
    void laserHandling(Bullet *laser){

        // Update the laser's position and register its new dimensions
        laser->move();
        SDL_Rect laserDim = laser->getDim();

        // Check if the laser collided with any of the bricks on the field
        for (int i = wall.size()-1; i >= 0 ; i--){
            if (wall[i]->checkCollision(laserDim) == true) {

                // If a collision occurs ,resolve outcome for the brick depending on its type
                wallHit(i);

                // Mark the laser for destruction unless the piercing power up is active
                if (!piercing)
                    laser->setAlive( false );
            }
        }
        // Lasers are always marked for destruction upon leaving the field
        if (laserDim.y + laserDim.h < SCOREBOARD_HEIGHT)
            laser->setAlive( false );

    }

    // Function which handles the minutiae associated with scoring
    void addPoints(int p){

        // Do not bother with this function if scoring is disabled in options
        if (!f_scoreEnable)
            return;

        int tempScore = score;
        score += p;

        // Update Score Display texture
        scoreTextTexture.loadFromRenderedText( updateText("Score: ", score), textColor);

        // Do not bother with extra lives if infinite lives are enabled
        if (f_infiniteLives)
            return;

        // If score increments past lifeUpThresh threshold, add an extra life
        if ( (score / lifeUpThresh) > (tempScore / lifeUpThresh) ){
            lives++;
            playSound( -1, sfx_lifeUp , 0 );
            livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
        }
    }

    // Function for incrementing and tracking hit count based on a specified integer value
    void hitTracker(int h){
        int tempCount = hitCount;
        hitCount += h;

        // If score increments past speedup threshold, increase speed
        if ( (hitCount / speedupThresh) > (tempCount / speedupThresh) )
            speedUp();
    }

    // Function for increasing speed of all ball objects
    void speedUp(){
        SDL_Point tempVel;

        // Increase the velocity of all active balls
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

        // Increase the speed index and update paddle hit divider and launch Y-Velocity.
        // This will ultimately increase the speed of balls which may be stuck to the paddle.
        if (speedIndex < SPEED_INDEX_MAX)
            speedIndex++;

        paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;
        yLaunchVel  = BALL_VELOCITY + speedIndex;
    }

    // Function for decreasing speed of all ball objects
    void slowDown(){
        SDL_Point tempVel;

        // Decrease the velocity of all active balls
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

        // Decrease the speed index and update paddle hit divider and launch Y-Velocity.
        // This will ultimately decrease the speed of balls which may be stuck to the paddle.
        if (speedIndex > SPEED_INDEX_MIN)
            speedIndex--;

        paddleHitDiv = PADDLE_HIT_DIVIDER + speedIndex * 2;
        yLaunchVel  = BALL_VELOCITY + speedIndex;
    }

    ///Handles Player input
    void handleEvent( SDL_Event* e){

        int x, y;
        SDL_Rect tempDim = player.getDim();

        //Get mouse position
        if( e->type == SDL_MOUSEMOTION ){
            if (!f_paused){
                SDL_GetMouseState( &x, &y );
                tempDim.x = x - tempDim.w/2;

                // Set the-x position of the paddle such that it is centered on the mouse's x-position,
                // but prohibit any part of the paddle from travelling beyond the dimensions of the screen.
                if ( x > SCREEN_WIDTH - tempDim.w/2 )
                    tempDim.x = SCREEN_WIDTH - tempDim.w;
                if ( x < tempDim.w/2 )
                    tempDim.x = 0;

                player.setDim(tempDim);
            }
        }

        // Left mouse button controls action input
        if( e->button.button == SDL_BUTTON_LEFT && e->type == SDL_MOUSEBUTTONDOWN ){
            spInput = true;
        }
        if( e->button.button == SDL_BUTTON_LEFT && e->type == SDL_MOUSEBUTTONUP ){
            spInput = false;
        }

        // Escape key pauses and unpauses the game.
        // Only triggers on key release to prevent accidental pause toggling.
        if (e->type == SDL_KEYUP) {
            switch (e->key.keysym.sym) {

                case SDLK_ESCAPE:
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

                    if (f_GameOver)
                        set_next_state(STATE_MENU);
                break;

                // Backspace ends the game and returns to main menu
                case SDLK_BACKSPACE:
                    set_next_state(STATE_MENU);
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

            // If laser powerup is active, the action input is pressed, and no other lasers exist,
            // Two lasers are spawned from each end of the paddle.
            if (lasers) {
                if (spInput) {
                    if ( laserA == NULL && laserB == NULL ){
                        playSound( -1, sfx_laserShot , 0 );
                        laserA = new Bullet(playerDim.x, playerDim.y);
                        laserB = new Bullet(playerDim.x + playerDim.w - BULLET_WIDTH, playerDim.y);
                    }
                }
            }

            // Laser handling routines for both lasers. Destroy either laser if they are marked for destruction
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

            // Create structures for ball dimensions and hit velocity
            SDL_Rect ballDim;
            SDL_Point hitVel;
            
            // Evaluate logic for each active ball
            for (int i = 0; i < balls.size(); i++){

                // Handle collisions between ball(s) and brick(s)/playing field
                hitDetection(balls[i]);

                // Update ball position
                if (!f_LevelComplete){
                     balls[i]->move();
                }

                // Register ball dimensions
                ballDim = balls[i]->getDim();

                // Handle collision between ball and paddle
                if (player.checkCollision(ballDim) && balls[i]->getVel().y > 0){

                    // Register the relative X-offset of where the ball collided with the paddle
                    balls[i]->setOffset(ballDim.x - playerDim.x);

                    // Calculate the offset between the center of the ball and center of the paddle during collision.
                    int hitSpeed = ballDim.x + ballDim.w/2 - ( playerDim.x + playerDim.w/2);

                    // Calculate horizontal velocity as a function of the hitspeed and the width/hitDivision of the paddle
                    if ( hitSpeed> 0)
                        hitVel.x = ( hitSpeed + (playerDim.w / paddleHitDiv) ) / ( playerDim.w / paddleHitDiv );
                    else
                        hitVel.x = ( hitSpeed - (playerDim.w / paddleHitDiv) ) / ( playerDim.w / paddleHitDiv );

                    // Vertical velocity is relatively fixed based on the speed index.
                    hitVel.y = -yLaunchVel;

                    // If "catching" power up is enabled, stop the ball, calculate and store and mark it as stuck
                    if (catching){
                        balls[i]->setVel(0,0);      
                        balls[i]->storeVel( hitVel.x, hitVel.y);
                        balls[i]->setStuck(true);
                        playSound( -1, sfx_wallHit , 0 );
                    }
                    // Otherwise, simply update the ball with the new velocity
                    else {
                        balls[i]->setVel( hitVel.x, hitVel.y);
                        playSound( -1, sfx_paddleHit , 0 );
                    }
                }

                // Ball Stuck Logic
                if ( balls[i]->checkStuck() ){
                    
                    // Update ball position so its relative position on the paddle remains constant
                    balls[i]->setPos(playerDim.x + balls[i]->getOffset(), playerDim.y - ballDim.h );

                    // Release the ball if space input is asserted.
                    if (spInput){
                        balls[i]->releaseVel();
                        balls[i]->setStuck(false);
                        playSound( -1, sfx_paddleHit , 0 );
                    }
                }

                // Missed ball logic. If it trevels of the bottom of the screen:
                if  ( ballDim.y > SCREEN_HEIGHT + 10 ) {
                    
                    // Delete a ball and remove it from the ball vector
                    delete balls[i];
                    balls.erase(balls.begin()+i);

                    // If there are no remaining balls in the vector
                    if (balls.size() == 0){

                        // Play a harsh penalty SFX
                        playSound( -1, sfx_lastBallMiss , 0 );

                        // Clear the playing field of any pickups which may be active
                        if (pickup != NULL){
                            delete pickup;
                            pickup = NULL;
                        }

                        // Hide any info messages which may be displayed, and reset the delayTimer.
                        f_ShowInfo = false;
                        f_InfoFade = false;
                        delayTimer.stop();

                        // Remove one life and update the lives HUD text
                        if (!f_infiniteLives){
                            lives--;
                            livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
                        }

                        // If there are no remaining lives, the game is over
                        if (lives == 0){
                            infoTextTexture.loadFromRenderedText( updateText("Game Over"), textColor);
                            f_GameOver = true;
                        }
                        // Otherwise, prepare to serve the next ball
                        else {
                            infoTextTexture.loadFromRenderedText( updateText("Get Ready..."), textColor);
                            f_newServe = true;
                            f_ShowInfo = true;
                            delayTimer.start();

                            // Reset Paddle Width and hit division properties
                            SDL_Rect tempDim = player.getDim();
                            tempDim.w = PADDLE_WIDTH;
                            player.setDim(tempDim);

                            // Clear all powerup effects
                            piercing = false;
                            catching = false;
                            lasers = false;
                        }
                    }
                    else // If there are other active balls when one is missed, play a lighter penalty SFX
                        playSound( -1, sfx_ballMiss , 0 );
                }
            }

            // Pickup Logic
            if (pickup != NULL){

                // Update pickup position and register its dimensions.
                pickup->move();
                SDL_Rect pickupDim = pickup->getDim();
                bool f_PickupDelete = false;

                if (pickupDim.y + pickupDim.h > playerDim.y){
                    if (pickup->checkCollision(playerDim)){

                        // If the player touches the pickup, evaluate its type and affect the game accordingly
                        // All pickups play a unique sound effect, and display a color-coded info message when collected
                        switch (pickup->type){
                            
                            case PICKUP_POINT:  // Add some bonus points to the score total
                                addPoints(BASE_POINTS * speedIndex * 10);
                                playSound( -1, sfx_pwrPnts , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Bonus Points"), textColor);
                            break;

                            case PICKUP_CATCH:  // Balls stick to the paddle on impact. Action button to release. Incompatible with lasers.
                                catching = true;
                                lasers = false;
                                playSound( -1, sfx_pwrCatch , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Sticky Paddle"), textColor);
                            break;

                            case PICKUP_MULTI:  // All active balls spawn a duplicate ball with a slightly different trajectory

                                SDL_Point tempVel;

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

                                    balls.push_back(new Ball(ballDim, tempVel));

                                    // Balls which are stuck to the paddle spawn duplicates wwhich are also stuck at the same offset.
                                    // However, the stored velocity for launch will be slightly different.
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

                            case PICKUP_PIERCE: // Ball and laser objects are not affected when colliding with bricks
                                piercing = true;
                                playSound( -1, sfx_pwrPierce , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Piercing"), textColor);
                            break;

                            case PICKUP_SHOOT:  // Enables twin lasers to be fired from each end of the paddle using the action input
                                lasers = true;
                                catching = false;
                                playSound( -1, sfx_pwrShoot , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Lasers"), textColor);
                            break;

                            case PICKUP_GROW: // Paddle size increases by a set amount. Can only grow twice above default size.

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

                            case PICKUP_SHRINK: // Paddle size decreases by a set amount. Can only shrink twice below default size.

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

                            case PICKUP_FAST:   // All balls increase in speed. Paddle hit trajectory is also affected
                                speedUp();
                                playSound( -1, sfx_pwrSpeedUp , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Ball Speed Up"), textColor);
                            break;

                            case PICKUP_SLOW:   // All balls decrease in speed. Paddle hit trajectory is also affected
                                slowDown();
                                playSound( -1, sfx_pwrSpeedDown , 0 );
                                infoTextTexture.loadFromRenderedText( updateText("Ball Speed Down"), textColor);
                            break;

                            case PICKUP_LIFE:   // Adds an extra life to the total. Does nothing if infinite lives are enabled

                                if (!f_infiniteLives){
                                    lives++;
                                    playSound( -1, sfx_lifeUp , 0 );
                                    livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
                                }
                                else
                                    addPoints(BASE_POINTS * 100);

                                infoTextTexture.loadFromRenderedText( updateText("Extra Life"), textColor);
                            break;

                        }

                        // Display the info message and set its color to associate with the collected powerup. Mark pickup for deletion.
                        inR = pickup->r; inG = pickup->g; inB = pickup->b;
                        delayTimer.start();
                        f_ShowInfo = true;
                        f_InfoFade = true;
                        f_PickupDelete = true;
                    }
                }

                // Mark pickup for deletion if it leaves the bottom of the screen
                if (pickupDim.y > SCREEN_HEIGHT + 10)
                    f_PickupDelete = true;

                // Delete the pickup is marked for deletion
                if (f_PickupDelete){
                    delete pickup;
                    pickup = NULL;
                }
            }
        }

        //--- Gamestate flags ---//

        if (f_newServe){
            if (delayTimer.getTicks() > DELAY_TIME/2){
                infoTextTexture.loadFromRenderedText( updateText("Serve!"), textColor);
                f_newServe = false;
                balls.push_back(new Ball());
                resetBall(balls[0]);
            }
        }

        if (f_ShowInfo){

            if (f_InfoFade) {
                tmpR = inR - (inR * delayTimer.getTicks() / DELAY_TIME);
                tmpB = inB - (inB * delayTimer.getTicks() / DELAY_TIME);
                tmpG = inG - (inG * delayTimer.getTicks() / DELAY_TIME);
            }
            else {
                tmpR = spR;
                tmpB = spB;
                tmpG = spG;
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

        // Set sprite color and render sprites
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

            if (pickup->type == PICKUP_LIFE)
                pickup->randomizeColor();

            SDL_SetRenderDrawColor( gRenderer, pickup->r, pickup->g, pickup->b, 0xFF );
            pickup->render();
        }

        scoreTextTexture.setColor(spR, spG, spB);
        scoreTextTexture.render(5, 1 );

        // BUGNOTE: Whichever text texture is rendered last causes all gRenderer entities to flicker when updated
        livesTextTexture.setColor(spR, spG, spB);
        livesTextTexture.render(SCREEN_WIDTH - livesTextTexture.getWidth(), 1 );

        if (f_ShowInfo) {   // Enabled when displaying the current level or powerup info
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