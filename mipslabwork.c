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
int gravity;
int prime = 1234567;
bool initial = true;
bool active = false;
int pos;

/* Interrupt Service Routine */
void user_isr(void) 
{
    if (IFS(0) & 0x0100) 
    { 
        IFSCLR(0) = 0x100; 
        timeoutcount++; 

        if (timeoutcount == 10) 
        { // Every 10th interrupt = 1 second
            timeoutcount = 0;

           switch (gamestate) 
        {
            case 0: // Menu state
            clearScreen(); // Clear the screen to reset any previous drawings
            
            if (initial) 
            {
                display_update(); // Update the display to reflect the cleared screen
                CountDown(); 
                move_obj(icon_X, icon_col, 12, 1, 0);
                initial = false;
            }
            
            draw_icon(icon_X, icon_col, 12);


            // Check if button 4 is pressed
            if (getbtns() == 4) 
            { 
                clearScreen(); // Clear the screen to remove all drawings
                display_update(); // Update the display to reflect the cleared screen
                gamestate = 1; // Transition to the next game state
                initial = true;  // Reset the initial flag for the next state
            }
            break;

            case 1: // Game state
                    
                    clearScreen();
                    draw_top_line();
                    draw_bottom_line();
                    draw_icon(icon_X, icon_col, 12); 

                    if (initial) 
                    {
                        clearScreen();
                        draw_top_line();
                        draw_bottom_line();
                        draw_icon(icon_X, icon_col, 12);
                        delay(1000);
                        gravity = 1;
                        initial = false;
                    }
                
                    
                    if (getbtns() == 4 )
                    {
                        gravity = -2;
                        clearScreen();
                        draw_bottom_line();
                        draw_top_line();
                        draw_icon(icon_X, icon_col, 12);
                        active = true;
                        pos = pos+2;
                       
                    } 
                  
                    else 
                    {
                        clearScreen();
                        draw_bottom_line();
                        draw_top_line();
                        draw_icon(icon_X, icon_col, 12);
                        gravity = 1;
                        pos = pos-1;
                        
                    }
                      // SW1 - Resume the Timer
                        if (IFS(0) & 0x80) {          // Check SW1 interrupt flag
                        T2CON = 0x8070;           // Resume Timer2
                        IFSCLR(0) = 0x80;         // Clear SW1 interrupt flag
                        CountDown();              // Show the countdown
                        gamestate = 1;            // Set the game state to 'Game Running'
                        active = true;            // Reactivate the game loop
                    }

                        // SW3 - Pause the Timer
                        if (IFS(0) & 0x8000) {        // Check SW3 interrupt flag
                            T2CONCLR   = 0x8000;      // Pause Timer2 by clearing the ON bit
                            IFSCLR(0)  = 0x8000;      // Clear SW3 interrupt flag
                        }


                    move_obj(icon_X, icon_col, 12, gravity, 0);

                    if (active) 
                    {    
                        // Draw and move obsts based on their positions and intervals
                        draw_icon(obst1_X, obst1_col, 26);
                        move_obj(obst1_X, obst1_col, 26, 0, -1);

                        draw_icon(obst2_X, obst2_col, 20);
                        move_obj(obst2_X, obst2_col, 20, 0, -1);

                        draw_icon(obst3_X, obst3_col, 16);
                        move_obj(obst3_X, obst3_col, 16, 0, -1);

                        draw_icon(obst4_X, obst4_col, 24);
                        move_obj(obst4_X, obst4_col, 24, 0, -1);

                        draw_icon(obst5_X, obst5_col, 36);
                        move_obj(obst5_X, obst5_col, 36, 0, -1);

                        draw_icon(obst7_X, obst7_col, 26);
                        move_obj(obst7_X, obst7_col, 26, 0, -1);

                        draw_icon(obst8_X, obst8_col, 20);
                        move_obj(obst8_X, obst8_col, 20, 0, -1);

                        draw_icon(obst9_X, obst9_col, 16);
                        move_obj(obst9_X, obst9_col, 16, 0, -1);
                    }
                      
                    // Check for collisions
                    if (collision()) 
                    {
                        active = false;
                        clearScreen();
                        gamestate = 2; // Transition to Game Over state
                    }
                    break;

             case 2: // Game Over state
                clearScreen(); // Clear the screen before showing game over
                draw_icon(restart_X, restart_col, 251); // Draw the restart icon
                if (getbtns() == 1) { // Restart the game when button 1 is pressed
                    clearScreen(); // Clear the screen before restarting
                    move_obj(icon_X, icon_col, 12,pos,0);
                    initial = true;      // Reset initial flag
                    gravity = 1;      // Reset box movement
                    active = false;    // Reset active status
                    pos = 0;        // Reset pos Y
                    gamestate = 0;     // Set gamestate back to the menu

                    reInt();
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
    PR2 = ((80000000 / 256) / 100); // Set period for 10Hz or an interrupt 100 times per second
    T2CONSET = 0x8000; // Enable timer 2
    IPCSET(2) = 0x1F;  // Priority level for Timer2




   // IPCSET(2) = 6; // Set interrupt priority
    IECSET(0) = 0x100; // Enable Timer2 interrupt

    /* SWITCH 3 */
  TRISDSET = 0x400; // Set bit 10 as input
  IECSET(0)=0x8000;
  IPC(3)=0x1F00; //bit 10-12 är priority

  /*SWTICH 1*/
  TRISDSET = 0x100; // Set bit  as input
  IECSET(0)=0x80;
  IPC(1)=0x1F000000;

    /* SWITCH 3 - Pause */
    TRISDSET = 0x400;     // Set bit 10 as input for SW3
  /* SWITCH 3 - Pause */
    TRISDSET   = 0x400;        // Set bit 10 as input for SW3
    IECSET(0) = 0x8000;       // Enable SW3 interrupt (bit 15)
    IPCSET(3)  = 0x1F00;       // Set priority for SW3 (bits 10-15)

    /* SWITCH 1 - Resume */
    TRISDSET   = 0x100;        // Set bit 8 as input for SW1
    IECSET(0) = 0x80;         // Enable SW1 interrupt (bit 7)
    IPCSET(1)= 0x1F000000;   // Set priority for SW1 (bits 24-29)

    enable_interrupts();

    // Initialize icon positions and ensure they are within bounds
    reInt();

    // Initialize player icon position
    move_obj(icon_X, icon_col, 12, -2, 0);

    return;
}

/* This function is called repetitively from the main program */
void labwork(void) {
    if (gamestate == 1 && active) { 
        prime = nextprime(prime);
    } else if (gamestate == 0) {
        
    }
}