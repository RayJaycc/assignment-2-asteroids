/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   THE CONTROLS TO LET THE USER INTERACT WITH THE GAME
*******************************************************************************************************
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <display.h>
#include <mbed.h>
#include <physics.h>
#include <controls.h>
#include <drawing.h>

/*variables exclusive to this file*/
static DigitalIn buttons[] = {P5_0, P5_1, P5_4, P5_2, P5_3};
static bool pressRelease(btnId_t b);
static bool isPressed(btnId_t p);

/*changing define values will change how game plays*/
#define turnPoint 9
#define accMax 2
#define accInc 0.2

/*simple method to check if the joystick is being held in a direction.
* upon release, it will be false.
*/
bool isPressed(btnId_t p){
    return !buttons[p];
}

/*method to check if the joystick was clicked in a direction.
* unlike is pressed, pressRelease does not stay true if held.
*/
bool pressRelease(btnId_t b){
	bool result = false;
	static bool waspressed[] = {false, false, false, false, false};
	bool pressed = isPressed(b);

	result = waspressed[b] && !pressed;
	waspressed[b] = pressed;
	return result;
}

/*When the joystick is moved in certain directions, the ship will act a certain way.
* Left while rotate the ship left, right will rotate the ship right, up will move the ship forwards
* by increasing the acceleration, and centre will fire missiles.
*/
void controls(void){
  if(isPressed(JLEFT)){
    direction-=turnPoint;
  } else if(isPressed(JRIGHT)){
    direction+=turnPoint;
  } else if(isPressed(JUP) && (accel<accMax)){
			accel+=accInc;
  }
	if(isPressed(JCENTRE)){
		missileSystem();
  }
}

/*When the player runs out of lives, prompt them with a gameover screen and
* give them the option to start again. 
* the state of 'joy' is used to prevent the game from automatically restarting,
* and only when it has input can the user start again.
*/
void playerContinue(void) {
	bool startAgain = false;
	stickMd_t joy = no_input;
	gameOverText();
	
	do{
    gameOverResp(joy);
		
		if (pressRelease(JLEFT)){
			joy = input;
		}

		if (pressRelease(JCENTRE) && joy == input){
			startAgain = true;
		} 
	}
	while(startAgain == false);

	return;
}


