#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include "initGPIO.h"
#include <wiringPi.h>
#include "framebuffer.h"
#include <stdbool.h>

#include "graphics/froggertitle.h"
#include "graphics/blackscreen.h"
#include "graphics/frog 60x60.h"
#include "graphics/level1_2.h"
#include "graphics/level3_4.h"
#include "graphics/car1 60x100.h"
#include "graphics/car2 60x100.h"
#include "graphics/car3 60x100.h"
#include "graphics/car4 60x180.h"
#include "graphics/cow 60x100.h"
#include "graphics/lilypad 60x70.h"
#include "graphics/log 60x240.h"
#include "graphics/pausemenu500x500.h"
#include "graphics/pig 60x60.h"
#include "graphics/rock1 60x120.h"
#include "graphics/rock2 60x180.h"
#include "graphics/turtles 60x300.h"
#include "graphics/valuepack60x60.h"
#include "graphics/stats 300x60.h"

/* SNES reader definitions*/

static unsigned int *gpio;

// set function of a pin p to input(INP_GPIO) or output(OUT_GPIO)
#define INP_GPIO(g) *(gpio + ((g) / 10)) &= ~(7 << (((g) % 10) * 3))
#define OUT_GPIO(g) *(gpio + ((g) / 10)) |= (1 << (((g) % 10) * 3))

//defining some constants

#define CLK 11
#define LAT 9
#define DAT 10
#define GPLEV0 13
#define GPSET0 7
#define GPCLR0 10

/* Definitions */
typedef struct
{
	short int color;
	int x, y;
} Pixel;

struct fbs framebufferstruct;
void drawPixel(Pixel *pixel);

int xfrog, yfrog; //global variable for the frog's coordinates

int moves, lives, timeLeft; // variable for the number of moves and lives
bool level1 = true;

// global pointers for each image to draw
short int *titlePtr = (short int *)title.pixel_data;
short int *level1_2Ptr = (short int *)level1_2.pixel_data;
short int *level3_4Ptr = (short int *)level3_4.pixel_data;
short int *blackscreenPtr = (short int *)blackscreen.pixel_data;
short int *frogPtr = (short int *)frog.pixel_data;
short int *car1Ptr = (short int *)car1.pixel_data;
short int *car2Ptr = (short int *)car2.pixel_data;
short int *car3Ptr = (short int *)car3.pixel_data;
short int *car4Ptr = (short int *)car4.pixel_data;
short int *cowPtr = (short int *)cow.pixel_data;
short int *lilypadPtr = (short int *)lilypad.pixel_data;
short int *logPtr = (short int *)log.pixel_data;
short int *pausemenuPtr = (short int *)pausemenu.pixel_data;
short int *pigPtr = (short int *)pig.pixel_data;
short int *rock1Ptr = (short int *)rock1.pixel_data;
short int *rock2Ptr = (short int *)rock2.pixel_data;
short int *turtlesPtr = (short int *)turtles.pixel_data;
short int *valuepackPtr = (short int *)valuepack.pixel_data;
short int *statsPtr = (short int *)stats.pixel_data;

short int stage[1280 + 1][720 + 1];
/* Draw an image */
void drawCanvas()
{
	framebufferstruct = initFbInfo();
	long int i = 0;
	for (int x = 0; x < 1280; x++)
	{
		for (int y = 0; y < 720; y++)
		{
			long int location = (x + framebufferstruct.xOff) * (framebufferstruct.bits / 8) +
								(y + framebufferstruct.yOff) * framebufferstruct.lineLength;
			*((unsigned short int *)(framebufferstruct.fptr + location)) = stage[x][y];
		}
	}
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
}
void drawImage(short int *imagePtr, int width, int length, int xstart, int ystart)
{
	//framebufferstruct = initFbInfo();
	// initialize + get FBS
	//short int *ptr = imagePtr; 				//(short int *) title.pixel_data;

	Pixel *pixel; // initialize a pixel
	pixel = malloc(sizeof(Pixel));
	int i = 0;
	//unsigned int quarter,byte,word;

	for (int y = ystart; y < length + ystart; y++)
	{
		for (int x = xstart; x < width + xstart; x++)
		{
			pixel->color = imagePtr[i];
			pixel->x = x;
			pixel->y = y;
			//drawPixel(pixel);
			stage[x][y] = pixel->color;
			i++;
		}
	}

	/* free pixel's allocated memory */
	free(pixel);
	pixel = NULL;
	// long int location =(framebufferstruct.xOff) * (framebufferstruct.bits/8) +
	//                    (framebufferstruct.yOff) * framebufferstruct.lineLength;
	// printf("%d\n",  *((unsigned short int*)(framebufferstruct.fptr + location)));
	// printf("%d",  *((unsigned short int*)(framebufferstruct.fptr + location+4)));
	// munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
}

