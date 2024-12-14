/* mipslabwork.c
   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall
   This file modified 2017-04-31 by Ture Teknolog */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include "pic32mx.h"  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declarations for these labs */
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
int count = 0;
int gamestate = 0;
int box_move;
int prime = 1234567;
bool first = true;
bool active = false;
int score1;

/* Interrupt Service Routine */
void user_isr(void) 
{
    if (IFS(0) & 0x0100) 
    { 
        IFSCLR(0) = 0x100; 
        timeoutcount++; 

        if (timeoutcount == 10) 
        { // Every 10th interrupt
            timeoutcount = 0;

           switch (gamestate) 
{
            case 0: // Menu state
            clearScreen(); // Clear the screen to reset any previous drawings
            
            if (first) 
            {
                display_update(); // Update the display to reflect the cleared screen
                CountDown(); 
                move_icon(icon_row, icon_col, 12, 1, 0);
                first = false;
            }
             
            // move_icon(icon_row, icon_col, 12, -3, 0);
            draw_icon(icon_row, icon_col, 12);

            // Check if button 4 is pressed
            if (getbtns() == 4) 
            { 
                clearScreen(); // Clear the screen to remove all drawings
                display_update(); // Update the display to reflect the cleared screen
                gamestate = 1; // Transition to the next game state
                first = true;  // Reset the first flag for the next state
            }
            break;

            case 1: // Game state
                    count2++;
                    clearScreen();
                    draw_top_line();
                    draw_bottom_line();
                    draw_icon(icon_row, icon_col, 12); 

                    if (first) 
                    {
                        clearScreen();
                        draw_top_line();
                        draw_bottom_line();
                        draw_icon(icon_row, icon_col, 12);
                        delay(1000);
                        box_move = 1;
                        first = false;
                    }
                   
                    
                    if (getbtns() == 4 && !first)
                    {
                        box_move = -2;
                        clearScreen();
                        draw_bottom_line();
                        draw_top_line();
                        draw_icon(icon_row, icon_col, 12);
                        active = true;
                       
                    } 
                  
                    else 
                    {
                        clearScreen();
                        draw_bottom_line();
                        draw_top_line();
                        draw_icon(icon_row, icon_col, 12);
                        box_move = 1;
                        
                    }

                    move_icon(icon_row, icon_col, 12, box_move, 0);

                    if (active) 
                    {    
                        // Draw and move pipes based on their positions and intervals
                        draw_icon(pipe1_row, pipe1_col, 26);
                        move_icon(pipe1_row, pipe1_col, 26, 0, -1);

                        draw_icon(pipe2_row, pipe2_col, 20);
                        move_icon(pipe2_row, pipe2_col, 20, 0, -1);

                        draw_icon(pipe3_row, pipe3_col, 16);
                        move_icon(pipe3_row, pipe3_col, 16, 0, -1);

                        draw_icon(pipe4_row, pipe4_col, 24);
                        move_icon(pipe4_row, pipe4_col, 24, 0, -1);

                        draw_icon(pipe5_row, pipe5_col, 36);
                        move_icon(pipe5_row, pipe5_col, 36, 0, -1);

                        draw_icon(pipe7_row, pipe7_col, 26);
                        move_icon(pipe7_row, pipe7_col, 26, 0, -1);

                        draw_icon(pipe8_row, pipe8_col, 20);
                        move_icon(pipe8_row, pipe8_col, 20, 0, -1);

                        draw_icon(pipe9_row, pipe9_col, 16);
                        move_icon(pipe9_row, pipe9_col, 16, 0, -1);
                    }

                    // Check for collisions
                    if (collision()) 
                    {
                        move_icon(pipe1_row, pipe1_col, 26, 0, 180);
                        move_icon(pipe2_row, pipe2_col, 20, 0, 180);
                        move_icon(pipe3_row, pipe3_col, 16, 0, 180);
                        move_icon(pipe5_row, pipe4_col, 24, 0, 180);
                        move_icon(pipe9_row, pipe5_col, 36, 0, 180);
                        move_icon(pipe4_row, pipe7_col, 26, 0, 180);
                        move_icon(pipe7_row, pipe8_col, 20, 0, 180);
                        move_icon(pipe8_row, pipe9_col, 16, 0, 180);

                        active = false;
                        clearScreen();
                        gamestate = 2; // Transition to Game Over state
                    }
                    break;

             case 2: // Game Over state
                clearScreen(); // Clear the screen before showing game over
                draw_icon(restart_row, restart_col, 251); // Draw the restart icon
                
                if (getbtns() == 1) { // Restart the game when button 1 is pressed
                    clearScreen(); // Clear the screen before restarting

                    // Reinitialize all relevant variables
                    first = true;      // Reset first flag
                    box_move = 1;      // Reset box movement
                    active = false;    // Reset active status
                    score1 = 0;        // Reset score
                    gamestate = 0;     // Set gamestate back to the menu

                    // Reinitialize pipes and player icon positions
                    reInt();

                    // Reinitialize the player icon position
                    move_icon(icon_row, icon_col, 12, 0, 0);

                    clearScreen(); // Clear screen again after reinitializing the game
                }
                break;

             break;
            } // End of switch
        } // End of timeoutcount == 10 check
    } // End of IFS(0) & 0x0100 check
}


/* Lab-specific initialization goes here */
void labinit(void)
{
    *trisE = 0x00 & *trisE; // Set portE to output, LED lights
    TRISDSET = 0x800; // Set switch 4 to input
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
    reInt();

    // Initialize player icon position
    move_icon(icon_row, icon_col, 12, 0, 0);

    return;
}

/* This function is called repetitively from the main program */
void labwork(void) {   
    prime = nextprime(prime);
}
