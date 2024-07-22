/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog 

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "pic32mx.h"  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
#include <stdio.h>


int mytime = 0x5957;

char textstring[] = "text, more text, and even more text!";
volatile int *trisE;
volatile int *portE;
volatile int *portD;
volatile int *triseD;
volatile int *trisE = (volatile int*) 0xbf886100;
volatile int *portE = (volatile int*) 0xbf886110;
volatile int count2 = 0; // Declare count2 globally and initialize to 0


int timeoutcount = 0; 
int count=0;
int gamestate = 0;
int box_move;
int prime = 1234567;



/* Interrupt Service Routine */
void user_isr(void) {
    if (IFS(0) & 0x0100) { 
        IFSCLR(0) = 0x100; 
        timeoutcount++; 

        if (timeoutcount == 10) { // Every 10th interrupt
            timeoutcount = 0;

            switch (gamestate) {
                case 0: // Menu state
                    clearScreen();
                    draw_icon(icon_row, icon_col, 20);
                    if (getbtns() == 4) {
                        gamestate = 1;
                    }
                    break;

                case 1: // Game state
                    count2++;
                    clearScreen(); // Clear the screen at the beginning of each frame
                                        draw_icon(icon_row, icon_col, 20); // Ensure the player icon is drawn


                    // Draw and move pipes based on their positions and intervals
                    draw_icon(pipe1_row, pipe1_col, 28);
                    move_icon(pipe1_row, pipe1_col, 28, 0, -1);

                    draw_icon(pipe2_row, pipe2_col, 22);
                    move_icon(pipe2_row, pipe2_col, 22, 0, -1);

                    draw_icon(pipe3_row, pipe3_col, 18);
                    move_icon(pipe3_row, pipe3_col, 18, 0, -1);

                    draw_icon(pipe4_row, pipe4_col, 26);
                    move_icon(pipe4_row, pipe4_col, 26, 0, -1);

                    draw_icon(pipe5_row, pipe5_col, 38);
                    move_icon(pipe5_row, pipe5_col, 38, 0, -1);

                    draw_icon(pipe7_row, pipe7_row, 14);
                    move_icon(pipe7_row, pipe7_col, 14, 0, -1);

                    draw_icon(pipe8_row, pipe8_row, 46);
                    move_icon(pipe8_row, pipe8_col, 46, 0, -1);

                    draw_icon(pipe9_row, pipe9_col, 10);
                    move_icon(pipe9_row, pipe9_col, 10, 0, -1);

                    // Player icon movement
                    box_move = 2;
                    if (getbtns() == 4) {
                        box_move = -3;
                    }
                    move_icon(icon_row, icon_col, 20, box_move, 0);

                    break;
            }
        }
    }
}




 /*//SW3
   if (IFS(0) & 0x8000) { 
	   T2CONCLR = 0x8000;
      IFSCLR(0) = 0x8000; 
   }*/



/* Lab-specific initialization goes here */
void labinit(void)
{
    // Initialize switches and interrupts
    *trisE = 0x00 & *trisE; // Sets portE to output, LED lights
    TRISDSET = 0x800; // Sets switch 4 to input
    IPC(4) = 0x7 << 2 | 0x3; // Priority
    IEC(0) = (IEC(0) | 0x80000); // Enable switch 4 interrupt

    // Timer2 initialization
    TMR2 = 0;          // Initialize timer 2
    T2CONSET = 0x0070; // Set to prescale 1:256
    PR2 = ((80000000 / 256) / 100); // Set period for 10Hz
    T2CONSET = 0x8000; // Enable timer 2

    IPCSET(2) = 6; // Set interrupt priority
    IECSET(0) = 0x100; // Enable Timer2 interrupt

    enable_interrupts();

    // Initialize icon positions and ensure they are within bounds
    move_icon(pipe1_row, pipe1_col, 28, 0, 0);
    move_icon(pipe2_row, pipe2_col, 22, 0, 30);
    move_icon(pipe3_row, pipe3_col, 18, 0, 30);
    move_icon(pipe5_row, pipe5_col, 38, 0, 60);
    move_icon(pipe9_row, pipe9_col, 10, 0, 60);
    move_icon(pipe4_row, pipe4_col, 26, 0, 90);
    move_icon(pipe7_row, pipe7_col, 14, 0, 90);
    move_icon(pipe8_row, pipe8_col, 46, 0, 120);

    // Initialize player icon position
    move_icon(icon_row, icon_col, 20, 0, 0);

    return;
}


/* This function is called repetitively from the main program */
void labwork( void )
{   
   prime = nextprime(prime);
}

  