struct Car
{
	int x;
	int y;
} car[6];
struct Lilypad
{
	int x;
	int y;
} lilypads[7];
struct Log
{
	int x;
	int y;
} logs[4];
struct Turtle
{
	int x;
	int y;
} turtle[2];
struct Rock{
    int x;
    int y;
} rock[10];
struct Cow{
    int x;
    int y;
} cows[4];
struct pig{
    int x;
    int y;
} pigs[4];

void initOb(){
car[1].x = 1180;
car[1].y = 600;
 car[2].x = 0;
    //int secondCar2x = 500;
car[2].y = 540;
car[3].x = 1180;
 car[3].y = 480;
 car[4].x = 1100;
 car[4].y = 420;
car[5].x = 1180;
 car[5].y = 360;

 lilypads[1].x = 0;
lilypads[2].x = 200;
 lilypads[3].x = 400;
 lilypads[1].y = 240;
 logs[1].x = 0;
     logs[2].x = 350;
     logs[3].x = 650;
 logs[1].y = 180;
     lilypads[4].x = 1200;
 lilypads[5].x = 600;
 lilypads[6].x = 200;
lilypads[4].y = 120;
turtle[1].x = 980;
turtle[1].y = 60;

rock[1].x = 0;
rock[2].x = 200;
rock[1].y = rock[2].y = 600;
rock[3].x = 1100;
rock[4].x = 700;
rock[3].y = rock[4].y = 540;
rock[5].x = 0;
rock[5].y = 480;
rock[6].x = 1160;
rock[7].x = 600;
rock[6].y = rock[7].y = 420;
rock[8].x = 0;
rock[9].x = 200;
rock[8].y = rock[9].y = 360;

cows[1].x = 0;
cows[2].x = 200;
cows[1].y = cows[2].y = 240;
cows[3].x = 0;
cows[3].y = 120;
pigs[1].x = 1220;
pigs[1].y = 180;
pigs[2].x = 1220;
pigs[3].x = 720;
pigs[2].y = pigs[3].y = 60;

}

