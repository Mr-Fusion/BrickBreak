#ifndef CONST_H_INCLUDED
#define CONST_H_INCLUDED

//The window renderer
extern SDL_Renderer* gRenderer = NULL;

//The window we'll be rendering to
extern SDL_Window* gWindow = NULL;

//Globally used font
extern TTF_Font *gFont = NULL;

#define DBG_SHOW_FPS        false

#define TITLE				"BrickBreak"

#define MAJOR_REV           0
#define MINOR_REV           4
#define INCREMENTAL_REV     4

//Screen dimension constants
#define SCREEN_WIDTH        528//640
#define SCREEN_HEIGHT       720//480
#define SCREEN_FPS          60

const int SCREEN_TICK_PER_FRAME = 1000 / SCREEN_FPS;

enum {
    MENU_START_GAME = 0,
    MENU_SETTINGS = 1,
    MENU_QUIT = 2,
    MENU_SIZE = 3
};

enum {
    SETTINGS_SOUND = 0,
    SETTINGS_DIFFY = 1,
    SETTINGS_SHUFF = 2,
    SETTINGS_MULTI = 3,
    SETTINGS_BACK = 4,
    SETTINGS_SIZE = 5
};

enum {
    DIFFY_NORM = 0,
    DIFFY_EASY = 1,
    DIFFY_FREE = 2,
    DIFFY_SIZE = 3
};

struct settings {
    settings() : difficulty(DIFFY_NORM), sfxEnable(true), shuffleEnable(false), multiEnable(false) {}
    int difficulty;
    bool sfxEnable;
    bool shuffleEnable;
    bool multiEnable;
} gameSettings;

#endif // CONST_H_INCLUDED
