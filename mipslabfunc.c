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
 * Return the first prime number larger than the integer
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
 * Conversion produces the least-significant digits first,
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
    i = ITOA_BUFSIZ - 2;                /* Location for first ASCII digit. */
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
   * we must add 1 in order to return a pointer to the first occupied position. */
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
         display_string(2, "FLAPPY BOX");
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
	        display_string(2, "   LEVEL 1");
	        display_update();
	        delay( 1000 );
	       display_update();
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

void display_score(int line, int n) {
    int i;
    char int_str[16];  
    if (line < 0 || line >= 4)
        return;
    num32asc(int_str, n);
    for (i = 0; i < 16 && int_str[i] != '\0'; i++) {
        textbuffer[line][i] = int_str[i];
    }
    for (; i < 16; i++) { //resten spaces
        textbuffer[line][i] = ' ';
    }
}
void highScoreCheck(){
  if (score > highScore) {
     highScore = score;
  }
}
void start(){
  display_string(1,"   START");
  display_string(2,"   PRESS 1");
  display_update();
}
/*###############################################################################*/
uint8_t myArray[1024] = {0}; 

void draw_pixel(int row, int col){  // ritar en pixel på den row och col koordinat man skickar in, row∈[0,31], col∈[0,127]
  if(row < 0) { row = 0; }
  if(row > 31) { row = 31; }

  if(col < 0) { col = 0; }
  if(col > 127) { col = 127; }

  int rowIndex = row / 8; //page number
  int bit_pos = row % 8; //pixel number 
  int bitmask = 1 << bit_pos;


  display_pixel(rowIndex, col, bitmask);

}

void display_pixel(int row, int col, int val) { 
    uint8_t x = myArray[row*128 + col]; // Get the current pixel value in memory
    x = (val == 0) ? (x & ~val) : (x | val); // Only set or clear the bit based on val
    myArray[row*128 + col] = x; // Update the pixel in memory

    // Set the page and column address for display update
    DISPLAY_CHANGE_TO_COMMAND_MODE;
    spi_send_recv(0x22);  // Command to set page address
    spi_send_recv(row);   // Set the row (page) address
    spi_send_recv(row);   // Same value for start and end address (single row)
    spi_send_recv(0x21);  // Command to set column address
    spi_send_recv(col);   // Set column (x-axis) address
    spi_send_recv(col);   // Same value for start and end address (single column)

    DISPLAY_CHANGE_TO_DATA_MODE;
    spi_send_recv(myArray[row*128 + col]);  // Send the updated pixel value to the display
}


void draw_icon(uint8_t* data_row, uint8_t* data_col, int size){   //ritar hela iconen med hjälp av en forloop som loopar igenom varje pixel man har på rows och columner så att hela iconen kan displayas
  int i;
  for(i = 0; i < size; i++){
    draw_pixel(data_row[i], data_col[i]);
  }
}

void move_icon(uint8_t* data_row, uint8_t* data_col,int iconsize,int rowmovment, int colmovment){  // flyttar hela ikonen genom att lägga till det vi skickar in i row- /colmovment i iconens rows och col pixlar. 
  int i;
  for(i = 0; i < iconsize; i++){ //kanske måste lägga till en ny size så att de fungerar på pipsen 
    data_row[i] += rowmovment;
    data_col[i] += colmovment;
  }
}

void clearScreen(){
    int i, j;
    // Clear the array used for storing pixel data
    for(i = 0; i < 1024; i++){
        myArray[i] = 0;
    }

    // Reset the display by writing zero to all screen data
    for(i = 0; i < 4; i++) {
        DISPLAY_CHANGE_TO_COMMAND_MODE;

        spi_send_recv(0x22);   // Set page address
        spi_send_recv(i);      // Set page number
        spi_send_recv(0 & 0xF); // Start column
        spi_send_recv(0x10 | ((0 >> 4) & 0xF)); // End column

        DISPLAY_CHANGE_TO_DATA_MODE;

        for(j = 0; j < 128; j++) {
            spi_send_recv(0);  // Send zero to all pixels (clear screen)
        }
    }
}


bool collision_col(uint8_t* pipe_col, int size){
  int i;
  for(i = 0; i <= size; i++){
    int col_dis = pipe_col[i] - icon_col[3];
    if(col_dis < 4 && col_dis >= 0){
        return true;
    }
  }
  return false; 
}
bool collision_row(uint8_t* pipe_row, int size){
  int i;
  for(i = 0; i <= size; i++){
    int row_dis1 = icon_row[1] - pipe_row[i];
    int row_dis2 = pipe_row[i]-icon_row[4];
    if((row_dis1 < 4 && row_dis1 >= 0)||(row_dis2 < 4 && row_dis2 >= 0))
    {
        return true;
    }
  }
  return false; 
}
bool collision_margins() {
  int i;
    for ( i = 0; i < 12; i++) {
        if (icon_row[i] == 0) {
            return true; // Collision with top margin
        }
    }

    for ( i = 0; i <12; i++) {
        if (icon_row[i] == 31) {
            return true; // Collision with bottom margin
        }
    }

    return false; // No collision detected
}


   

void draw_top_line() {
    int size = 128; // Total number of pixels in one row (128 columns)
    uint8_t data_row[128];
    uint8_t data_col[128];
    int i;
    
    // Populate the arrays
    for ( i = 0; i < size; i++) {
        data_row[i] = 0;   // Top row (row index 0)
        data_col[i] = i;   // All columns from 0 to 127
    }
    
    // Draw the line
    draw_icon(data_row, data_col, size);
}

void draw_bottom_line() {
    int size = 128; // Total number of pixels in one row (128 columns)
    uint8_t data_row[128];
    uint8_t data_col[128];
    int i;
    
    // Populate the arrays
    for ( i = 0; i < size; i++) {
        data_row[i] = 128;   // Top row (row index 0)
        data_col[i] = i;   // All columns from 0 to 127
    }
    
    // Draw the line
    draw_icon(data_row, data_col, size);
}
void game_over(){
  display_string(1,"   GAME OVER");
    display_string(2,"   PRESS 2");

  display_update();
}






 







