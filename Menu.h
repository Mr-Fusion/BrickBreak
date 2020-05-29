#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <SDL.h>
#include <stdlib.h>
#include "Button.h"

#define LEFT_JUSTIFY    10
#define RIGHT_JUSTIFY   SCREEN_WIDTH - 10

enum {
    MENU_START_GAME = 0,
    MENU_SETTINGS = 1,
    MENU_QUIT = 2,
    MENU_SIZE = 3
};

enum {
    SETTINGS_SOUND = 0,
    SETTINGS_DIFFY = 1,
    SETTINGS_MULTI = 2,
    SETTINGS_BACK = 3,
    SETTINGS_SIZE = 4
};

enum {
    DIFFY_NORM = 0,
    DIFFY_EASY = 1,
    DIFFY_FREE = 2,
    DIFFY_SIZE = 3
}

class Menu : public GameState
{
    public:

    bool retInput = false;
    bool subMenu = false;
    int curSelection = 0;

    bool sfxEnable = true;
    bool multiEnable = false;
    int difficulty = DIFFY_NORM;

    LTexture titleText;
    LTexture startText;
    LTexture settingText;
    LTexture quitText;

    LTexture soundText;
    LTexture diffyText;
    LTexture multiText;
    LTexture backText;

    LTexture soundTextValue;
    LTexture diffyTextValue;
    LTexture multiTextValue;

    LTexture creditText;

    SDL_Color textColor;

    ///Constructor Function
    Menu(){

        textColor = { 0xFF, 0xFF, 0xFF };

        //Load media
        if( !loadMedia() )
        {
            printf( "Failed to load media!\n" );
        }
        else
        {
        	//Initialize Menu here

        	SDL_SetWindowSize(gWindow,SCREEN_WIDTH, SCREEN_HEIGHT);
        }

    }

    ///Deconstructor
    ~Menu(){
        printf("Menu Object Deconstructing...\n");

		//Destroy Menu assets here

        //Free loaded image
        titleText.free();
        startText.free();
        settingText.free();
        quitText.free();
        creditText.free();

    }

    bool loadMedia()
    {
        //Loading success flag
        bool success = true;

        //Load sprite sheet texture



        //Open the font
        gFont = TTF_OpenFont( "../assets/SAMSRG__.TTF", 60 );
        if( gFont == NULL )
        {
            printf( "Failed to load Pong Title font! SDL_ttf Error: %s\n", TTF_GetError() );
            success = false;
        }
        else
        {
            //Render text
            if( !titleText.loadFromRenderedText( TITLE, textColor ) )
            {
                printf( "Failed to render title text!\n" );
                success = false;
            }
        }

        gFont = TTF_OpenFont( "../assets/PressStart2P.ttf", 14 );
        if( gFont == NULL )
        {
            printf( "Failed to load Minesweeper font! SDL_ttf Error: %s\n", TTF_GetError() );
            success = false;
        }
        else
        {
            //Render text
            //Set text to be rendered
            std::stringstream credit;
            credit.str( "" );
            credit << "By Joel Turner | Version: " << MAJOR_REV << "." << MINOR_REV << "." << INCREMENTAL_REV;

            //Render text
            if( !creditText.loadFromRenderedText( credit.str().c_str(), textColor ) )
            {
                printf( "Failed to render credit text!\n" );
                success = false;
            }
        }

        gFont = TTF_OpenFont( "../assets/SAMST___.TTF", 40 );
        if( gFont == NULL )
        {
            printf( "Failed to load Pong font! SDL_ttf Error: %s\n", TTF_GetError() );
            success = false;
        }
        else
        {
            //Render text
            if( !startText.loadFromRenderedText( "Start Game", textColor ) )
            {
                printf( "Failed to render startText!\n" );
                success = false;
            }
            if( !settingText.loadFromRenderedText( "Settings", textColor ) )
            {
                printf( "Failed to render seetingText!\n" );
                success = false;
            }
            if( !quitText.loadFromRenderedText( "Quit", textColor ) )
            {
                printf( "Failed to render quitText!\n" );
                success = false;
            }

            if( !soundText.loadFromRenderedText( "Sound Effects:", textColor ) )
            {
                printf( "Failed to render soundText!\n" );
                success = false;
            }
            if( !diffyText.loadFromRenderedText( "Difficulty:", textColor ) )
            {
                printf( "Failed to render diffyText!\n" );
                success = false;
            }
            if( !multiText.loadFromRenderedText( "Oops, all multiballs!", textColor ) )
            {
                printf( "Failed to render multiText!\n" );
                success = false;
            }
            if( !backText.loadFromRenderedText( "Back", textColor ) )
            {
                printf( "Failed to render backText!\n" );
                success = false;
            }

            if( !soundTextValue.loadFromRenderedText( "On", textColor ) )
            {
                printf( "Failed to render soundTextValue!\n" );
                success = false;
            }
            if( !diffyTextValue.loadFromRenderedText( "Normal", textColor ) )
            {
                printf( "Failed to render diffyTextValue!\n" );
                success = false;
            }
            if( !multiTextValue.loadFromRenderedText( "Off", textColor ) )
            {
                printf( "Failed to render multiTextValue!\n" );
                success = false;
            }

        }
        return success;
    }
    