void drawLevel1Ob()
{
	drawImage(car1Ptr, 100, 60, car[1].x, car[1].y);
	car[1].x = car[1].x - 3;
	if (car[1].x < 0)
	{
		car[1].x = 1180;
	}
	drawImage(car2Ptr, 100, 60, car[2].x, car[2].y);
	car[2].x = car[2].x + 2;
	if (car[2].x + 100 > 1280)
	{
		car[2].x = 0;
	}
	// drawImage(car2Ptr, 100, 60, secondCar2x, car[2].y); // drawing a second car in lane 2
	// secondCar2x += 15;
	// if (secondCar2x+100>1280) {
	// 	secondCar2x = 0;
	// }
	drawImage(car3Ptr, 100, 60, car[3].x, car[3].y);
	//drawImage(car3Ptr, 100, 60, car[3].x-200, car[3].y);
	car[3].x = car[3].x - 5;
	if (car[3].x < 0)
	{
		car[3].x = 1180;
	}
	drawImage(car4Ptr, 180, 60, car[4].x, car[4].y);
	car[4].x = car[4].x - 2;
	if (car[4].x < 0)
	{
		car[4].x = 1100;
	}
	drawImage(car1Ptr, 100, 60, car[5].x, car[5].y);
	car[5].x = car[5].x - 4;
	if (car[5].x < 0)
	{
		car[5].x = 1180;
	}
	drawImage(lilypadPtr, 80, 60, lilypads[1].x, lilypads[1].y); // drawing 3 lilypadss in lane 1
	lilypads[1].x += 10;
	if (lilypads[1].x + 80 > 1280)
	{
		lilypads[1].x = 0;
	}
	drawImage(lilypadPtr, 80, 60, lilypads[2].x, lilypads[1].y);
	lilypads[2].x += 10;
	if (lilypads[2].x + 80 > 1280)
	{
		lilypads[2].x = 0;
	}
	drawImage(lilypadPtr, 80, 60, lilypads[3].x, lilypads[1].y);
	lilypads[3].x += 10;
	if (lilypads[3].x + 80 > 1280)
	{
		lilypads[3].x = 0;
	}
	drawImage(lilypadPtr, 80, 60, lilypads[4].x, lilypads[4].y); // drawing 3 lilypads in lane 3
	lilypads[4].x -= 6;
	if (lilypads[4].x < 0)
	{
		lilypads[4].x = 1200;
	}
	drawImage(lilypadPtr, 80, 60, lilypads[5].x, lilypads[4].y);
	lilypads[5].x -= 6;
	if (lilypads[5].x < 0)
	{
		lilypads[5].x = 1200;
	}
	drawImage(lilypadPtr, 80, 60, lilypads[6].x, lilypads[4].y);
	lilypads[6].x -= 6;
	if (lilypads[6].x < 0)
	{
		lilypads[6].x = 1200;
	}
	drawImage(logPtr, 240, 60, logs[1].x, logs[1].y); // drawing 3 logs as well
	logs[1].x += 8;
	if (logs[1].x + 240 > 1280)
	{
		logs[1].x = 0;
	}
	drawImage(logPtr, 240, 60, logs[2].x, logs[1].y); // drawing 3 logs as well
	logs[2].x += 8;
	if (logs[2].x + 240 > 1280)
	{
		logs[2].x = 0;
	}
	drawImage(logPtr, 240, 60, logs[3].x, logs[1].y); // drawing 3 logs as well
	logs[3].x += 8;
	if (logs[3].x + 240 > 1280)
	{
		logs[3].x = 0;
	}
	drawImage(turtlesPtr, 300, 60, turtle[1].x, turtle[1].y);
	turtle[1].x -= 12;
	if (turtle[1].x < 0)
	{
		turtle[1].x = 980;
	}
	drawImage(frogPtr, 60, 60, xfrog, yfrog);
}
void drawLevel2Ob() {
    drawImage(rock1Ptr, 120, 60, rock[1].x, rock[1].y);
    rock[1].x += 5;
    if (rock[1].x+120>1280) {
        rock[1].x = 0;
    }
    drawImage(rock1Ptr, 120, 60, rock[2].x, rock[2].y);
    rock[2].x += 5;
    if (rock[2].x+120>1280) {
        rock[2].x = 0;
    }
    drawImage(rock2Ptr, 180, 60, rock[3].x, rock[3].y);
    rock[3].x -= 5;
    if (rock[3].x<0) {
        rock[3].x = 1100;
    }
    drawImage(rock2Ptr, 180, 60, rock[4].x, rock[4].y);
    rock[4].x -= 5;
    if (rock[4].x<0) {
        rock[4].x = 1100;
    }
    drawImage(rock1Ptr, 120, 60, rock[5].x, rock[5].y);
    rock[5].x += 5;
    if (rock[5].x+120>1280) {
        rock[5].x = 0;
    }
    drawImage(rock1Ptr, 120, 60, rock[6].x, rock[6].y);
    rock[6].x -= 5;
    if (rock[6].x<0) {
        rock[6].x = 1160;
    }
    drawImage(rock1Ptr, 120, 60, rock[7].x, rock[7].y);
    rock[7].x -= 5;
    if (rock[7].x<0) {
        rock[7].x = 1160;
    }
    drawImage(rock2Ptr, 180, 60, rock[8].x, rock[8].y);
    rock[8].x += 5;
    if (rock[8].x+180>1280) {
        rock[8].x = 0;
    }
    drawImage(rock2Ptr, 180, 60, rock[9].x, rock[9].y);
    rock[9].x += 5;
    if (rock[9].x+180>1280) {
        rock[9].x = 0;
    }
	
drawImage(cowPtr, 100, 60, cows[1].x, cows[1].y);
cows[1].x += 5;
if(cows[1].x + 100 > 1280) {
	cows[1].x = 0;
}
drawImage(cowPtr, 100, 60, cows[2].x, cows[2].y);
cows[2].x += 5;
if(cows[2].x + 100 > 1280) {
	cows[2].x = 0;
}
drawImage(cowPtr, 100, 60, cows[3].x, cows[3].y);
cows[3].x += 5;
if(cows[3].x + 100 > 1280) {
	cows[3].x = 0;
}
drawImage(pigPtr, 60, 60, pigs[1].x, pigs[1].y);
pigs[1].x -= 5;
if(pigs[1].x < 0) {
	pigs[1].x = 1220;
}
drawImage(pigPtr, 60, 60, pigs[2].x, pigs[2].y);
pigs[2].x -= 5;
if(pigs[1].x < 0) {
	pigs[1].x = 1220;
}
drawImage(pigPtr, 60, 60, pigs[3].x, pigs[3].y);
pigs[3].x -= 5;
if(pigs[1].x < 0) {
	pigs[1].x = 1220;
}

    drawImage(frogPtr, 60, 60, xfrog, yfrog);
}



