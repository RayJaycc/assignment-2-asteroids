/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   HEADER DEFINING MISSILES AND ASTEROIDS FOR USE IN GAME
*******************************************************************************************************
*/

/*Struct for the missiles
  misX, misY are the coordinates of the missile on screen.
	misVX, misVY are the velocity of the missile as it travels.
	misTtl is the lifespan of the missiles.
	struct missile *next is a node in linked list, defining the next 
  missile fired until other action is performed.	
*/
typedef struct missile {
    float misX, misY;
    float misVX, misVY;
    float misTtl;
    struct missile *next;
} node_t;

/*Struct for the asteroid objects
	astX, astY are the coordinates of the asteroid on screen.
	astVX, astVY are the velocity of the asteroids as they travel.
	astTtl is the lifespan of the asteroids in game.
	struct asteroid *next is a node in linked list, defining the next 
  asteroid that can be spawned on the screen.	
*/
typedef struct asteroid {
    float astX, astY;
    float astVX, astVY;
    float astTtl;
    struct asteroid *next;
} asteroid_t;

/*Variables used to define the properties of the ship and asteroids
  declared here for use in other cpp files, e.g. drawing.cpp
*/
extern float shipX;
extern float shipY;
extern int direction;
extern float faceX;
extern float faceY;
extern float leftX;
extern float leftY;
extern float rightX;
extern float rightY;
extern struct missile *active;
extern struct asteroid *asteroidActive;
extern int lives;
extern int shieldHealth;
extern double accel;
extern int timeAlive;
extern int totalTimeAlive;

/*Methods that will be utilised by other .cpp files */
void physics(void);
void checkShipAsteroids(struct asteroid *c);
void checkMissilesAsteroids(struct missile *a, struct asteroid *b);
void update(struct missile *l);
void asteroidUpdate(struct asteroid *l);
void wrapAround(void);
void time(void);
void missileSystem(void);
void asteroidSystem(void);
void initialiseHeap(void);
void initialiseAsteroidHeap(void);
node_t *allocnode(void);
asteroid_t *asteroidAllocnode(void);
void freenode(node_t *n);
void asteroidFreenode(asteroid_t *n);
void initialValues(void);
void defineShip(void);
void speedCheck(void);
void astClearList(struct asteroid *l);
void misClearList(struct missile *r);
