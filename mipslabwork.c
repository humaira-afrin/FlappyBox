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
int score1 = 0;

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
            //clearScreen(); // Clear the screen to reset any previous drawings

            
                //start(); // Draw the start text
                //display_update(); // Update the display with the start text
            

            // Initialize and draw the movable icon if it's the first frame
            if (first) 
            {
                display_update(); // Update the display to reflect the cleared screen
                CountDown(); 
                move_icon(icon_row, icon_col, 12, 1, 0);
                first = false;
            }
            draw_icon(icon_row, icon_col, 12);

            // Check if button 4 is pressed
            if (getbtns() == 4) 
            { 
                clearScreen(); // Clear the screen to remove all drawings
                display_update(); // Update the display to reflect the cleared screen
                //start();
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
                        delay(2000);
                        box_move = 1;
                        first = false;
                    }
                    
                    if (getbtns() == 4 && !first) 
                    {
                        box_move = -1;
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

                        draw_icon(pipe7_row, pipe7_col, 14);
                        move_icon(pipe7_row, pipe7_col, 14, 0, -1);

                        draw_icon(pipe8_row, pipe8_col, 46);
                        move_icon(pipe8_row, pipe8_col, 46, 0, -1);

                        draw_icon(pipe9_row, pipe9_col, 10);
                        move_icon(pipe9_row, pipe9_col, 10, 0, -1);



                    }

                    // Check for collisions
                    if ((collision_col(pipe1_col, 28) && collision_row(pipe1_row, 28)) ||
                        (collision_margins()) ||
                        (collision_col(pipe2_col, 22) && collision_row(pipe2_row, 22)) ||
                        (collision_col(pipe3_col, 18) && collision_row(pipe3_row, 18)) ||
                        (collision_col(pipe4_col, 26) && collision_row(pipe4_row, 26)) ||
                        (collision_col(pipe5_col, 38) && collision_row(pipe5_row, 38)) ||
                        (collision_col(pipe7_col, 14) && collision_row(pipe7_row, 14)) ||
                        (collision_col(pipe8_col, 46) && collision_row(pipe8_row, 46)) ||
                        (collision_col(pipe9_col, 10) && collision_row(pipe9_row, 10))) 
                    {
                        clearScreen();
                        move_icon(pipe1_row, pipe1_col, 28, 0, 180);
                        move_icon(pipe2_row, pipe2_col, 22, 0, 180);
                        move_icon(pipe3_row, pipe3_col, 18, 0, 180);
                        move_icon(pipe5_row, pipe5_col, 38, 0, 180);
                        move_icon(pipe9_row, pipe9_col, 10, 0, 180);
                        move_icon(pipe4_row, pipe4_col, 26, 0, 180);
                        move_icon(pipe7_row, pipe7_col, 14, 0, 180);
                        move_icon(pipe8_row, pipe8_col, 46, 0, 180);

                      

                        active = false;
                        clearScreen();
                        gamestate = 2; // Transition to Game Over state
                    }
                    break;

             case 2: // Game Over state
                clearScreen(); // Clear the screen before showing game over
                //game_over(); // Show the game over screen
                draw_icon(start_row, start_col, 59); // Draw the arrow to restart

                
                
                if (getbtns() == 1) { // Restart the game when button 1 is pressed
                    clearScreen(); // Clear the screen before restarting
                    
                    // Reset all necessary variables to their initial values
                    first = true; // Reset first flag
                    score1 = 0; // Reset score
                    box_move = 1; // Reset box movement
                    active = false; // Reset active status
                    
                    // Reinitialize pipes and player icon positions
                    move_icon(pipe1_row, pipe1_col, 28, 0, 0);
                    move_icon(pipe2_row, pipe2_col, 22, 0, 30);
                    move_icon(pipe3_row, pipe3_col, 18, 0, 30);
                    move_icon(pipe5_row, pipe5_col, 38, 0, 60);
                    move_icon(pipe9_row, pipe9_col, 10, 0, 60);
                    move_icon(pipe4_row, pipe4_col, 26, 0, 90);
                    move_icon(pipe7_row, pipe7_col, 14, 0, 90);
                    move_icon(pipe8_row, pipe8_col, 46, 0, 120);
                     
                    
                    // Re-initialize the player icon position
                    move_icon(icon_row, icon_col, 12, 0, 0);

                    // Reset any additional necessary states here
                   // Declare the variables outside the loop
                   
                    // Go back to the Menu state
                    gamestate = 1;
                    clearScreen(); // Clear screen again after reinitializing the game
                }
                break;


                default: // Fallback case
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
    move_icon(pipe1_row, pipe1_col, 28, 0, 0);
    move_icon(pipe2_row, pipe2_col, 22, 0, 30);
    move_icon(pipe3_row, pipe3_col, 18, 0, 30);
    move_icon(pipe5_row, pipe5_col, 38, 0, 60);
    move_icon(pipe9_row, pipe9_col, 10, 0, 60);
    move_icon(pipe4_row, pipe4_col, 26, 0, 90);
    move_icon(pipe7_row, pipe7_col, 14, 0, 90);
    move_icon(pipe8_row, pipe8_col, 46, 0, 120);
 

    // Initialize player icon position
    move_icon(icon_row, icon_col, 12, 0, 0);

    return;
}

/* This function is called repetitively from the main program */
void labwork(void) {   
    prime = nextprime(prime);
}