void drawLevel(int l)
{
	if (l == 1)
	{
		drawImage(level1_2Ptr, 1280, 720, 0, 0);
	}
	else
	{
		drawImage(level3_4Ptr, 1280, 720, 0, 0);
	}
}
bool carCollision()
{
	return ((( xfrog<car[1].x+100 && xfrog+60>car[1].x ) && ( yfrog<car[1].y+60 && yfrog+60>car[1].y ) )) || // if top right pixel or top left pixel of car hits frog north border && y coord of car and frog are same
		    ((( xfrog<=car[2].x+100 && xfrog+60>=car[2].x ) && ( yfrog<=car[2].y+60 && yfrog+60>car[2].y ) )) || // same thing with car2
		   //secondCar2x + 100 == xfrog+i && car[2].y == yfrog || secondCar2x == xfrog+i && car[2].y == yfrog || // and so on...
		    ((( xfrog<car[3].x+100 && xfrog+60>car[3].x ) && ( yfrog<car[3].y+60 && yfrog+60>car[3].y ) )) ||
		    ((( xfrog<car[4].x+180 && xfrog+60>car[4].x ) && ( yfrog<car[4].y+60 && yfrog+60>car[4].y ) )) ||
		    ((( xfrog<car[5].x+100 && xfrog+60>car[5].x ) && ( yfrog<car[5].y+60 && yfrog+60>car[5].y ) ));
}
bool logCollision()
{
	return ((( xfrog<logs[1].x+240 && xfrog+60>logs[1].x ) && ( yfrog<logs[1].y+60 && yfrog+60>logs[1].y ) )) || // if top right pixel or top left pixel of car hits frog north border && y coord of car and frog are same
		    ((( xfrog<logs[2].x+240 && xfrog+60>logs[2].x ) && ( yfrog<logs[2].y+60 && yfrog+60>logs[2].y ) )) || // same thing with car2
		   //secondCar2x + 100 == xfrog+i && car[2].y == yfrog || secondCar2x == xfrog+i && car[2].y == yfrog || // and so on...
		   ((( xfrog<logs[3].x+240 && xfrog+60>logs[3].x ) && ( yfrog<logs[3].y+60 && yfrog+60>logs[3].y ) )) ;
}
bool lilyCollision()
{
	return ((( xfrog<lilypads[1].x+80 && xfrog+60>lilypads[1].x ) && ( yfrog<lilypads[1].y+60 && yfrog+60>lilypads[1].y ) ))|| // if frog north border hits top middle pixel of lilypad
		   ((( xfrog<lilypads[1].x+80 && xfrog+60>lilypads[1].x ) && ( yfrog<lilypads[1].y+60 && yfrog+60>lilypads[1].y ) )) ||
		   ((( xfrog<lilypads[1].x+80 && xfrog+60>lilypads[1].x ) && ( yfrog<lilypads[1].y+60 && yfrog+60>lilypads[1].y ) ));
}
bool turtleCollision()
{
	return ((( xfrog<turtle[1].x+300&& xfrog+60>turtle[1].x ) && ( yfrog<turtle[1].y+60 && yfrog+60>turtle[1].y ) ));
}
// /* Draw a pixel */
// void drawPixel(Pixel *pixel){
// 	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
//                        (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
// 	 *((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
// 	 //printf("%d",pixel -> color);

