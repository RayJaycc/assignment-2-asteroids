/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   MAKING ELEMENTS VISIBLE ON THE DISPLAY
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
#include <drawing.h>
#include <physics.h>

/*Initialise LCD*/
Display *lcd = Display::theDisplay();

/*variables exclusive to this file*/
static unsigned char GO_bmp[] ={0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x7e, 
0x00, 0x00, 0xff, 0x00, 0x01, 0xff, 0x80, 0x01, 0xff, 0x80, 
0x01, 0xff, 0x80, 0x00, 0x08, 0x80, 0x00, 0x08, 0x00, 0x01, 
0x90, 0x80, 0x00, 0xe7, 0x00, 0x04, 0x7c, 0x20, 0x0e, 0x10, 
0x78, 0x0b, 0x81, 0xc0, 0x00, 0x7e, 0x00, 0x00, 0x3e, 0x00, 
0x01, 0xc1, 0x80, 0x1e, 0x00, 0x78, 0x0c, 0x00, 0x30, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};//the hex values that compose the bitmap image
typedef uint16_t colour_t;
#define rgb(r,g,b) ((uint16_t)(31 * r /255)<<11 | (uint16_t)(63 * g /255)<<5 | (uint16_t)(31 * b /255))//define RGB to allow for colour creation
const colour_t background = rgb(0,51,102);//different values will create different colours

/*changing define values will change how game plays*/
#define asteroidSize 6
#define missileSize 1
#define borderX 0
#define borderY 0
#define borderWidth 480
#define borderHeight 15
#define imageWidth 24
#define imageHeight 24
#define image1X 210
#define image1Y 25
#define image2X 240
#define image2Y 25
#define image3X 270
#define image3Y 25

/*draw the game onto the LCD, invoked by all.cpp*/
void draw(void){
	lcd->setTextSize(1);
	lcd->fillScreen(background);
	drawmissiles(active);
	drawasteroids(asteroidActive);
	drawBorder(); 
	shield();
	shieldOverlay();
	swap_double_buffers();
}

/*draw the border present at the top of the screen*/
void drawBorder(void){
	lcd->drawRect(borderX, borderY, borderWidth, borderHeight, WHITE);
	lcd->setCursor(5,5);
	lcd->printf("Lives Left:%i", lives);
	lcd->setCursor(250,5);
	lcd->printf("Current Life:%i", timeAlive);
	lcd->setCursor(370,5);
	lcd->printf("Total Score:%i", totalTimeAlive);
}

/*gameover screen, invoked by playerContinue once.
* swap to single buffer to prevent blinking.
*/
void gameOverText(void){
	init_single_buffer();
	lcd->setTextSize(3);
	lcd->fillScreen(background);
	lcd->setCursor(170,55);
	bitmap();
	lcd->printf("GAME OVER");
	lcd->setCursor(140,105);
	lcd->printf("FINAL SCORE:%i", totalTimeAlive);
	lcd->setCursor(170,155);
	lcd->printf("CONTINUE?");
}

/*display text according to the input of 'joy'
* when the input changes, clear the screen to remove "NO",
* re-apply gameOverText, and print new text. 
* The boolean 'init' is used to ensure this happens only once.
*/
void gameOverResp(stickMd_t joy){
	static bool init;
	lcd->setCursor(230,185);
	switch (joy){
		case no_input:
			lcd->printf("NO  ");
		init = false;
		break;
		case input:
			if(!init){
				init = true;
				lcd->fillScreen(BLACK);
				gameOverText();
				lcd->setCursor(220,185);
				lcd->printf("YES ");
				lcd->setCursor(140,215);
				lcd->printf("PRESS CENTRE");
			}
		break;
	}
}

/*initialise two buffers to allow them to be swapped between.
* Eliminates the issue of flickering while playing the game.
*/
void init_double_buffering(void){
    uint16_t *buffer = lcd->getFb();
    uint16_t *frame2 = buffer+(480*272);
    LPC_LCD->UPBASE = (uint32_t)frame2;
}

/*initialise a single buffer.
* This will be used with gameOverText to prevent flickering.
*/
void init_single_buffer(void){
	uint16_t *buffer = lcd->getFb();
	LPC_LCD->UPBASE = (uint32_t)buffer;
}

/*drawing the missiles on the screen.
* If a new missile this will also be drawn.
*/
void drawmissiles(struct missile *lst){
    while(lst) {
        lcd->fillCircle(lst->misX,lst->misY, missileSize, WHITE);
        lst = lst->next;
    }
}
/*drawing the asteroids on the screen.
* When a new asteroid spawns this will be drawn.
*/
void drawasteroids(struct asteroid *lst){
    while(lst) {
        lcd->fillCircle(lst->astX,lst->astY, asteroidSize, WHITE);
        lst = lst->next;
    }
}

/*Text defining how strong the shield is*/
void shield(void){
	lcd->setCursor(120,5);
	if(shieldHealth==3){
		lcd->printf("Shield Online!");
	} else if(shieldHealth==2){
		lcd->printf("Shield Weakened!");
	} else if(shieldHealth==1){
		lcd->printf("Shield Critical!");
	} else if(shieldHealth==0){
		lcd->printf("Shield Offline!");
	}
}

/*shield is made visible by overlaying different colours on top of the ship.
* full shield green, middle yellow, low red, and depleted white.
* lines are placed on top by making their placement the same as the ship.
*/
void shieldOverlay(void){
	if(shieldHealth==3){
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+faceX,shipY+faceY,GREEN);
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+rightX,shipY+rightY,GREEN);
					lcd->drawLine(shipX+rightX,shipY+rightY,shipX+faceX,shipY+faceY,GREEN);
				}
				if(shieldHealth==2){
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+faceX,shipY+faceY,YELLOW);
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+rightX,shipY+rightY,YELLOW);
					lcd->drawLine(shipX+rightX,shipY+rightY,shipX+faceX,shipY+faceY,YELLOW);
				}
				if(shieldHealth==1){
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+faceX,shipY+faceY,RED);
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+rightX,shipY+rightY,RED);
					lcd->drawLine(shipX+rightX,shipY+rightY,shipX+faceX,shipY+faceY,RED);
				}
				if(shieldHealth<1){
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+faceX,shipY+faceY,WHITE);
					lcd->drawLine(shipX+leftX,shipY+leftY,shipX+rightX,shipY+rightY,WHITE);
					lcd->drawLine(shipX+rightX,shipY+rightY,shipX+faceX,shipY+faceY,WHITE);
				}
}

/*place the bitmap on the screen.
* X, Y, imageName, width, height, colour.
*/
void bitmap(void){
	lcd->drawBitmap(image1X, image1Y, GO_bmp, imageWidth, imageHeight, WHITE);
	lcd->drawBitmap(image2X, image2Y, GO_bmp, imageWidth, imageHeight, WHITE);
	lcd->drawBitmap(image3X, image3Y, GO_bmp, imageWidth, imageHeight, WHITE);
}

/*swap between the two buffers defined before*/
void swap_double_buffers(void){
    unsigned int b;
    b = LPC_LCD->UPBASE;
    LPC_LCD->UPBASE = (uint32_t)lcd->getFb();
    lcd->setFb((uint16_t *)b);
}
