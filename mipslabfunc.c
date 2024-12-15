/* mipslabfunc.c
   This file written 2015 by F Lundevall
   Some parts are original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdbool.h>

/* Declare a helper function which is local to this file */
static void num32asc( char * s, int ); 

#define DISPLAY_CHANGE_TO_COMMAND_MODE (PORTFCLR = 0x10)
#define DISPLAY_CHANGE_TO_DATA_MODE (PORTFSET = 0x10)

#define DISPLAY_ACTIVATE_RESET (PORTGCLR = 0x200)
#define DISPLAY_DO_NOT_RESET (PORTGSET = 0x200)

#define DISPLAY_ACTIVATE_VDD (PORTFCLR = 0x40)
#define DISPLAY_ACTIVATE_VBAT (PORTFCLR = 0x20)

#define DISPLAY_TURN_OFF_VDD (PORTFSET = 0x40)
#define DISPLAY_TURN_OFF_VBAT (PORTFSET = 0x20)


/* quicksleep:
   A simple function to create a small delay.
   Very inefficient use of computing resources,
   but very handy in some special cases. */
void quicksleep(int cyc) {
	int i;
	for(i = cyc; i > 0; i--);
}

/* tick:
   Add 1 to time in memory, at location pointed to by parameter.
   Time is stored as 4 pairs of 2 NBCD-digits.
   1st pair (most significant byte) counts days.
   2nd pair counts hours.
   3rd pair counts minutes.
   4th pair (least significant byte) counts seconds.
   In most labs, only the 3rd and 4th pairs are used. */
void tick( unsigned int * timep )
{
  /* Get current value, store locally */
  register unsigned int t = * timep;
  t += 1; /* Increment local copy */
  
  /* If result was not a valid BCD-coded time, adjust now */

  if( (t & 0x0000000f) >= 0x0000000a ) t += 0x00000006;
  if( (t & 0x000000f0) >= 0x00000060 ) t += 0x000000a0;
  /* Seconds are now OK */

  if( (t & 0x00000f00) >= 0x00000a00 ) t += 0x00000600;
  if( (t & 0x0000f000) >= 0x00006000 ) t += 0x0000a000;
  /* Minutes are now OK */

  if( (t & 0x000f0000) >= 0x000a0000 ) t += 0x00060000;
  if( (t & 0x00ff0000) >= 0x00240000 ) t += 0x00dc0000;
  /* Hours are now OK */

  if( (t & 0x0f000000) >= 0x0a000000 ) t += 0x06000000;
  if( (t & 0xf0000000) >= 0xa0000000 ) t = 0;
  /* Days are now OK */

  * timep = t; /* Store new value */
}

/* display_debug
   A function to help debugging.

   After calling display_debug,
   the two middle lines of the display show
   an address and its current contents.

   There's one parameter: the address to read and display.

   Note: When you use this function, you should comment out any
   repeated calls to display_image; display_image overwrites
   about half of the digits shown by display_debug.
*/   
void display_debug( volatile int * const addr )
{
  display_string( 1, "Addr" );
  display_string( 2, "Data" );
  num32asc( &textbuffer[1][6], (int) addr );
  num32asc( &textbuffer[2][6], *addr );
  display_update();
}

uint8_t spi_send_recv(uint8_t data) {
	while(!(SPI2STAT & 0x08));
	SPI2BUF = data;
	while(!(SPI2STAT & 1));
	return SPI2BUF;
}

void display_init(void) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;
	quicksleep(10);
	DISPLAY_ACTIVATE_VDD;
	quicksleep(1000000);
	
	spi_send_recv(0xAE);
	DISPLAY_ACTIVATE_RESET;
	quicksleep(10);
	DISPLAY_DO_NOT_RESET;
	quicksleep(10);
	
	spi_send_recv(0x8D);
	spi_send_recv(0x14);
	
	spi_send_recv(0xD9);
	spi_send_recv(0xF1);
	
	DISPLAY_ACTIVATE_VBAT;
	quicksleep(10000000);
	
	spi_send_recv(0xA1);
	spi_send_recv(0xC8);
	
	spi_send_recv(0xDA);
	spi_send_recv(0x20);
	
	spi_send_recv(0xAF);
}

