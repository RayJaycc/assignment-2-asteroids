/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   PHYSICS IS USED FOR THE SHIP, ITS' MISSILES, AND THE ASTEROIDS
*   EQUIVALENT TO MODEL
*******************************************************************************************************
*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include <display.h>
#include <mbed.h>
#include <utilities.h>
#include <physics.h>

/*Variables used exclusively in this file*/
static const int heapsize = 3;
static const int asteroidHeapsize = 30;
static node_t heap[heapsize];
static asteroid_t asteroidheap[asteroidHeapsize];
static node_t *freenodes;
static asteroid_t *asteroidfreenodes;
static const float Dt = 0.01;//used to continously lower lifespan of listItems

/*global variables used in methods*/
float shipX;
float shipY;
int direction;
float directionRadians;
float faceX;
float faceY;
float leftX; 
float leftY;
float rightX; 
float rightY; 
double score;
int lives;
double speed;
int shieldHealth;
int totalScore;
double accel;
int timeAlive;
int totalTimeAlive;
int invincibility;

/*changing define values will change how game plays*/
#define maxSpeed 3
#define minAccel 0
#define minSpeed 0
#define accDegrade 0.1
#define spDegrade 0.01
#define invMin 0
#define startX 240;
#define startY 30;
#define startAngle 90;
#define startScore 0;
#define startTime 0;
#define startShieldHealth 3;
#define initLives 3;
#define invPeriod 100;
#define invRegPer 50;
#define wrapLeft 5;
#define wrapRight 475;
#define wrapTop 25;
#define wrapBottom 255;

/*initially no active asteroids or missiles*/
struct missile *active = NULL;
struct asteroid *asteroidActive = NULL;

/*physics invoked by all.cpp*/
void physics(void){
	checkShipAsteroids(asteroidActive);
	checkMissilesAsteroids(active, asteroidActive);
	update(active);
	asteroidUpdate(asteroidActive);
	wrapAround();
	defineShip();
	time();
	if(invincibility>invMin){
		invincibility--;
	}
	score++;
	speedCheck();
}

/*defining the shape of the ship and how it will move*/
void defineShip(void){
	directionRadians=radians(direction);
	faceX=10*(cos(directionRadians));
	faceY=10*(sin(directionRadians));
	leftX=(-5*(cos(directionRadians)))-(5*(sin(directionRadians)));
	leftY=(-5*(sin(directionRadians)))+(5*(cos(directionRadians)));
	rightX=(-5*(cos(directionRadians)))-(-5*(sin(directionRadians)));;
	rightY=(-5*(sin(directionRadians)))+(-5*(cos(directionRadians)));;
	shipX+=speed*(cos(directionRadians));
	shipY+=speed*(sin(directionRadians));
}

/*ship will increase/decrease in speed*/
void speedCheck(void){
	if(speed<maxSpeed){
			speed+=accel;
		}
		if(accel>minAccel){
			accel-=accDegrade;
		}
		if(speed>minSpeed){
			speed-=spDegrade;
		}
}

/*values at the start of the game*/
void initialValues(void){
	shipX=startX;
	shipY=startY;
	direction=startAngle;
	score=startScore;
	shieldHealth=startShieldHealth;
	totalScore=startScore;
	totalTimeAlive=startTime;
	lives=initLives;
	accel=minAccel;
	invincibility=invPeriod;
}

/*values after a life is taken, but the game is still playing*/
void respawn(void) {
	shipX=startX;
	shipY=startY;
	direction=startAngle;
	shieldHealth=startShieldHealth;
	invincibility=invPeriod;
	accel=minAccel; 
	score=startScore;
}

/*Constantly check if all spawned asteroids are touching the ship.
* if hit, reduce shield health or take a life if shield is depleted.
* Should the game still be playing, respawn the player and continue.
*/
void checkShipAsteroids(struct asteroid *c){
    for( ; c ; c = c->next ) {
        if(c->astX<(shipX+8) && c->astX>(shipX-8)){
          if(c->astY<(shipY+8) && c->astY>(shipY-8)){
            if(shieldHealth>0){
							if(invincibility==0){
								shieldHealth--;
								invincibility=invRegPer;
							} 
						} else if(shieldHealth<=0){
							if(invincibility==0){
								if(lives>0){
									lives--;
								}
								respawn();
								totalTimeAlive+=timeAlive;
							}
						}
          }
        }
    }
}

/*constantly check if all spawned missiles touch a spawned asteroid.
* If the missile does touch an asteroid, astTtl is set to 0 and thus 'destroyed'.
*/
void checkMissilesAsteroids(struct missile *a, struct asteroid *b) {
  for(; a ; a = a->next){
    for(; b ; b = b->next){
      if(a->misX > (b->astX-8) && a->misX < (b->astX+8)){
        if(a->misY > (b->astY-8) && a->misY < (b->astY+8)){
          b->astTtl=0;
        }
      }

      }
    }
 }

