/* mipslabdata.c
   This file compiled 2015 by F Lundevall
   from original code written by Axel Isaksson

   For copyright and licensing, see file COPYING */

#include <stdint.h>   /* Declarations of uint_32 and the like */
#include <pic32mx.h>  /* Declarations of system-specific addresses etc */
#include "mipslab.h"  /* Declatations for these labs */
const int decimalPosToBinary[] = {1,2,4,8,16,32,64,128};

char textbuffer[4][16];

int gameState = 2;
int score = 0;
int gameScore0  = 0;
int gameScore1  = 0;
int gameScore2  = 0;
int gameScore3  = 0;


const uint8_t const font[] = {
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 94, 0, 0, 0, 0,
	0, 0, 4, 3, 4, 3, 0, 0,
	0, 36, 126, 36, 36, 126, 36, 0,
	0, 36, 74, 255, 82, 36, 0, 0,
	0, 70, 38, 16, 8, 100, 98, 0,
	0, 52, 74, 74, 52, 32, 80, 0,
	0, 0, 0, 4, 3, 0, 0, 0,
	0, 0, 0, 126, 129, 0, 0, 0,
	0, 0, 0, 129, 126, 0, 0, 0,
	0, 42, 28, 62, 28, 42, 0, 0,
	0, 8, 8, 62, 8, 8, 0, 0,
	0, 0, 0, 128, 96, 0, 0, 0,
	0, 8, 8, 8, 8, 8, 0, 0,
	0, 0, 0, 0, 96, 0, 0, 0,
	0, 64, 32, 16, 8, 4, 2, 0,
	0, 62, 65, 73, 65, 62, 0, 0,
	0, 0, 66, 127, 64, 0, 0, 0,
	0, 0, 98, 81, 73, 70, 0, 0,
	0, 0, 34, 73, 73, 54, 0, 0,
	0, 0, 14, 8, 127, 8, 0, 0,
	0, 0, 35, 69, 69, 57, 0, 0,
	0, 0, 62, 73, 73, 50, 0, 0,
	0, 0, 1, 97, 25, 7, 0, 0,
	0, 0, 54, 73, 73, 54, 0, 0,
	0, 0, 6, 9, 9, 126, 0, 0,
	0, 0, 0, 102, 0, 0, 0, 0,
	0, 0, 128, 102, 0, 0, 0, 0,
	0, 0, 8, 20, 34, 65, 0, 0,
	0, 0, 20, 20, 20, 20, 0, 0,
	0, 0, 65, 34, 20, 8, 0, 0,
	0, 2, 1, 81, 9, 6, 0, 0,
	0, 28, 34, 89, 89, 82, 12, 0,
	0, 0, 126, 9, 9, 126, 0, 0,
	0, 0, 127, 73, 73, 54, 0, 0,
	0, 0, 62, 65, 65, 34, 0, 0,
	0, 0, 127, 65, 65, 62, 0, 0,
	0, 0, 127, 73, 73, 65, 0, 0,
	0, 0, 127, 9, 9, 1, 0, 0,
	0, 0, 62, 65, 81, 50, 0, 0,
	0, 0, 127, 8, 8, 127, 0, 0,
	0, 0, 65, 127, 65, 0, 0, 0,
	0, 0, 32, 64, 64, 63, 0, 0,
	0, 0, 127, 8, 20, 99, 0, 0,
	0, 0, 127, 64, 64, 64, 0, 0,
	0, 127, 2, 4, 2, 127, 0, 0,
	0, 127, 6, 8, 48, 127, 0, 0,
	0, 0, 62, 65, 65, 62, 0, 0,
	0, 0, 127, 9, 9, 6, 0, 0,
	0, 0, 62, 65, 97, 126, 64, 0,
	0, 0, 127, 9, 9, 118, 0, 0,
	0, 0, 38, 73, 73, 50, 0, 0,
	0, 1, 1, 127, 1, 1, 0, 0,
	0, 0, 63, 64, 64, 63, 0, 0,
	0, 31, 32, 64, 32, 31, 0, 0,
	0, 63, 64, 48, 64, 63, 0, 0,
	0, 0, 119, 8, 8, 119, 0, 0,
	0, 3, 4, 120, 4, 3, 0, 0,
	0, 0, 113, 73, 73, 71, 0, 0,
	0, 0, 127, 65, 65, 0, 0, 0,
	0, 2, 4, 8, 16, 32, 64, 0,
	0, 0, 0, 65, 65, 127, 0, 0,
	0, 4, 2, 1, 2, 4, 0, 0,
	0, 64, 64, 64, 64, 64, 64, 0,
	0, 0, 1, 2, 4, 0, 0, 0,
	0, 0, 48, 72, 40, 120, 0, 0,
	0, 0, 127, 72, 72, 48, 0, 0,
	0, 0, 48, 72, 72, 0, 0, 0,
	0, 0, 48, 72, 72, 127, 0, 0,
	0, 0, 48, 88, 88, 16, 0, 0,
	0, 0, 126, 9, 1, 2, 0, 0,
	0, 0, 80, 152, 152, 112, 0, 0,
	0, 0, 127, 8, 8, 112, 0, 0,
	0, 0, 0, 122, 0, 0, 0, 0,
	0, 0, 64, 128, 128, 122, 0, 0,
	0, 0, 127, 16, 40, 72, 0, 0,
	0, 0, 0, 127, 0, 0, 0, 0,
	0, 120, 8, 16, 8, 112, 0, 0,
	0, 0, 120, 8, 8, 112, 0, 0,
	0, 0, 48, 72, 72, 48, 0, 0,
	0, 0, 248, 40, 40, 16, 0, 0,
	0, 0, 16, 40, 40, 248, 0, 0,
	0, 0, 112, 8, 8, 16, 0, 0,
	0, 0, 72, 84, 84, 36, 0, 0,
	0, 0, 8, 60, 72, 32, 0, 0,
	0, 0, 56, 64, 32, 120, 0, 0,
	0, 0, 56, 64, 56, 0, 0, 0,
	0, 56, 64, 32, 64, 56, 0, 0,
	0, 0, 72, 48, 48, 72, 0, 0,
	0, 0, 24, 160, 160, 120, 0, 0,
	0, 0, 100, 84, 84, 76, 0, 0,
	0, 0, 8, 28, 34, 65, 0, 0,
	0, 0, 0, 126, 0, 0, 0, 0,
	0, 0, 65, 34, 28, 8, 0, 0,
	0, 0, 4, 2, 4, 2, 0, 0,
	0, 120, 68, 66, 68, 120, 0, 0,
};

    

