# Brickbreak

Brickbreak is an open-source fan-made Breakout clone incorporating gameplay aspects and level design from Arkanoid.

## Running the Game

The binary included in this release has been compiled to run on modern 32-bit or 64-bit Windows systems. If you are using Windows 10, simply download and extract this release, and run "Brickbreak.exe" which is located within the "win32" directory.

If you are using a Mac or Linux OS, you will need to recompile the project in order to run the game.

## Compiling the Game

This game was developed in C++ using SDL 2.0 libraries. I used Lazy Foo's tutorials to get started with this project, and I highly recommend following those tutorials in order to set up an SDL 2.0 environment for your development platform of choice.

Lazy Foo's Website:
https://lazyfoo.net/tutorials/SDL/index.php

## How to Play

The goal of Brickbreak is to destroy all the bricks on the screen by launching a ball with your paddle.

Move the mouse to control the position of the paddle. Left-click to interact with certain power-ups such as firing lasers and releasing the ball.

The angle/trajectory of the ball is determined by its offset when it collides with the paddle. A collision toward the ends of the paddle will result in a wider shot than a collision toward the center of the paddle.

Breaking a brick will award a number of points depending on the current speed of the ball. The ball will gradually speed up over time as bricks are destroyed. When all bricks on the screen are destroyed, a new level will load. There are currently 34 levels in total; can you beat them all in one sitting?

## Controls

Mouse: Movement/Menu Navigation

Left Click: Action/Select

Escape: Pauses the Game (Returns to Menu when game is over)

Backspace: Return to the menu (Anytime during normal gameplay)
