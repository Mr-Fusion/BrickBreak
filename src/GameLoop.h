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

#define XVEL_MIN            1
#define XVEL_MAX            8

#define SCORE_LIFEUP        5000

#define SCOREBOARD_WIDTH    SCREEN_WIDTH
#define SCOREBOARD_HEIGHT   60

#define DELAY_TIME          2400

class GameLoop : public GameState
{
    public:

    // Gameplay Stats/Values
    int currentLev = 0;
    int lives = 0;
    int score = 0;

    // Gameplay Event Flags
    bool f_ShowInfo = false;
    bool f_InfoFade = false;
    bool f_GameOver = false;
    bool f_LevelComplete = false;

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
    Mix_Chunk *alienShot = NULL;
    Mix_Chunk *playerShot = NULL;
    Mix_Chunk *alienHitA = NULL;
    Mix_Chunk *alienHitB = NULL;
    Mix_Chunk *playerHit = NULL;

    // Game Entities
    SDL_Rect field;
    SDL_Rect scoreBoard;
    Paddle player;
    Pickup *pickup = NULL;
    Bullet *laserA = NULL;
    Bullet *laserB = NULL;

    std::vector<Brick*> wall;
    std::vector<Ball*> balls;

    // Temp variables
    int offsetPB;

    int pickupRate = 0;
    int brickMap[NUM_BRICKS];
    SDL_Rect tempDim;

