/* mipslab.h
   Header file for all labs.
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   Latest update 2015-08-28 by F Lundevall

   For copyright and licensing, see file COPYING */

/* Declare display-related functions from mipslabfunc.c */
#include <stdbool.h>
void display_image(int x, const uint8_t *data);
void display_init(void);
void display_string(int line, char *s);
void display_update(void);
uint8_t spi_send_recv(uint8_t data);

/* Declare lab-related functions from mipslabfunc.c */
extern const int decimalPosToBinary[8];
void mark_pixel(int X, int col);
void show_pixel(int X, int col, int val);
void draw_icon(uint8_t* data_X, uint8_t* data_col, int size);
void move_obj(uint8_t* data_X, uint8_t* data_col,int iconsize, int Xmovment, int colmovment);
void clearScreen();
bool collision_col(uint8_t* obst_col, int size);
bool collision_X(uint8_t* obst_X, int size);
void draw_top_line();
void draw_bottom_line();
bool collision_margins();


char * itoaconv( int num );
void labwork(void);
int nextprime( int inval );
void quicksleep(int cyc);
void tick( unsigned int * timep );

/* Declare display_debug - a function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/
void display_debug( volatile int * const addr );

/* Declare bitmap array containing font */
extern const uint8_t const font[128*8];
/* Declare bitmap array containing icon */
extern uint8_t icon_X[12];
extern uint8_t icon_col[12];
extern uint8_t arX_X[18];
extern uint8_t arX_col[18];


extern uint8_t start_X[59];
extern uint8_t start_col[59];
extern uint8_t restart_X[251];
extern uint8_t restart_col[251];
/**** obstS ***** */

extern uint8_t obst1_X[26];
extern uint8_t obst1_col[26];
extern uint8_t obst2_X[20];
extern uint8_t obst2_col[20];
extern uint8_t obst3_X[16];
extern uint8_t obst3_col[16];
extern uint8_t obst4_X[24];
extern uint8_t obst4_col[24];
extern uint8_t obst5_X[36];
extern uint8_t obst5_col[36];
extern uint8_t obst7_X[26];
extern uint8_t obst7_col[26];
extern uint8_t obst8_X[20];
extern uint8_t obst8_col[20];
extern uint8_t obst9_X[16];
extern uint8_t obst9_col[16];

extern uint8_t mainstart_X[59];
extern uint8_t mainstart_col[59];
//extern const uint8_t const icon2[128];
  uint8_t triangle[128*4];
  uint8_t nybild[128*4];
  uint8_t triangle1[128*4];
    uint8_t triangle2[128*4];

extern const uint8_t const obs1[128];
extern const uint8_t const box[4*8];
extern uint8_t gameover_X[235];
extern uint8_t gameover_col[235];

int gameState;
extern countStart;
int score;
int highScore;




/* Declare text buffer for display output */
extern char textbuffer[4][16];

/* Declare functions written by students.
   Note: Since we declare these functions here,
   students must define their functions with the exact types
   specified in the laboratory instructions. */
/* Written as part of asm lab: delay, time2string */
void delay(int);
void time2string( char *, int );
/* Written as part of i/o lab: getbtns, getsw, enable_interrupt */
int getbtns(void);
int getsw(void);
void enable_interrupt(void);