void display_string(int line, char *s) {
	int i;
	if(line < 0 || line >= 4)
		return;
	if(!s)
		return;
	
	for(i = 0; i < 16; i++)
		if(*s) {
			textbuffer[line][i] = *s;
			s++;
		} else
			textbuffer[line][i] = ' ';
}

void display_image(int x, const uint8_t *data) {
	int i, j;
	
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;

		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(x & 0xF);
		spi_send_recv(0x10 | ((x >> 4) & 0xF));
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 128; j++)
			spi_send_recv(~data[i*128 + j]);
	}
}

void display_box(int yOffset, const uint8_t *data) {
    int i, j;
    
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);
        spi_send_recv((i - yOffset) & 0xF);  // Corrected yOffset calculation

        spi_send_recv(0);  // No xOffset for y-axis offset
        spi_send_recv(0x10 | ((i - yOffset) >> 4) & 0xF);  // Corrected yOffset calculation

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 8; j++)
            spi_send_recv(~data[i * 8 + j]);
    }
}


void display_update(void) {
	int i, j, k;
	int c;
	for(i = 0; i < 4; i++) {
		DISPLAY_CHANGE_TO_COMMAND_MODE;
		spi_send_recv(0x22);
		spi_send_recv(i);
		
		spi_send_recv(0x0);
		spi_send_recv(0x10);
		
		DISPLAY_CHANGE_TO_DATA_MODE;
		
		for(j = 0; j < 16; j++) {
			c = textbuffer[i][j];
			if(c & 0x80)
				continue;
			
			for(k = 0; k < 8; k++)
				spi_send_recv(font[c*8 + k]);
		}
	}
}

/* Helper function, local to this file.
   Converts a number to hexadecimal ASCII digits. */
static void num32asc( char * s, int n ) 
{
  int i;
  for( i = 28; i >= 0; i -= 4 )
    *s++ = "0123456789ABCDEF"[ (n >> i) & 15 ];
}

/*
 * nextprime
 * 
 * Return the initial prime number larger than the integer
 * given as a parameter. The integer must be positive.
 */
#define PRIME_FALSE   0     /* Constant to help readability. */
#define PRIME_TRUE    1     /* Constant to help readability. */
int nextprime( int inval )
{
   register int perhapsprime = 0; /* Holds a tentative prime while we check it. */
   register int testfactor; /* Holds various factors for which we test perhapsprime. */
   register int found;      /* Flag, false until we find a prime. */

   if (inval < 3 )          /* Initial sanity check of parameter. */
   {
     if(inval <= 0) return(1);  /* Return 1 for zero or negative input. */
     if(inval == 1) return(2);  /* Easy special case. */
     if(inval == 2) return(3);  /* Easy special case. */
   }
   else
   {
     /* Testing an even number for primeness is pointless, since
      * all even numbers are divisible by 2. Therefore, we make sure
      * that perhapsprime is larger than the parameter, and odd. */
     perhapsprime = ( inval + 1 ) | 1 ;
   }
   /* While prime not found, loop. */
   for( found = PRIME_FALSE; found != PRIME_TRUE; perhapsprime += 2 )
   {
     /* Check factors from 3 up to perhapsprime/2. */
     for( testfactor = 3; testfactor <= (perhapsprime >> 1) + 1; testfactor += 1 )
     {
       found = PRIME_TRUE;      /* Assume we will find a prime. */
       if( (perhapsprime % testfactor) == 0 ) /* If testfactor divides perhapsprime... */
       {
         found = PRIME_FALSE;   /* ...then, perhapsprime was non-prime. */
         goto check_next_prime; /* Break the inner loop, go test a new perhapsprime. */
       }
     }
     check_next_prime:;         /* This label is used to break the inner loop. */
     if( found == PRIME_TRUE )  /* If the loop ended normally, we found a prime. */
     {
       return( perhapsprime );  /* Return the prime we found. */
     } 
   }
   return( perhapsprime );      /* When the loop ends, perhapsprime is a real prime. */
} 