    ///Handles mouse event
    void handleEvent( SDL_Event* e){

        int size;

        if (subMenu)
            size = SETTINGS_SIZE;
        else
            size = MENU_SIZE;

        if (e->type == SDL_KEYDOWN) {
            switch (e->key.keysym.sym) {
                case SDLK_RETURN:
                    retInput = true;
                break;
                case SDLK_w:
                    curSelection--;
                    if (curSelection < 0)
                        curSelection = size - 1;
                break;

                case SDLK_s:
                    curSelection++;
                    if (curSelection >= size )
                        curSelection = 0;
                break;
            }
        }
    }

    void toggleText(LTexture *text, bool *flag) {
        if (flag) {
            flag = false;
            text->loadFromRenderedText( "Off", textColor );
        }
        else {
            flag = true;
            text->loadFromRenderedText( "On", textColor );
        }
    }

    void updateDiffy(int d){
        switch (d) {
            case DIFFY_NORM:
                diffyTextValue.loadFromRenderedText( "Normal", textColor );
            break;
            case DIFFY_EASY:
                diffyTextValue.loadFromRenderedText( "Easy", textColor );
            break;
            case DIFFY_FREE:
                diffyTextValue.loadFromRenderedText( "Freeplay", textColor );
            break;
        }
    }

    void topMenu(){

        // Set all options to white text by default
        startText.setColor(0xFF, 0xFF, 0xFF);
        settingText.setColor(0xFF, 0xFF, 0xFF);
        quitText.setColor(0xFF, 0xFF, 0xFF);

        // Highlight the appropriate option in red corresponding to the current cursor selection
        switch (curSelection) {
            case MENU_START_GAME:
                startText.setColor(0xFF, 0x00, 0x00);
            break;
            case MENU_SETTINGS:
                settingText.setColor(0xFF, 0x00, 0x00);
            break;
            case MENU_QUIT:
                quitText.setColor(0xFF, 0x00, 0x00);
            break;
        }

        if (retInput){
            switch (curSelection) {
                case MENU_START_GAME:
                    set_next_state( STATE_GAME );
                break;
                case MENU_SETTINGS:
                    subMenu = true;
                    curSelection = 0;
                break;
                case MENU_QUIT:
                    set_next_state( STATE_EXIT );
                break;
            }
            retInput = false;
        }
    }

    void settingsMenu(){

        // Set all options to white text by default
        soundText.setColor(0xFF, 0xFF, 0xFF);
        diffyText.setColor(0xFF, 0xFF, 0xFF);
        multiText.setColor(0xFF, 0xFF, 0xFF);
        backText.setColor(0xFF, 0xFF, 0xFF);

        // Highlight the appropriate option in red corresponding to the current cursor selection
        switch (curSelection) {
            case SETTINGS_SOUND:
                soundText.setColor(0xFF, 0x00, 0x00);
                soundTextValue.setColor(0xFF, 0x00, 0x00);
            break;
            case SETTINGS_DIFFY:
                diffyText.setColor(0xFF, 0x00, 0x00);
                diffyTextValue.setColor(0xFF, 0x00, 0x00);
            break;
            case SETTINGS_MULTI:
                multiText.setColor(0xFF, 0x00, 0x00);
                multiTextValue.setColor(0xFF, 0x00, 0x00);
            break;
            case SETTINGS_BACK:
                backText.setColor(0xFF, 0x00, 0x00);
            break;
        }

        if (retInput){
            switch (curSelection) {
                case SETTINGS_SOUND:
                    toggleText( &soundTextValue, &sfxEnable);
                break;
                case SETTINGS_DIFFY:
                    difficulty++;
                    if (difficulty == DIFFY_SIZE)
                        difficulty = DIFFY_NORM;
                    updateDiffy( difficulty );
                break;
                case SETTINGS_MULTI:
                    toggleText( &multiTextValue, &multiEnable);
                break;
                case SETTINGS_BACK:
                    subMenu = false;
                    curSelection = 0;
                break;
            }
            retInput = false;
        }
    }

    void logic(){

        if ( subMenu )
            settingsMenu();
        else
            topMenu();

    }

    void render(){

        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

        titleText.render(SCREEN_WIDTH/2 - titleText.getWidth()/2, 30);

        if ( subMenu ) {
            soundText.render( LEFT_JUSTIFY, SCREEN_HEIGHT/2 - soundText.getHeight() );
            diffyText.render( LEFT_JUSTIFY, SCREEN_HEIGHT/2 );
            multiText.render( LEFT_JUSTIFY, SCREEN_HEIGHT/2 + multiText.getHeight() );
            backText.render( LEFT_JUSTIFY, SCREEN_HEIGHT/2 + backText.getHeight() * 2);

            soundTextValue.render( RIGHT_JUSTIFY - soundTextValue.getWidth(), SCREEN_HEIGHT/2 - soundTextValue.getHeight() );
            diffyTextValue.render( RIGHT_JUSTIFY - diffyTextValue.getWidth(), SCREEN_HEIGHT/2 );
            multiTextValue.render( RIGHT_JUSTIFY - multiTextValue.getWidth(), SCREEN_HEIGHT/2 + multiTextValue.getHeight() );
        }
        else {
            startText.render(SCREEN_WIDTH/2 - startText.getWidth()/2, SCREEN_HEIGHT/2 - startText.getHeight() );
            settingText.render(SCREEN_WIDTH/2 - settingText.getWidth()/2, SCREEN_HEIGHT/2);
            quitText.render(SCREEN_WIDTH/2 - quitText.getWidth()/2, SCREEN_HEIGHT/2 + quitText.getHeight());
            creditText.render(SCREEN_WIDTH/2 - creditText.getWidth()/2, SCREEN_HEIGHT - 60);
        }

    }

};

#endif // MENU_H_INCLUDED