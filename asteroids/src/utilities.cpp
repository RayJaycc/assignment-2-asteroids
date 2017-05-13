/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   METHODS THAT WILL BE INVOKED BY PHYSICS FOR THE PURPOSE OF DEFINING AND SPAWNING
*******************************************************************************************************
*/

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/*used for defining how the asteroids will travel across the screen*/
int randrange(int from, int to){
	int range = to-from;
	return rand()%range + from;
}

/*used to define how missiles will travel across the screen*/
float radians(float a){
	const float pi = 3.141592;
	return a * pi /180.0f;
}
