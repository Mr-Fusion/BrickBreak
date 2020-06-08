# Brickbreak

Brickbreak is an open-source fan-made Breakout clone incorporating gameplay aspects and level design from Arkanoid.

## Running the Game

The binary included in this release has been compiled to run on modern 32-bit or 64-bit Windows systems. If you are using Windows 10, simply download and extract this release, and run "Brickbreak.exe" which is located within the "win32" directory.

If you are using a Mac or Linux OS, you will need to recompile the project in order to run the game.

## Compiling the Game

This game was developed in C++ using SDL 2.0 libraries. I used Lazy Foo's tutorials to get started with this project, and I highly recommend following those tutorials in order to set up an SDL 2.0 environment for your development platform of choice.

Lazy Foo's Website:
[https://lazyfoo.net/tutorials/SDL/index.php](https://lazyfoo.net/tutorials/SDL/index.php)

## How to Play

The goal of Brickbreak is to destroy all the bricks on the screen by launching a ball with your paddle.

Move the mouse to control the position of the paddle. Left-click to interact with certain power-ups such as firing lasers and releasing the ball.

The angle/trajectory of the ball is determined by its offset when it collides with the paddle. A collision toward the ends of the paddle will result in a wider shot than a collision toward the center of the paddle.

Breaking a brick will award a number of points depending on the current speed of the ball. The ball will gradually speed up over time as bricks are destroyed. When all bricks on the screen are destroyed, a new level will load. There are currently 34 levels in total; can you beat them all in one sitting?

## Settings

The settings menu can be accessed via the main menu. The following options and their effects are as follows:

__Sound Effects:__ Toggle sound playback off/on

__Difficulty:__ "Normal" is the default gameplay. "Easy" starts each level with a slower ball, and takes longer to speed up. Power-ups are also preserved between rounds. "Freeplay" features the same ball speed properties as normal, but with unlimited lives. However, scoring in Freeplay is disabled.

__Shuffle Levels:__ When enabled, the level layouts will be shuffled in a random order when the game is started. Levels will not repeat until 34 rounds are cleared.

__All Multiballs:__ All power-up spawns will be of the Multiball type. How many balls can you juggle at once?

## Controls

__Mouse:__ Movement/Menu Navigation

__Left Click:__ Action/Select

__Escape:__ Pauses the Game (Returns to menu when game is over)

__Backspace:__ Return to the menu (Any time during normal gameplay)

## Power-ups

Power-ups have a random chance to spawn when a brick is destroyed, and will slowly fall to the bottom of the screen. Catch one to activate its effects! The power-up effects are color-coded:

__White:__ Awards Bonus Points

__Cyan:__ Sticky Paddle. Balls will be caught by the paddle on impact, and can be released using the action input.

__Magenta:__ Multiball. All balls on the playing field will spawn a clone with a slightly different trajectory

__Purple:__ Piercing. The trajectory of ball and laser objects will not be affected when colliding with bricks

__Orange:__ Lasers. Enables twin lasers to be fired from each end of the paddle using the action input

__Blue:__ Increases the paddle size

__Red:__ Decreases the paddle size

__Green:__ Increases the speed of the ball

__Yellow:__ Decreases the speed of the ball

__Rainbow:__ Adds an extra life!
