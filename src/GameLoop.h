#ifndef GAMELOOP_H_INCLUDED
#define GAMELOOP_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include "Const.h"
#include "GameState.h"
#include "LTimer.h"
#include "LTexture.h"
#include "Paddle.h"
#include "Ball.h"
#include "Brick.h"

#define DEFAULT_LIVES       3
#define NUM_BRICKS          60
#define NUM_ROWS            6
#define NUM_COLS            10
#define HOR_OFFSET          SCREEN_WIDTH/8
#define VER_OFFSET          SCREEN_HEIGHT/8

class GameLoop : public GameState
{
    public:

    // Gameplay Stats/Values
    int currentLev;
    int lives;
    int score;

    // Gameplay Event Flags
    bool levelBegin;
    bool showLvl;
    bool gameOver;

    // Player Input Control Flags
    bool lInput,rInput,uInput,dInput,spInput;

    SDL_Color textColor;

    // Background color
    int bgR, bgG, bgB;

    // Sprite color
    int spR, spG, spB;

    // In memory text stream
    std::stringstream msgText;
    std::stringstream lvlText;
    std::stringstream livesText;
    std::stringstream scoreText;

    // Scene textures
    LTexture livesTextTexture;
    LTexture lvlTextTexture;
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
    Paddle player;
    Ball ball;
    //Brick brick;

    std::vector<Brick*> wall;

    // Temp variables
    int offsetPB;
    bool stickyP, stuck, piercing;
    int hitSpeed;