/*
 * itoa
 * 
 * Simple conversion routine
 * Converts binary to decimal numbers
 * Returns pointer to (static) char array
 * 
 * The integer argument is converted to a string
 * of digits representing the integer in decimal format.
 * The integer is considered signed, and a minus-sign
 * precedes the string of digits if the number is
 * negative.
 * 
 * This routine will return a varying number of digits, from
 * one digit (for integers in the range 0 through 9) and up to
 * 10 digits and a leading minus-sign (for the largest negative
 * 32-bit integers).
 * 
 * If the integer has the special value
 * 100000...0 (that's 31 zeros), the number cannot be
 * negated. We check for this, and treat this as a special case.
 * If the integer has any other value, the sign is saved separately.
 * 
 * If the integer is negative, it is then converted to
 * its positive counterpart. We then use the positive
 * absolute value for conversion.
 * 
 * Conversion produces the least-significant digits initial,
 * which is the reverse of the order in which we wish to
 * print the digits. We therefore store all digits in a buffer,
 * in ASCII form.
 * 
 * To avoid a separate step for reversing the contents of the buffer,
 * the buffer is initialized with an end-of-string marker at the
 * very end of the buffer. The digits produced by conversion are then
 * stored right-to-left in the buffer: starting with the position
 * immediately before the end-of-string marker and proceeding towards
 * the beginning of the buffer.
 * 
 * For this to work, the buffer size must of course be big enough
 * to hold the decimal representation of the largest possible integer,
 * and the minus sign, and the trailing end-of-string marker.
 * The value 24 for ITOA_BUFSIZ was selected to allow conversion of
 * 64-bit quantities; however, the size of an int on your current compiler
 * may not allow this straight away.
 */
#define ITOA_BUFSIZ ( 24 )
char * itoaconv( int num )
{
  register int i, sign;
  static char itoa_buffer[ ITOA_BUFSIZ ];
  static const char maxneg[] = "-2147483648";
  
  itoa_buffer[ ITOA_BUFSIZ - 1 ] = 0;   /* Insert the end-of-string marker. */
  sign = num;                           /* Save sign. */
  if( num < 0 && num - 1 > 0 )          /* Check for most negative integer */
  {
    for( i = 0; i < sizeof( maxneg ); i += 1 )
    itoa_buffer[ i + 1 ] = maxneg[ i ];
    i = 0;
  }
  else
  {
    if( num < 0 ) num = -num;           /* Make number positive. */
    i = ITOA_BUFSIZ - 2;                /* Location for initial ASCII digit. */
    do {
      itoa_buffer[ i ] = num % 10 + '0';/* Insert next digit. */
      num = num / 10;                   /* Remove digit from number. */
      i -= 1;                           /* Move index to next empty position. */
    } while( num > 0 );
    if( sign < 0 )
    {
      itoa_buffer[ i ] = '-';
      i -= 1;
    }
  }
  /* Since the loop always sets the index i to the next empty position,
   * we must add 1 in order to return a pointer to the initial occupied position. */
  return( &itoa_buffer[ i + 1 ] );
}
/*****************************************************/

/*void clearScreenMemory (void){
	int i = 0;
	for(i = 0; i<512;i++){
		icon[i] = 255;
	}
}*/

void CountDown (void) 
{
         display_string(2, " FLAPPY BOX");
	        display_update();
	        delay( 1000 );
	        display_string(2, "      3");
	        display_update();
	        delay( 1000 );
	        display_string(2, "      2");
	        display_update();
	        delay( 1000 );
	        display_string(2, "      1");
	        display_update();
	        delay( 1000);
	        display_string(2, "   Press 1");
	        display_update();
	        delay( 200 );  
	       //display_update();
}


