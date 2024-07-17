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

int timeoutcount = 0; 
int count=0;
int gamestate = 0;
int box_move;
int prime = 1234567;



/* Interrupt Service Routine */
void user_isr(void)
{
   if (IFS(0) & 0x0100)
   { 
      IFSCLR(0) = 0x100; 
      timeoutcount++; 
        if (timeoutcount == 10) // Every 10th interrupt
        {
            timeoutcount = 0;

            switch (gamestate) 
            {
                case 0:
                    clearScreen();
                    draw_icon(icon_row, icon_col, 20);
                    if (getbtns() == 4) {
                        gamestate = 1;
                    }
                    break;

                case 1:
                    clearScreen();
                    draw_icon(icon_row, icon_col, 20);
                    draw_icon(pipe1_row,pipe1_col,28);
                    draw_icon(pipe2_row,pipe2_col,22);
                    draw_icon(pipe3_row,pipe3_col,18);
                    draw_icon(pipe4_row,pipe4_col,26);
                    draw_icon(pipe5_row,pipe5_col,38);  
                    draw_icon(pipe7_row,pipe7_col,14);
                    draw_icon(pipe8_row,pipe8_col,46);
                    draw_icon(pipe9_row,pipe9_col,10);



                    box_move = 2;
                    if (getbtns() == 4) {
                        box_move = -3;
                    }
                    move_icon(icon_row, icon_col, 20, box_move, 0);
                    move_icon(pipe1_row, pipe1_col, 28, 0, -1);  // Example movement for pipe 1
                    move_icon(pipe2_row, pipe2_col, 22, 0, -2);  // Example movement for pipe 2
                    move_icon(pipe3_row, pipe3_col, 18, 0, -3);  // Example movement for pipe 3
                    move_icon(pipe4_row, pipe4_col, 26, 0, -1);  // Example movement for pipe 4
                    move_icon(pipe5_row, pipe5_col, 38, 0, -2); 
                    move_icon(pipe7_row, pipe7_col, 14, 0, -3);  
                    move_icon(pipe8_row, pipe8_col, 46, 0, -1);  
                    move_icon(pipe9_row, pipe9_col, 10, 0, -2);
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
  //Switch 4 interrupt
  *trisE = 0x00 & *trisE;//Sets portE to output, LED lights
  TRISDSET = 0x800; //Sets switch 4 to input
  IPC(4) = 0x7 << 2 | 0x3; //Prioritet
  IEC(0) = (IEC(0) | 0x80000); //Enable switch 4 interrupt


  //Timer2
  TMR2 = 0;          // Initalize timer 2
  T2CONSET = 0x0070; // Set to prescale 1:256
  PR2 = ((80000000 / 256) / 100);
  T2CONSET = 0x8000;

  IPCSET(2) = 6;
  IECSET(0) = 0x100;

  enable_interrupts();
  return;
}

/* This function is called repetitively from the main program */
void labwork( void )
{   
   prime = nextprime(prime);
}

  





