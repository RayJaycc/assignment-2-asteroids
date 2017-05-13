/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   METHODS TO PASS TO OTHER .cpp FILES FROM CONTROLS
*******************************************************************************************************
*/

typedef enum {JLEFT=0,JDOWN,JRIGHT,JUP,JCENTRE}btnId_t;
typedef enum {no_input=0, input}stickMd_t;
void controls(void);
void playerContinue(void);