void merge(uint8_t *nybild,  uint8_t *icon,  uint8_t *triangle) {
    int i; 
    for (i = 0; i < 128 * 4; ++i) {
        nybild[i] = icon[i] & triangle[i];  
        if ((icon[i] && triangle[i]) == 0) {
          gameState=1;
        } 
    } 
}

// void display_score(int line, int n) {
//     int i;
//     char int_str[16];  
//     if (line < 0 || line >= 4)
//         return;
//     num32asc(int_str, n);
//     for (i = 0; i < 16 && int_str[i] != '\0'; i++) {
//         textbuffer[line][i] = int_str[i];
//     }
//     for (; i < 16; i++) { //resten spaces
//         textbuffer[line][i] = ' ';
//     }
// }
void highScoreCheck(){
  if (score > highScore) {
     highScore = score;
  }
}

/*###############################################################################*/
uint8_t memArr[1024] = {0}; 

void mark_pixel(int X, int col){  
  if(X < 0) { X = 0; } // Ensures the X coordinate (row) is not negative.
  if(X > 31) { X = 31; }

  if(col < 0) { col = 0; }
  if(col > 127) { col = 127; }

  int XIndex = X / 8; //page number
  int bit_pos = X % 8; //exakt position of the row in the page
  int bitmask = 1 << bit_pos; // to modufy that bit position row 8 is 0 row 9 is 1 roe 10 is 2
 // om bit pis = 2 third row, bit mask will be 100 turn on third pixel

  show_pixel(XIndex, col, bitmask);

}

void show_pixel(int X, int col, int pos) { 
    uint8_t x = memArr[X*128 + col]; // Get the current pixel value in memory
    if (pos == 0) {  // edge case, safe
        x = x & ~pos; // Clear the bit (
    } else {
        x = x | pos;  // Set the bit (turn on the pixel)
    }
    memArr[X*128 + col] = x; // Update the pixel in memory

    // Set the page and column address for display update
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);  // Command to set page address
    spi_send_recv(X);   // Set the X (page) address
    spi_send_recv(X);   // Same value for start and end address (single X)
    spi_send_recv(0x21);  // Command to set column address
    spi_send_recv(col);   // Set column (x-axis) address
    spi_send_recv(col);   // Same value for start and end address (single column)

    DISPLAY_CHANGE_TO_DATA_MODE;
    spi_send_recv(memArr[X*128 + col]);  // Send the updated pixel value to the display
}


void draw_icon(uint8_t* data_X, uint8_t* data_col, int size){   //ritar hela iconen med hjälp av en forloop som loopar igenom varje pixel man har på Xs och columner så att hela iconen kan displayas
  int i;
  for(i = 0; i < size; i++){
    mark_pixel(data_X[i], data_col[i]);
  }
}

void move_obj(uint8_t* data_X, uint8_t* data_col,int iconsize,int Xmovment, int colmovment){  // flyttar hela ikonen genom att lägga till det vi skickar in i X- /colmovment i iconens Xs och col pixlar. 
  int i;
  for(i = 0; i < iconsize; i++){ //kanske måste lägga till en ny size så att de fungerar på pipsen 
    data_X[i] += Xmovment;
    data_col[i] += colmovment;
  }
}


void clearScreen() {
    int i, j;
    
    // Clear the array used for storing pixel data (reset all pixels to 0)
    for(i = 0; i < 1024; i++) {
        memArr[i] = 0;  // Set all bytes to 0 (clearing the screen in memory)
    }

    // Reset the display by writing zero to all screen data
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        // Set the page address for the current page (0, 1, 2, 3)
        spi_send_recv(0x22);   // Set page address command
        spi_send_recv(i);      // Set the page number (0-3)

        // Set the start and end column for the page (0 to 127)
        spi_send_recv(0x00);   // Start column (0)
        spi_send_recv(0x10);   // rightmost column (128

        DISPLAY_CHANGE_TO_DATA_MODE;

        // Write zeros to all columns for the current page (clear the pixels on the screen)
        for(j = 0; j < 128; j++) {
            spi_send_recv(0);  // Send zero to clear the pixel
        }
    }
}