//box
uint8_t icon_row[] = {12,12,12,12,12,12,13,14,15,16,17,17,17,17,17,17,16,15,14,13
 };
uint8_t icon_col[] = {20,21,22,23,24,25,20,20,20,20,20,21,22,23,24,25,25,25,25,25

 };
uint8_t boarder_row[] = {11,11,11,11,11,11,11,11,12,13,14,15,16,17,18,18,18,18,18,18,18,18,17,16,15,14,13,12

 };
uint8_t boarder_col[] = {19,20,21,22,23,24,25,26,19,19,19,19,19,19,19,20,21,22,23,24,25,26,26,26,26,26,26,26
 };

//PIPE 1
uint8_t pipe1_row[] = {
	31,30,29,28,27,26,25,24,23,22,21,20,20,20,20,20,20,21,22,23,24,25,26,27,28,29,30,31, //28
};
uint8_t pipe1_col[] = {
	122,122,122,122,122,122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127,127,127,127,127,127, //28
};
uint8_t pipe1border_row[] = {
	30,28,26,24,22,20,20,20,21,23,25,27,29,31,20,//15
};
uint8_t pipe1border_col[] = {
	122,122,122,122,122,124,126,127,127,127,127,127,127,127,
};

//PIPE 2
uint8_t pipe2_row[] = {
	31,30,29,28,27,26,25,24,23,23,23,23,23,23,24,25,26,27,28,29,30,31, //26
};
uint8_t pipe2_col[] = {
	122,122,122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127,127,127, //26
};
uint8_t pipe2border_row[] = {
	31,29,27,25,23,23,23,22,22,24,26,28,30,23,//14
};
uint8_t pipe2border_col[] = {
	122,122,122,122,122,124,126,127,127,127,127,127,127,127,
};

//PIPE 3
uint8_t pipe3_row[] = {
	0,1,2,3,4,5,6,6,6,6,6,6,5,4,3,2,1,0, //18
};
uint8_t pipe3_col[] = {
	122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127, //18
};
uint8_t pipe3border_row[] = {
	0,2,4,6,6,6,5,3,1,6,//10
};
uint8_t pipe3border_col[] = {
	122,122,122,122,124,126,127,127,127,127,
};

//PIPE 4
uint8_t pipe4_row[] = {
	0,1,2,3,4,5,6,7,8,9,10,10,10,10,10,10,9,8,7,6,5,4,3,2,1,0,//26
};
uint8_t pipe4_col[] = {
	122,122,122,122,122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127,127,127,127,127, //26
};
uint8_t pipe4border_row[] = {
	0,2,4,6,8,10,10,10,9,7,5,3,1,10,//14
};
uint8_t pipe4border_col[] = {
	122,122,122,122,122,122,124,126,127,127,127,127,127,127,
};

uint8_t pipe5_row[] = {
	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,16,16,16,16,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0,//38
};

uint8_t pipe5_col[] = {
	122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
};

uint8_t pipe6_row[] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,21,21,21,21,21,20,19,18,17,16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1,0 //48
};

uint8_t pipe6_col[] = {
    122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127, //48
};

uint8_t pipe7_row[] = {
	31,30,29,28,27,27,27,27,27,27,28,29,30,31, //14
};

uint8_t pipe7_col[] = {
	122,122,122,122,122,123,124,125,126,127,127,127,127,127,
};

uint8_t pipe8_row[] = {
	31,30,29,28,27,26,25,24,23,22,21,20,19,18,17,16,15,14,13,12,11,11,11,11,11,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
};

uint8_t pipe8_col[] = {
	122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,124,125,126,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,
};

uint8_t pipe9_row[] ={
    31,30,29,29,29,29,29,29,30,31, //10
};
uint8_t pipe9_col[] = {
    122,122,12,123,124,125,126,127,127,127, //10
};