    ///Constructor Function
    GameLoop(){

        currentLev = 0;

        showLvl = false;
        gameOver = false;

        bgR = bgG = bgB = 0x00;

        spR = spG = spB = 0xFF;

        textColor = { spR, spG, spB, 0xFF};

        field.x = 0;
        field.y = 0;
        field.w = SCREEN_WIDTH;
        field.h = SCREEN_HEIGHT;

        lInput = rInput = uInput = dInput = false;

        lives = 0;
        score = 0;

        stickyP = true;
        stuck = true;
        offsetPB = player.getDim().w/2;
        piercing = false;

        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
            //Initialize playing field dimensions, difficulty, and appearance
            startGame();

            //Initialize and display graphical interface
            SDL_SetWindowSize(gWindow,SCREEN_WIDTH,SCREEN_HEIGHT);
        }

    }

    ///Deconstructor
    ~GameLoop(){
        printf("Gamestate Object Deconstructing...\n");

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
        lvlTextTexture.free();
        scoreTextTexture.free();
    }

    void startGame()
    {
        //Initialization goes here
        lives = DEFAULT_LIVES;
        score = 0;
        updateLivesText();
        updateScoreText();
        goNextLevel();
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

        //Set text to be rendered
        msgText.str( " " );
        livesText.str( " " );

        //Render text
        livesTextTexture.setText(livesText.str().c_str(), textColor);

        lvlTextTexture.setText(msgText.str().c_str(), textColor);

        return success;
    }

    void updateLvlText(int num){
        //Set text to be rendered
        lvlText.str( "" );
        lvlText << "Mission: " << num;

        //Render text
        lvlTextTexture.setText(lvlText.str().c_str(), textColor);

    }

    void updateLivesText() {
        //Set text to be rendered
        livesText.str( "" );
        livesText << "Lives: " << lives;

        //Render text
        livesTextTexture.setText(livesText.str().c_str(), textColor);
    }

    void updateScoreText() {
        //Set text to be rendered
        scoreText.str( "" );
        scoreText << "Score: " << score;

        //Render text
        scoreTextTexture.setText(scoreText.str().c_str(), textColor);
    }

    void goNextLevel(){

        // Prepare for next level here
        currentLev++;
        initLevel();
    }

    void initLevel() {

        for (int i = 0; i < NUM_COLS; i++){
            for (int j = 0; j < NUM_ROWS; j++)
                wall.push_back(new Brick(HOR_OFFSET + BRICK_WIDTH * i, VER_OFFSET + BRICK_HEIGHT * j));
        }

        // Update level string and display text on screen
        updateLvlText(currentLev);
        showLvl = true;

        // Set countdown to hide level text some time after level begins
        delayTimer.start();
        levelBegin = true;

    }

    void hitDetection(){

        // Temp Variables
        SDL_Point   ballVel = ball.getVel();
        SDL_Rect    ballNextPos = ball.getDim();
        int         hitCntX, hitCntY;

        hitCntX = hitCntY = 0;
        ballNextPos.x += ballVel.x;
        ballNextPos.y += ballVel.y;

        for (int i = wall.size()-1; i >= 0 ; i--){

            if (wall[i]->checkCollision(ballNextPos) == true){
                if (ballVel.x >= 0){
                    if ( ball.checkCollE(wall[i]->getDim()) == false )
                        hitCntX++;
                }
                else {
                    if ( ball.checkCollW(wall[i]->getDim()) == false )
                        hitCntX++;
                }

                if (ballVel.y >= 0){
                    if ( ball.checkCollS(wall[i]->getDim()) == false )
                        hitCntY++;
                }
                else {
                    if ( ball.checkCollN(wall[i]->getDim()) == false )
                        hitCntY++;
                }
                delete wall[i];
                wall.erase(wall.begin()+i);
                score += 10;
            }

        }

        if (!piercing){
            if (hitCntX > 0 || hitCntY > 0){
                if (hitCntY > hitCntX)
                    ball.vBounce();
                else if (hitCntY < hitCntX)
                    ball.hBounce();
                else {
                    ball.vBounce();
                    ball.hBounce();
                }
            }
        }


        if ( ( ball.getDim().y < 0 ) && ( ball.getVel().y < 0 ) ) {
            ball.vBounce();
        }

        if ( ( ball.getDim().x > SCREEN_WIDTH - ball.getDim().w ) && ( ball.getVel().x > 0 ) ) {
            ball.hBounce();
        }

        if ( ( ball.getDim().x < 0 ) && ( ball.getVel().x < 0 ) ) {
            ball.hBounce();
        }

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

        if (!gameOver){
            // Player Input/Control Logic
            if (lInput)
                player.moveLeft();

            if (rInput)
                player.moveRight();

            // Check for collisions between ball(s) and brick(s)
            hitDetection();



            // Ball Logic
            ball.update();

            if (player.checkCollision(ball.getDim())){
                offsetPB = ball.getDim().x - player.getDim().x;
                if (stickyP){
                    ball.setVel(0,0);
                    stuck = true;
                }
                else {
                    hitSpeed = ball.getDim().x - ( player.getDim().x + player.getDim().w/2);

                    if ( hitSpeed> 0)
                        ball.setVel( ( hitSpeed + (player.getDim().w / 10) ) / (player.getDim().w / 10) , -ball.vel.y);
                    else
                        ball.setVel( ( hitSpeed - (player.getDim().w / 10) ) / (player.getDim().w / 10) , -ball.vel.y);
                    
                }
                
            }

            if (stuck){
                ball.setPos(player.getDim().x + offsetPB, player.getDim().y - ball.getDim().h );
                if (spInput){
                    hitSpeed = ball.getDim().x - ( player.getDim().x + player.getDim().w/2);
                    if ( hitSpeed> 0)
                        ball.setVel( ( hitSpeed + (player.getDim().w / 10) ) / (player.getDim().w / 10) , -BALL_VELOCITY);
                    else
                        ball.setVel( ( hitSpeed - (player.getDim().w / 10) ) / (player.getDim().w / 10) , -BALL_VELOCITY);

                    stuck = false;
                    stickyP = false;
                }
            }

            if  ( ball.getDim().y > SCREEN_HEIGHT + 10 ) {
                lives--;
                updateLivesText();
                offsetPB = player.getDim().w/2;
                stuck = true;
                if (lives == 0)
                    gameOver = true;
            }

        }

        if (levelBegin){
            if (delayTimer.getTicks() > 3000){
                delayTimer.stop();
                showLvl = false;
                levelBegin = false;
            }
        }

    }

    void render(){

        // Set background color and fill
        SDL_SetRenderDrawColor( gRenderer, bgR, bgG, bgB, 0xFF );
        SDL_RenderFillRect(gRenderer, &field);

        // Set sprite color
        SDL_SetRenderDrawColor( gRenderer, spR, spG, spB, 0xFF );
        player.render();
        ball.render();

        for (int i = 0; i < wall.size(); i++){
            wall[i]->render();
        }
        

        // Update Text color and render
        livesTextTexture.setColor(spR, spG, spB);
        livesTextTexture.render(SCREEN_WIDTH - livesTextTexture.getWidth(), 1 );

        scoreTextTexture.setColor(spR, spG, spB);
        scoreTextTexture.render(5, 1 );

        if (showLvl) {
            lvlTextTexture.setColor(spR, spG, spB);
            lvlTextTexture.render(SCREEN_WIDTH/2 - lvlTextTexture.getWidth()/2, SCREEN_HEIGHT - lvlTextTexture.getHeight() * 2 );
        }

    }

};

#endif // GAMELOOP_H_INCLUDED