// 	// long int location =(pixel->x+framebufferstruct.xOff)* (framebufferstruct.bits/8) +
//     //                    (pixel->y+framebufferstruct.yOff)  * framebufferstruct.lineLength;
// 	//  *((unsigned short int*)stage + location) = pixel->color;

// }

// functions taken from project part 1 below

// Init_GPIO initializes a GPIO line using the INP_GPIO and OUT_GPIO functions.
// @param: line - the line number to initialize, code - function code describing whtehr the line is input/output
void Init_GPIO(int line, int code)
{
	if (code == 0)
	{ //if code =1 then it's read only
		INP_GPIO(line);
	}
	else
	{
		INP_GPIO(line);
		OUT_GPIO(line);
	}
}

// Write_Latch writes a bit to the latch line.
// @param: b - the bit to be written.
void Write_Latch(int b)
{
	if (b == 1)
	{
		gpio[GPSET0] = 1 << LAT; // write 1 to pin 9 (LAT)
	}
	else
	{
		gpio[GPCLR0] = 1 << LAT; // write 0 to LAT
	}
}

// Write_Clock writes a bit to the clock line.
// @param: b - the bit to be written.
void Write_Clock(int b)
{
	if (b == 1)
	{
		gpio[GPSET0] = 1 << CLK;
	}
	else
	{
		gpio[GPCLR0] = 1 << CLK;
	}
}

// Read_Data reads and returns a bit from the data line using GPLEV0.
int Read_Data()
{
	int v = (gpio[GPLEV0] >> DAT) & 1;
	return v;
}

// Wait causes the program to wait for a specified time interval.
// @params: s - time interval in microseconds for which to wait.
void Wait(int s)
{
	delayMicroseconds(s);
}

// Menu_Read_SNES used to read SNES inputs while on the main menu
int Menu_Read_SNES()
{
	gpio = getGPIOPtr(); // get gpio pointer

	int buttons[17]; // array for the buttons
	// initializing 3 GPIO lines
	Init_GPIO(CLK, 1);
	Init_GPIO(LAT, 1);
	Init_GPIO(DAT, 0);

	int pressed = 0;
	int cursor;		// variable indicating the location of the cursor (0 means on START GAME, 1 means on QUIT GAME)
	buttons[9] = 1; // initialize A as unpressed

	while (buttons[9] == 1)
	{ // while A is not pressed
		pressed = 0;

		while (pressed == 0)
		{
			// reference: pseudocode from lecture notes "RPi 2 SNES" slide 20
			Write_Clock(1);
			Write_Latch(1);
			Wait(12);
			Write_Latch(0);

			for (int i = 1; i <= 16; i++)
			{ // loop through each button
				Wait(6);
				Write_Clock(0);
				Wait(6);
				int b = Read_Data();
				buttons[i] = b;

				if (b == 0)
				{ // if the button is pressed

					switch (i)
					{

					// UP
					case 5:
						drawImage(titlePtr, 1280, 720, 0, 0);
						drawImage(frogPtr, 60, 60, 300, 450);
						drawCanvas();
						cursor = 0;
						break;

					// DOWN
					case 6:
						drawImage(titlePtr, 1280, 720, 0, 0);
						drawImage(frogPtr, 60, 60, 300, 530);
						drawCanvas();
						cursor = 1;
						break;

					// A
					case 9:
						printf("You have pressed A\n");
						break;
					}
					pressed = 1;
					break;
				}
				Write_Clock(1);
			}
		}
		Wait(200000);
	}
	return cursor;
}

