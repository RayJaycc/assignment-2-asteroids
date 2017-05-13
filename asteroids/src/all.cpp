/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   THE GAME BEING PLAYED, INVOKES OTHER CLASSES WITH TICKERS
*******************************************************************************************************
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <display.h>
#include <mbed.h>
#include <controls.h>
#include <physics.h>
#include <drawing.h>

/*attaching tickers to other .cpp files, allowing them to be invoked repeatedly*/
Ticker drawing;
Ticker model;
Ticker controller;
Ticker asteroids;

/*defining the length of a tick, and thus how frequently methods are invoked*/
static const float Dt = 0.01;
static const float drawTick = 0.025;
static const float controlTick = 0.1;
static const float asteroidTick = 0.1;

/*When starting the game attach all tickers and initialise values to default*/
void gameInit(void){
	initialiseHeap();
	initialiseAsteroidHeap();
	init_double_buffering();
	initialValues();
	drawing.attach(draw, drawTick);
	model.attach(physics, Dt);
	controller.attach(controls, controlTick);
	asteroids.attach(asteroidSystem, asteroidTick);
}

/*first initialise game, allow playing while the player still has lives.
* when lives reach zero empty all lists, de-attach the tickers to 
* prevent methods from being called, call gameOver and wait for player
* input.
*/
int main(){
	while(1){
		gameInit();
		while(lives>0){};
			
			astClearList(asteroidActive);
			misClearList(active);
			drawing.detach();
			model.detach();
			asteroids.detach();
			playerContinue();
			wait_ms(100);
			gameInit();
	}
}