    ///Constructor Function
    GameLoop(){
        lInput = rInput = uInput = dInput = spInput = false;
        stuck = piercing = catching = lasers = false;

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

        for (int i = 0; i < balls.size(); i++){
            resetBall(balls[i]);
        }

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
            livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
            scoreTextTexture.loadFromRenderedText( updateText("Score: ", score), textColor);
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
        Mix_FreeChunk( playerShot );
        playerShot = NULL;
        Mix_FreeChunk( alienShot );
        alienShot = NULL;
        Mix_FreeChunk( alienHitA );
        alienHitA = NULL;
        Mix_FreeChunk( alienHitB );
        alienHitB = NULL;
        Mix_FreeChunk( playerHit );
        playerHit = NULL;

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
        result << "round" << level << ".csv";

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
        playerShot = Mix_LoadWAV( "../assets/sfx_player_shot.wav" );
        if( playerShot == NULL )
        {
            printf( "Failed to load player shot sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        alienShot = Mix_LoadWAV( "../assets/sfx_alien_shot.wav" );
        if( alienShot == NULL )
        {
            printf( "Failed to load alien shot sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        alienHitA = Mix_LoadWAV( "../assets/sfx_alien_hitA.wav" );
        if( alienHitA == NULL )
        {
            printf( "Failed to load alien hitA sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        alienHitB = Mix_LoadWAV( "../assets/sfx_alien_hitB.wav" );
        if( alienHitB == NULL )
        {
            printf( "Failed to load alien hitB sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        playerHit = Mix_LoadWAV( "../assets/sfx_player_hit.wav" );
        if( playerHit == NULL )
        {
            printf( "Failed to load player hit sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
            success = false;
        }

        //Render text
        livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);

        infoTextTexture.loadFromRenderedText( updateText("Round: ", currentLev), textColor);

        return success;
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
        CSVread(++currentLev);

        // Reset Paddle Width
        SDL_Rect tempDim = player.getDim();
        tempDim.w = PADDLE_WIDTH;
        player.setDim(tempDim);

        // Clear all balls
        for (int i = balls.size() - 1; i >= 0 ; i--){
            delete balls[i];
            balls.erase(balls.begin()+i);
        }

        // Serve new ball
        balls.push_back(new Ball());
        resetBall(balls[0]);



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

        thisBall->setStuck(true);
        //TODO: Revisit "Sticky" Functions
        tempDim = player.getDim();
        //offsetPB = tempDim.w/2;
        offsetPB = thisBall->setOffset(tempDim.w/2);
        thisBall->setPos(tempDim.x + offsetPB, tempDim.y - thisBall->getDim().h );
        piercing = false;
        catching = false;
        lasers = false;
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
            thisBall->vBounce();
        }

        // Right edge of field
        if ( ( ballDim.x > SCREEN_WIDTH - ballDim.w ) && ( ballVel.x > 0 ) ) {
            thisBall->hBounce();
        }

        // Left edge of field
        if ( ( ballDim.x < 0 ) && ( ballVel.x < 0 ) ) {
            thisBall->hBounce();
        }

    }

    void wallHit(int index){

        SDL_Rect brickDim;

        switch (wall[index]->getType()){
            case BRICK_GRAY:
                wall[index]->setType(BRICK_WHITE);
                break;
            case BRICK_DARK:
                wall[index]->setType(BRICK_GRAY);
                break;
            default:
                brickDim = wall[index]->getDim();
                delete wall[index];
                wall.erase(wall.begin()+index);
                addPoints(10);

                // Roll for spawning a pickup from eliminated brick
                // If the roll is unsuccessful, increase the likelihood for next time
                if ( ( rand() % pickupRate ) >= 10){
                    if (pickup == NULL){
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
        int tempScore = score;
        score += p;

        // If score increments past SCORE_LIFEUP threshold, add an extra live
        if ( (score / SCORE_LIFEUP) > (tempScore / SCORE_LIFEUP) ){
            lives++;
            livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);
        }

        // Update Score Display texture
        scoreTextTexture.loadFromRenderedText( updateText("Score: ", score), textColor);
    }

    ///Handles Player input
    void handleEvent( SDL_Event* e){

        int x, y;

        //Get mouse position
        if( e->type == SDL_MOUSEMOTION ){
            SDL_GetMouseState( &x, &y );
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
                    set_next_state(STATE_MENU);
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
            }
        }
        
    }

    // Main Game Loop logic flow
    void logic(){

        if (!f_GameOver){

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

                    int hitSpeed = ballDim.x - ( playerDim.x + playerDim.w/2);

                    // If the paddle is "sticky", stop the ball and enable the "stuck" routine below
                    if (catching){

                        if ( hitSpeed> 0)
                            balls[i]->storeVel( ( hitSpeed + (playerDim.w / PADDLE_HIT_DIVIDER) ) / (playerDim.w / PADDLE_HIT_DIVIDER) , -balls[i]->vel.y);
                        else
                            balls[i]->storeVel( ( hitSpeed - (playerDim.w / PADDLE_HIT_DIVIDER) ) / (playerDim.w / PADDLE_HIT_DIVIDER) , -balls[i]->vel.y);

                        balls[i]->setVel(0,0);
                        balls[i]->setStuck(true);
                    }
                    // Otherwise, calculate the new horizontal trajectory of the ball
                    // based on the offset between the middle of the paddle and the point of collision.
                    // A larger offset results in a wider horizontal trajectory
                    else {
                        
                        // TODO: condense these lines to a "LaunchBall" function
                        //hitSpeed = ballDim.x - ( playerDim.x + playerDim.w/2);

                        if ( hitSpeed> 0)
                            balls[i]->setVel( ( hitSpeed + (playerDim.w / PADDLE_HIT_DIVIDER) ) / (playerDim.w / PADDLE_HIT_DIVIDER) , -balls[i]->vel.y);
                        else
                            balls[i]->setVel( ( hitSpeed - (playerDim.w / PADDLE_HIT_DIVIDER) ) / (playerDim.w / PADDLE_HIT_DIVIDER) , -balls[i]->vel.y);
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
                        balls[i]->releaseVel();
                        balls[i]->setStuck(false);
                    }
                }

                // Missed ball logic
                if  ( ballDim.y > SCREEN_HEIGHT + 10 ) {
                    
                    delete balls[i];
                    balls.erase(balls.begin()+i);

                    if (balls.size() == 0){

                        lives--;
                        livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);

                        if (lives == 0){
                            infoTextTexture.loadFromRenderedText( updateText("Game Over"), textColor);
                            f_GameOver = true;
                        }
                        else {
                            balls.push_back(new Ball());
                            resetBall(balls[0]);
                        }
                    }
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

                                infoTextTexture.loadFromRenderedText( updateText("Bonus Points"), textColor);
                            break;

                            // TODO: Fix catch handling for multiple balls
                            case PICKUP_CATCH:
                                catching = true;
                                lasers = false;

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

                                infoTextTexture.loadFromRenderedText( updateText("Multiball"), textColor);
                            break;

                            case PICKUP_PIERCE:
                                piercing = true;

                                infoTextTexture.loadFromRenderedText( updateText("Piercing"), textColor);
                            break;

                            case PICKUP_SHOOT:
                                lasers = true;
                                catching = false;

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
                                }

                                infoTextTexture.loadFromRenderedText( updateText("Paddle Shrink"), textColor);
                            break;

                            case PICKUP_FAST:
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
                                infoTextTexture.loadFromRenderedText( updateText("Ball Speed Up"), textColor);
                            break;

                            case PICKUP_SLOW:
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
                                infoTextTexture.loadFromRenderedText( updateText("Ball Speed Down"), textColor);
                            break;

                            case PICKUP_LIFE:
                                lives++;
                                livesTextTexture.loadFromRenderedText( updateText("Lives: ", lives), textColor);

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

        if (f_GameOver) {
            infoTextTexture.setColor(spR, spG, spB);
            infoTextTexture.render(SCREEN_WIDTH/2 - infoTextTexture.getWidth()/2, SCREEN_HEIGHT - infoTextTexture.getHeight() * 2 );
        }

    }

};

#endif // GAMELOOP_H_INCLUDED