// function to update frog position on display when joypad buttons are pressed.
void InterpretButtons(int i)
{

	switch (i)
	{
	case 4:
		printf("Program is terminating...\n"); // pressing START opens pause menu
		break;
	case 5:
		printf("You have pressed Joy-pad UP\n"); // go 60 px up
		if (yfrog - 60 >= 0)
		{ // edge protection, make sure frog doesnt go outside screen
			if (level1)
			{
				yfrog -= 60;
				drawLevel(1);
				drawLevel1Ob();
				drawCanvas();
				moves--;
			}
			else
			{
				yfrog -= 60;
				drawLevel(2);
				drawImage(frogPtr, 60, 60, xfrog, yfrog);
				drawLevel2Ob();
				drawCanvas();
				moves--;
			}
		}
		else if (level1)
		{
			// go to the next 2 levels, new background screen
			level1 = false;
			drawLevel(2);
			yfrog = 720 - 60;
			drawImage(frogPtr, 60, 60, xfrog, yfrog);
			drawCanvas();
			moves--;
		}
		break;
	case 6:
		printf("You have pressed Joy-pad DOWN\n"); // 60 px down
		if (yfrog + 60 < 720 && level1)
		{
			yfrog += 60;
			drawLevel(1);
			drawLevel1Ob();
			drawCanvas();
			moves--;
		}
		else if (!level1)
		{
			if (yfrog + 60 < 720)
			{
				yfrog += 60;
				drawLevel(2);
				drawImage(frogPtr, 60, 60, xfrog, yfrog);
				drawLevel2Ob();
				drawCanvas();
				moves--;
			}
			else
			{
				level1 = true;
				yfrog = 0;
				yfrog += 60;
				drawLevel(1);
				drawLevel1Ob();
				drawCanvas();
				moves--;
			}
		}
		break;
	case 7:
		printf("You have pressed Joy-pad LEFT\n"); // 60 px left
		if (xfrog - 60 >= 0)
		{
			if (level1)
			{
				xfrog -= 60;
				drawLevel(1);
				drawLevel1Ob();
				drawCanvas();
				moves--;
			}
			else
			{
				xfrog -= 60;
				drawLevel(2);
				//drawImage(frogPtr, 60, 60, xfrog, yfrog);
				drawLevel2Ob();
				drawCanvas();
				moves--;
			}
		}
		break;
	case 8:
		printf("You have pressed Joy-pad RIGHT\n");
		if (xfrog + 60 < 1220)
		{
			if (level1)
			{
				xfrog += 60;
				drawLevel(1);
				drawLevel1Ob();
				drawCanvas();
				moves--;
			}
			else
			{
				xfrog += 60;
				drawLevel(2);
				drawImage(frogPtr, 60, 60, xfrog, yfrog);
				drawLevel2Ob();
				drawCanvas();
				moves--;
			}
		}
		break;
	case 9:
		printf("You have pressed A\n");
		break;
	}
}