/*continously check if spawned missile should still be 'alive'.
* If misTtl reaches zero, it is 'destroyed' and the node expires.
* misTtl continously lowers with Dt, and will instantly reach zero
* if it reaches the out of bounds area.
*/
 void update(struct missile *l){
    for( ; l ; l = l->next ) {
        l->misX += l->misVX * Dt;
        l->misY += l->misVY * Dt;
        if( l->misX<0 || 480<l->misX ) l->misTtl=0;
        if( l->misY<10 || 260<l->misY ) l->misTtl=0;
        l->misTtl -= Dt;
        if( l->next->misTtl<=0 ) {
            struct missile *expired = l->next;
            l->next = l->next->next;
            freenode(expired);
        }
    }
}

/*continously check if spawned asteroids should still be 'alive'.
* if is astTtl reaches zero, it is destroyed and the node expires.
* astTtl continously lowers with Dt, and will instantly reach zero 
* if it reaches the out of bounds area.
*/
void asteroidUpdate(struct asteroid *l){
    for( ; l ; l = l->next ) {
        l->astX += l->astVX * Dt;
        l->astY += l->astVY * Dt;
        if( l->astX<-20 || 500<l->astX ) l->astTtl=0;
        if( l->astY<25 || 280<l->astY ) l->astTtl=0;
        l->astTtl -= Dt;
        if( l->next->astTtl<=0 ) {
            struct asteroid *expired = l->next;
            l->next = l->next->next;
            asteroidFreenode(expired);
        }
    }
}

/*iterate through the list of asteroids and set astTtl to zero,
* destroying all asteroids that existed before.
*/
void astClearList(struct asteroid *l){
	while (l){
		l->astTtl=0;
		l = l->next;
	}
}

/*iterate through the list of missiles and set misTtl to zero,
* destroying all missiles that existed before.
*/
void misClearList(struct missile *r){
	while (r){
		r->misTtl=0;
		r = r->next;
	}
}

/*method to prevent the ship from leaving the screen.
* should the ship reach the defined values, it will be moved
* into another position on the screen.
*/
void wrapAround(void){
  if(shipX>480){
		shipX=wrapLeft;
	} else if(shipX<0){
		shipX=wrapRight;
	} else if(shipY>260){
		shipY=wrapTop;
	} else if(shipY<20){
		shipY=wrapBottom;
	}
}

/*time spent playing the game; the greater it is, the higher the user scores*/
void time(void){
	timeAlive = (int)((score/100)+0.5);
}

/*when the missile spawn, they will initially appear at the same coordinates as
* the ship. They will travel in the direction the shape was facing when the command was given.
*/
void strike(struct missile *r){
    r->misX = shipX;
    r->misY = shipY;
    r->misVX = 200*(cos(directionRadians));
    r->misVY = 200*(sin(directionRadians));
    r->misTtl = 200;
}

/*astX and astY are given a random value to allow them to spawn within the defined range on screen.
* astVX and astVY will make the asteroid travel in a random direction with a speed in the defined range.
* astTtle have a lifespan between 900-1100.
*/
void strike(struct asteroid *r){
    r->astX = randrange(10,470);
    r->astY = randrange(10,250);
    r->astVX = randrange(-50,50); 
    r->astVY = randrange(-50,50);
    r->astTtl = randrange(900,1100);
}

/*will spawn the missiles when there are free nodes*/
void missileSystem(void){
        struct missile *spark = allocnode();
        if(spark) {
            spark->next = active;
            active = spark;
            strike(spark);
        }
}

/*will spawn asteroids when there are free nodes*/
void asteroidSystem(void){
        struct asteroid *asteroidspark = asteroidAllocnode();
        if(asteroidspark) {
            asteroidspark->next = asteroidActive;
            asteroidActive = asteroidspark;
            strike(asteroidspark);
        }
}

/*creates a heap containing all nodes for the missiles.
* The heapsize defines how many nodes there will be, and thus how many missiles
* can be spawned.
*/
void initialiseHeap(void){
    int n;
    for( n=0 ; n<(heapsize-1) ; n++) {
        heap[n].next = &heap[n+1];
    }
    heap[n].next = NULL;
    freenodes = &heap[0];
}
/*creates a heap containing all nodes for the asteroids.
* The heapsize defines how many nodes there will be, and thus how mant asteroids
* can be spawned.
*/
void initialiseAsteroidHeap(void){
    int n;
    for( n=0 ; n<(asteroidHeapsize-1) ; n++) {
        asteroidheap[n].next = &asteroidheap[n+1];
    }
    asteroidheap[n].next = NULL;
    asteroidfreenodes = &asteroidheap[0];
}

/*allocate a node to a missile object, continue until there are no freenodes*/
node_t *allocnode(void){
    node_t *node = NULL;
    if( freenodes ) {
        node = freenodes;
        freenodes = freenodes->next;
    }
    return node;
}

/*allocate a node to an asteroid object, continue until there are no freenodes*/
asteroid_t *asteroidAllocnode(void){
    asteroid_t *node = NULL;
    if( asteroidfreenodes ) {
        node = asteroidfreenodes;
        asteroidfreenodes = asteroidfreenodes->next;
    }
    return node;
}

/*designate an available node for a missile*/
void freenode(node_t *n){
    n->next = freenodes;
    freenodes = n;
}

/*designate an available node for an asteroid*/
void asteroidFreenode(asteroid_t *n){
    n->next = asteroidfreenodes;
    asteroidfreenodes = n;
}
