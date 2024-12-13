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
void draw_pixel(int row, int col);
void display_pixel(int row, int col, int val);
void draw_icon(uint8_t* data_row, uint8_t* data_col, int size);
void move_icon(uint8_t* data_row, uint8_t* data_col,int iconsize, int rowmovment, int colmovment);
void clearScreen();
bool collision_col(uint8_t* pipe_col, int size);
bool collision_row(uint8_t* pipe_row, int size);
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
 extern uint8_t icon_row[12];
extern uint8_t icon_col[12];
extern uint8_t arrow_row[18];
extern uint8_t arrow_col[18];


extern uint8_t start_row[59];
extern uint8_t start_col[59];

/**** PIPES ***** */
extern uint8_t pipe1_row[28];
extern uint8_t pipe1_col[28];
extern uint8_t pipe2_row[22];
extern uint8_t pipe2_col[22];
extern uint8_t pipe3_row[18];
extern uint8_t pipe3_col[18];
extern uint8_t pipe4_row[26];
extern uint8_t pipe4_col[26];
extern uint8_t pipe5_row[38];
extern uint8_t pipe5_col[38];
extern uint8_t pipe6_row[48];
extern uint8_t pipe6_col[48];
extern uint8_t pipe7_row[14];
extern uint8_t pipe7_col[14];
extern uint8_t pipe8_row[46];
extern uint8_t pipe8_col[46];
extern uint8_t pipe9_row[10];
extern uint8_t pipe9_col[10];

extern uint8_t mainstart_row[59];
extern uint8_t mainstart_col[59];
//extern const uint8_t const icon2[128];
  uint8_t triangle[128*4];
  uint8_t nybild[128*4];
  uint8_t triangle1[128*4];
    uint8_t triangle2[128*4];

extern const uint8_t const obs1[128];
extern const uint8_t const box[4*8];
extern uint8_t gameover_row[235];
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