// Game_Read_SNES used to read SNES controller input while in the main game state
int Game_Read_SNES()
{
	gpio = getGPIOPtr(); // get gpio pointer

	int buttons[17]; // array for the buttons
	// initializing 3 GPIO lines
	Init_GPIO(CLK, 1);
	Init_GPIO(LAT, 1);
	Init_GPIO(DAT, 0);

	int pressed = 0;

	// initial coordinate locations for all the obstacles in leve1_2
	initOb();

	buttons[4] = 1; // initialize START as unpressed
	while (buttons[4] == 1)
	{ // while START is not pressed
		pressed = 0;
		while (pressed == 0)
		{
			// reference: pseudocode from lecture notes "RPi 2 SNES" slide 20
			Write_Clock(1);
			Write_Latch(1);
			Wait(12);
			Write_Latch(0);
			if (level1)
			{
				drawLevel(1);
				drawLevel1Ob();
				drawImage(statsPtr, 300, 60, 0, 0); // some flickering if i put it here
				drawCanvas();
			}
			else
			{
				drawLevel(2);
				drawLevel2Ob();
				drawImage(statsPtr, 300, 60, 0, 0);
				drawCanvas();
			}
			// doing collision detection

			// loop through 60 top 60 pixels of the frog (north border)
			
				// checking collissions for all cars in level 1
				if (level1)
				{
					printf("%d %d %d %d",xfrog,car[1].x,yfrog,car[1].y);
					if (carCollision())
					{
						lives--;
						xfrog = 610;
						yfrog = 660;
						drawLevel(1);
						drawLevel1Ob();
						drawCanvas();
						break;
					}

					// collision with lilyads in lane 1
					if (lilyCollision())
					{
						// move frog along with the lilypad
						//drawCanvas();
						xfrog += 10;
						//drawImage(frogPtr, 60, 60, xfrog, yfrog);
						drawLevel(1);
						drawLevel1Ob();
						drawCanvas();
						break;

						// collision with logs lane 2
						// check if frog north border collides with any 1 of 4 pixels evenly spread out on the log
					}
					else if (logCollision())
					{
						// move frog along with the lilypad
						//drawCanvas();
						xfrog += 8;
						//drawImage(frogPtr, 60, 60, xfrog, yfrog);
						drawLevel(1);
						drawLevel1Ob();
						drawCanvas();
						break;

						// collisions with lilypads in lane 3
					}
					else if (lilyCollision())
					{
						// move frog along with the lilypad
						// drawCanvas();
						xfrog -= 6;
						drawLevel(1);
						drawLevel1Ob();
						drawCanvas();
						break;

						// collisions with turtles (any 1 of 5 pixels)
					}
					else if (turtleCollision())
					{
						// move frog along with the lilypad
						//drawCanvas();
						xfrog -= 12;
						drawLevel(1);
						drawLevel1Ob();
						drawCanvas();
						break;
					}

					// something wrong here
					// else if (yfrog<=240 && !logCollision() && !lilyCollision() && !turtleCollision()) {
					// // frog drowns in water :( reset to starting place
					// lives--;		
					// xfrog = 610;		
					// yfrog = 660;
					// drawLevel(1);
					// 	drawLevel1Ob();
					// 	drawCanvas();
					// } 
					
				
			}

			for (int i = 1; i <= 16; i++)
			{ // loop through each button
				Wait(6);
				Write_Clock(0);
				Wait(6);
				int b = Read_Data();
				buttons[i] = b;

				if (b == 0)
				{						 // if the button is pressed
					InterpretButtons(i); // call interpretButtons to update the screen based on what button is pressed
					pressed = 1;
					break;
				}
				Write_Clock(1);
			}
		}
		Wait(200000);
	}
}

// randBetween function original source code from http://www.fundza.com/c4serious/randbetween/index.html
int randBetween(double min, double max)
{
	return ((int)rand() / RAND_MAX) * (max - min) + min;
}

/* main function */
int main()
{

	moves = 40;
	lives = 4;		// initializing moves to 40 and lives to 4
	timeLeft = 120; // given 120 seconds
	clock_t start = clock();

	// put this somewhere in the thread, right before you display the time:
	/*
	timeLeft -= (int) start/CLOCKS_PER_SEC
	if (timeLeft = 30) {
		// print a value pack somewhere on the screen. the value pack will add one life.
		xvaluepack = randBetween(0, 1220);
		yvaluepack = randBetween(0, 660);
		drawImage(valuepackPtr, 60, 60, xvaluepack, yvaluepack);
		//...
		if(frog location == value pack location) {
			lives++;
		}
	}
	*/

	drawImage(titlePtr, 1280, 720, 0, 0); // drawing the title
	drawImage(frogPtr, 60, 60, 300, 450); // drawing frog icon to be used as a cursor
	drawCanvas();

	int cursor = Menu_Read_SNES(); // reading controller input

	if (cursor == 1)
	{												// cursor is on 'QUIT GAME'
		drawImage(blackscreenPtr, 1280, 720, 0, 0); // print a black screen
		drawCanvas();
		exit(0);
	}

	// else, starting the level

	drawImage(level1_2Ptr, 1280, 720, 0, 0); // drawing the level background
	drawImage(frogPtr, 60, 60, 610, 660);	 // drawing frog icon in center of the level
	drawCanvas();
	xfrog = 610; // init variables for starting place coordinates of frog
	yfrog = 660;

	Game_Read_SNES();

	return 0;
}