bool collision_col(uint8_t* obst_col, int size) {
  int i;
    for ( i = 0; i < size; i++) { // Fixed loop condition
        int col_dis = obst_col[i] - icon_col[3];
        if (col_dis < 2 && col_dis >= 0) { // Adjust range if needed
            return true;
        }
    }
    return false; 
}

bool collision_X(uint8_t* obst_X, int size) {
  int i;
    for ( i = 0; i < size; i++) { // 
        int X_dis1 = icon_X[1] - obst_X[i];
        int X_dis2 = obst_X[i] - icon_X[4];
        if ((X_dis1 < 2 && X_dis1 >= 0) || (X_dis2 < 2 && X_dis2 >= 0)) { // 
            return true;
        }
    }
    return false; 
}

bool collision_margins() {
  int i;
    for ( i = 0; i < 12; i++) { //  icon_X has 12 elements
        if (icon_X[i] <= 1 || icon_X[i] >= 30) { // Adjust for partial overlaps
            return true;
        }
    }
    return false; 
}

bool collision() {
    if (collision_margins()) { // Check margins initial
        return true;
    }
    return (collision_col(obst1_col, 26) && collision_X(obst1_X, 26)) ||
           (collision_col(obst2_col, 20) && collision_X(obst2_X, 20)) ||
           (collision_col(obst3_col, 16) && collision_X(obst3_X, 16)) ||
           (collision_col(obst4_col, 24) && collision_X(obst4_X, 24)) ||
           (collision_col(obst5_col, 36) && collision_X(obst5_X, 36)) ||
           (collision_col(obst7_col, 26) && collision_X(obst7_X, 26)) ||
           (collision_col(obst8_col, 20) && collision_X(obst8_X, 20)) ||
           (collision_col(obst9_col, 16) && collision_X(obst9_X, 16));
}


void draw_top_line() {
    int size = 128; // Total number of pixels in one X (128 columns)
    uint8_t data_X[128];
    uint8_t data_col[128];
    int i;
    
    // Populate the arrays
    for ( i = 0; i < size; i++) {
        data_X[i] = 0;   // Top X (X index 0)
        data_col[i] = i;   // All columns from 0 to 127
    }
    
    // Draw the line
    draw_icon(data_X, data_col, size);
}
void draw_bottom_line() {
    int size = 128; // Total number of pixels in one X (128 columns)
    uint8_t data_X[128];
    uint8_t data_col[128];
    int i;
    
    // Populate the arrays
    for ( i = 0; i < size; i++) {
        data_X[i] = 128;   // Top X (X index 0)
        data_col[i] = i;   // All columns from 0 to 127
    }
    
    // Draw the line
    draw_icon(data_X, data_col, size);
}

void game_over(){
  display_string(1,"   GAME OVER");
  display_update();
  delay (1000);
   draw_icon(restart_X, restart_col, 251);
   display_update();
   delay (1000);
}

void start(){
  display_string(1,"   START");
  display_string(2,"   PRESS 1");
  display_update();

}




void reInt() {
    // Reinitialize obsts' positions 
    move_obj(obst1_X, obst1_col, 26, 0, 0);
    move_obj(obst3_X, obst3_col, 16, 0, 20);

    move_obj(obst2_X, obst2_col, 20, 0, 40);
    move_obj(obst4_X, obst4_col, 24, 0, 60);
    move_obj(obst7_X, obst7_col, 26, 0, 100);
    move_obj(obst5_X, obst5_col, 36, 0, 80);

    move_obj(obst8_X, obst8_col, 20, 0, 110); 
    move_obj(obst9_X, obst9_col, 16, 0, 120); 
    move_obj(icon_X, icon_col, 12, 0, 0);   
  }

 







