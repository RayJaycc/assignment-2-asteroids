/*
* Serge Gavrilov and Connor Clark
* Students 14005881 and 15005325
* Program submission for CM0506; small embedded systems
*/

/*
*******************************************************************************************************
*   HEADER FOR THE DISPLAY OF THE GAME
*******************************************************************************************************
*/

/*Methods that will be utilised by other .cpp files */
void draw(void);
void drawmissiles(struct missile *lst);
void drawasteroids(struct asteroid *lst);
void shield(void);
void shieldOverlay(void);
void swap_double_buffers(void);
void init_double_buffering(void);
void init_single_buffer(void);
void single_buffer(void);
void gameOverText(void);
void gameOverResp(stickMd_t);
void bitmap(void);
void drawBorder(void);
