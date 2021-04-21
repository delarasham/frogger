#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/mman.h>
#include "initGPIO.h"
#include <wiringPi.h>
#include "framebuffer.h"

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

/* SNES reader definitions*/

static unsigned int *gpio; 

// set function of a pin p to input(INP_GPIO) or output(OUT_GPIO)
#define INP_GPIO(g) *(gpio + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio + ((g)/10)) |= (1<<(((g)%10)*3))

//defining some constants

#define CLK 11
#define LAT 9
#define DAT 10
#define GPLEV0 13
#define GPSET0 7
#define GPCLR0 10

/* Definitions */
typedef struct {
	short int color;
	int x, y;
} Pixel;

struct fbs framebufferstruct;
void drawPixel(Pixel *pixel);

int xfrog, yfrog; //global variable for the frog's coordinates

int moves, lives, timeLeft; // variable for the number of moves and lives

// global pointers for each image to draw
short int *titlePtr = (short int *) title.pixel_data;
short int *level1_2Ptr = (short int *) level1_2.pixel_data;
short int *level3_4Ptr = (short int *) level3_4.pixel_data;
short int *blackscreenPtr = (short int *) blackscreen.pixel_data;
short int *frogPtr = (short int *) frog.pixel_data;
short int *car1Ptr = (short int *) car1.pixel_data;
short int *car2Ptr = (short int *) car2.pixel_data;
short int *car3Ptr = (short int *) car3.pixel_data;
short int *car4Ptr = (short int *) car4.pixel_data;
short int *cowPtr = (short int *) cow.pixel_data;
short int *lilypadPtr = (short int *) lilypad.pixel_data;
short int *logPtr = (short int *) log.pixel_data;
short int *pausemenuPtr = (short int *) pausemenu.pixel_data;
short int *pigPtr = (short int *) pig.pixel_data;
short int *rock1Ptr = (short int *) rock1.pixel_data;
short int *rock2Ptr = (short int *) rock2.pixel_data;
short int *turtlesPtr = (short int *) turtles.pixel_data;
short int *valuepackPtr = (short int *) valuepack.pixel_data;

short int stage[1280+1][720+1];
/* Draw an image */
void drawCanvas(){
	framebufferstruct = initFbInfo();
	long int i =0;
	for(int x=0;x<1280;x++){
		for(int y=0;y<720;y++){
			long int location =(x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	 *((unsigned short int*)(framebufferstruct.fptr + location)) = stage[x][y];
		}
	}
	munmap(framebufferstruct.fptr, framebufferstruct.screenSize);
}
void drawImage(short int * imagePtr, int width, int length, int xstart, int ystart) {
    //framebufferstruct = initFbInfo();
	 			// initialize + get FBS
	//short int *ptr = imagePtr; 				//(short int *) title.pixel_data;
	
	Pixel *pixel;								// initialize a pixel
	pixel = malloc(sizeof(Pixel));
	int i=0;
	//unsigned int quarter,byte,word;

	for (int y = ystart; y < length+ystart; y++) { 
		for (int x = xstart; x < width+xstart; x++) { 
			pixel->color = imagePtr[i]; 
			pixel->x = x;
			pixel->y = y;	
			//drawPixel(pixel);
			stage[x][y] = pixel ->color;
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

/* Draw a pixel */
void drawPixel(Pixel *pixel){
	long int location =(pixel->x +framebufferstruct.xOff) * (framebufferstruct.bits/8) +
                       (pixel->y+framebufferstruct.yOff) * framebufferstruct.lineLength;
	 *((unsigned short int*)(framebufferstruct.fptr + location)) = pixel->color;
	 //printf("%d",pixel -> color);

	// long int location =(pixel->x+framebufferstruct.xOff)* (framebufferstruct.bits/8) +
    //                    (pixel->y+framebufferstruct.yOff)  * framebufferstruct.lineLength;
	//  *((unsigned short int*)stage + location) = pixel->color;
	
}



// functions taken from project part 1 below

// Init_GPIO initializes a GPIO line using the INP_GPIO and OUT_GPIO functions.
// @param: line - the line number to initialize, code - function code describing whtehr the line is input/output 
void Init_GPIO(int line, int code){
	if(code == 0){	//if code =1 then it's read only
	INP_GPIO(line);
	}
	else{
	INP_GPIO(line);
	OUT_GPIO(line);
	}
}

// Write_Latch writes a bit to the latch line.
// @param: b - the bit to be written.
void Write_Latch(int b){
	if(b==1){
		gpio[GPSET0] = 1<<LAT; // write 1 to pin 9 (LAT)
	}
	else{
		gpio[GPCLR0] = 1<<LAT; // write 0 to LAT
	}
}

// Write_Clock writes a bit to the clock line.
// @param: b - the bit to be written.
void Write_Clock(int b){
	if(b==1){
        gpio[GPSET0] = 1<<CLK;
    }
    else{ 
        gpio[GPCLR0] = 1<<CLK;
    }
}

// Read_Data reads and returns a bit from the data line using GPLEV0.
int Read_Data(){
	int v = (gpio[GPLEV0]>>DAT) &1;
	return v;
}

// Wait causes the program to wait for a specified time interval.
// @params: s - time interval in microseconds for which to wait.
void Wait(int s) {
    delayMicroseconds(s);
}

// Menu_Read_SNES used to read SNES inputs while on the main menu
int Menu_Read_SNES() 
{
	gpio = getGPIOPtr(); // get gpio pointer

	int buttons[17]; // array for the buttons
	// initializing 3 GPIO lines
	Init_GPIO(CLK,1);
	Init_GPIO(LAT,1);
	Init_GPIO(DAT,0);

	int pressed = 0;
	int cursor;				// variable indicating the location of the cursor (0 means on START GAME, 1 means on QUIT GAME)
	buttons[9]=1;			// initialize A as unpressed
	
	while(buttons[9]==1) {	// while A is not pressed
		pressed = 0;

		while (pressed == 0) {
		// reference: pseudocode from lecture notes "RPi 2 SNES" slide 20
		Write_Clock(1);
		Write_Latch(1);
		Wait(12);
		Write_Latch(0);

		for (int i=1;i<=16;i++) {		// loop through each button
        	Wait(6);
        	Write_Clock(0);
    		Wait(6);
    		int b = Read_Data();
    		buttons[i] =b;

			if (b==0) {					// if the button is pressed

				switch (i) { 

					// UP
					case 5:
					//drawImage(titlePtr, 1280, 720, 0, 0);
					drawImage(frogPtr, 60, 60, 300, 450);
					drawCanvas();
					printf("You have pressed Joy-pad UP\n");
					cursor = 0;
               			break;

					// DOWN
					case 6:
					//drawImage(titlePtr, 1280, 720, 0, 0);
					drawImage(frogPtr, 60, 60, 300, 530);
					drawCanvas();
					printf("You have pressed Joy-pad DOWN\n");
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
void InterpretButtons(int i){

	switch (i){
		case 4:
		printf("Program is terminating...\n");	// pressing START opens pause menu
                break;
		case 5:
		printf("You have pressed Joy-pad UP\n"); // go 60 px up
		if (yfrog-60 >= 0) { // edge protection, make sure frog doesnt go outside screen
			drawImage(level1_2Ptr, 1280, 720, 0, 0);
			drawImage(frogPtr, 60, 60, xfrog, yfrog-60);
			yfrog -= 60;
			moves--;

		} else {
			// go to the next 2 levels, new background screen
			drawImage(level3_4Ptr, 1280, 720, 0, 0); 
			drawImage(frogPtr, 60, 60, 610, 660);
		}
                break;
		case 6:
		printf("You have pressed Joy-pad DOWN\n"); // 60 px down
		if (yfrog+60 < 720) {
			drawImage(level1_2Ptr, 1280, 720, 0, 0);
			drawImage(frogPtr, 60, 60, xfrog, yfrog+60);
			yfrog += 60;
			moves--;
		}
                break;
		case 7:
		printf("You have pressed Joy-pad LEFT\n"); // 60 px left
		if (xfrog-60 >= 0) {
			drawImage(level1_2Ptr, 1280, 720, 0, 0);
			drawImage(frogPtr, 60, 60, xfrog-60, yfrog);
			xfrog -= 60;
			moves--;
		}
                break;
		case 8:
		printf("You have pressed Joy-pad RIGHT\n");
		if (xfrog+60 < 1260) {
			drawImage(level1_2Ptr, 1280, 720, 0, 0);
			drawImage(frogPtr, 60, 60, xfrog+60, yfrog);
			xfrog += 60;
			moves--;
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
	Init_GPIO(CLK,1);
	Init_GPIO(LAT,1);
	Init_GPIO(DAT,0);

	int pressed = 0;

	// initial coordinate locations for all the obstacles in leve1_2
	int car1x = 1180;
	int car1y = 600;
	int car2x = 0;
	int secondCar2x = 500;
	int car2y = 540;
	int car3x = 1180;
	int car3y = 480;
	int car4x = 1100;
	int car4y = 420;
	int car5x = 1180;
	int car5y = 360;

	int lilypad1x = 0;
	int secondlilypad1x = 200;
	int thirdlilypad1x = 400;
	int lilypad1y = 240;
	int logx = 0;
	int secondlogx = 350;
	int thirdlogx = 650;
	int logy = 180;
	int lilypad2x = 1200;
	int secondlilypad2x = 600;
	int thirdlilypad2x = 200;
	int lilypad2y = 120;
	int turtlesx = 980;
	int turtlesy = 60;


	buttons[4]=1;			// initialize START as unpressed
	while(buttons[4]==1){	// while START is not pressed
		pressed = 0;
		while (pressed == 0) {
			// reference: pseudocode from lecture notes "RPi 2 SNES" slide 20
			Write_Clock(1);
			Write_Latch(1);
			Wait(12);
			Write_Latch(0);
			
			drawImage(level1_2Ptr, 1280, 720, 0, 0);
			
			// drawing all the obstacles
	
			// level 1: cars
			drawImage(car1Ptr, 100, 60, car1x, car1y);
			car1x = car1x - 10;
			if (car1x<0) {
				car1x = 1180;
			}
			drawImage(car2Ptr, 100, 60, car2x, car2y);
			car2x = car2x + 5;
			if (car2x+100>1280) {
				car2x = 0;
			}
			drawImage(car2Ptr, 100, 60, secondCar2x, car2y); // drawing a second car in lane 2
			secondCar2x += 15;
			if (secondCar2x+100>1280) {
				secondCar2x = 0;
			}
			drawImage(car3Ptr, 100, 60, car3x, car3y);
			//drawImage(car3Ptr, 100, 60, car3x-200, car3y);
			car3x = car3x - 15;
			if(car3x<0){
				car3x = 1180;
			}
			drawImage(car4Ptr, 180, 60, car4x, car4y);
			car4x = car4x - 10;
			if(car4x<0){
				car4x = 1100;
			} 
			drawImage(car1Ptr, 100,60, car5x, car5y);
			car5x = car5x - 20;
			if(car5x<0){
				car5x = 1180;
			} 

			// level 2: lilypad, log, turtles

			drawImage(lilypadPtr, 80, 60, lilypad1x, lilypad1y); // drawing 3 lilypads in lane 1
			lilypad1x += 10; 
			if (lilypad1x+80>1280) {
				lilypad1x = 0;
			}
			drawImage(lilypadPtr, 80, 60, secondlilypad1x, lilypad1y);
			secondlilypad1x += 10;
			if (secondlilypad1x+80>1280) {
				secondlilypad1x = 0;
			}
			drawImage(lilypadPtr, 80, 60, thirdlilypad1x, lilypad1y);
			thirdlilypad1x += 10;
			if (thirdlilypad1x+80>1280) {
				thirdlilypad1x = 0;
			}
			drawImage(lilypadPtr, 80, 60, lilypad2x, lilypad2y); // drawing 3 lilypads in lane 3
			lilypad2x -= 6;
			if (lilypad2x<0){
				lilypad2x = 1200;
			} 
			drawImage(lilypadPtr, 80, 60, secondlilypad2x, lilypad2y);
			secondlilypad2x -= 6;
			if (secondlilypad2x<0) {
				secondlilypad2x = 1200;
			}
			drawImage(lilypadPtr, 80, 60, thirdlilypad2x, lilypad2y);
			thirdlilypad2x -= 6;
			if (thirdlilypad2x<0) {
				thirdlilypad2x = 1200;
			}
			drawImage(logPtr, 240, 60, logx, logy);	// drawing 3 logs as well
			logx += 8;
			if (logx+240>1280){
				logx = 0;
			} 
			drawImage(logPtr, 240, 60, secondlogx, logy);	// drawing 3 logs as well
			secondlogx += 8;
			if (secondlogx+240>1280){
				secondlogx = 0;
			} 
			drawImage(logPtr, 240, 60, thirdlogx, logy);	// drawing 3 logs as well
			thirdlogx += 8;
			if (thirdlogx+240>1280){
				thirdlogx = 0;
			} 
			drawImage(turtlesPtr, 300, 60, turtlesx, turtlesy);
			turtlesx -= 12;
			if (turtlesx<0){
				turtlesx = 980;
			} 

			drawImage(frogPtr, 60, 60, xfrog, yfrog);
			drawCanvas();

			// doing collision detection

			// loop through 60 top 60 pixels of the frog (north border)
			for (int i=0; i<60; i++) {
				// checking collissions for all cars in level 1
				if (car1x + 100 == xfrog+i && car1y == yfrog || car1x == xfrog+i && car1y == yfrog || // if top right pixel or top left pixel of car hits frog north border && y coord of car and frog are same
					car2x + 100 == xfrog+i && car2y == yfrog || car2x == xfrog+i && car2y == yfrog || // same thing with car2
					secondCar2x + 100 == xfrog+i && car2y == yfrog || secondCar2x == xfrog+i && car2y == yfrog || // and so on...
					car3x + 100 == xfrog+i && car3y == yfrog || car3x == xfrog+i && car3y == yfrog || 
					car4x + 180 == xfrog+i && car4y == yfrog || car4x == xfrog+i && car4y == yfrog ||
					car5x + 100 == xfrog+i && car5y == yfrog || car5x == xfrog+i && car5y == yfrog )
				{
					lives--;		
					xfrog = 610;		
					yfrog = 660;
					drawCanvas();
					drawImage(frogPtr, 60, 60, xfrog, yfrog); // reset the frog location to starting place	
				}
				
				// collision with lilyads in lane 1
				if (lilypad1x + 40 == xfrog+i && lilypad1y == yfrog || // if frog north border hits top middle pixel of lilypad
					secondlilypad1x + 40 == xfrog+i && lilypad1y == yfrog || 
					thirdlilypad1x + 40 == xfrog+i && lilypad1y == yfrog) {
					// move frog along with the lilypad
					drawCanvas();
					xfrog += 10;
					drawImage(frogPtr, 60, 60, xfrog, yfrog);
					
				// collision with logs lane 2
				// check if frog north border collides with any 1 of 4 pixels evenly spread out on the log
				} else if (logx + 30 == xfrog+i && logy == yfrog || logx + 90 == xfrog+i && logy == yfrog || logx + 150 == xfrog+i && logy == yfrog || logx + 210 == xfrog+i && logy == yfrog || 
						   secondlogx + 30 == xfrog+i && logy == yfrog || secondlogx + 90 == xfrog+i && logy == yfrog || secondlogx + 150 == xfrog+i && logy == yfrog || secondlogx + 210 == xfrog+i && logy == yfrog ||
						   thirdlogx + 30 == xfrog+i && logy == yfrog || thirdlogx + 90 == xfrog+i && logy == yfrog || thirdlogx + 150 == xfrog+i && logy == yfrog || thirdlogx + 210 == xfrog+i && logy == yfrog)
				{
					// move frog along with the lilypad
					drawCanvas();
					xfrog += 8;
					drawImage(frogPtr, 60, 60, xfrog, yfrog);

				// collisions with lilypads in lane 3
				} else if (lilypad2x + 40 == xfrog+i && lilypad2y == yfrog || // if frog north border hits top middle pixel of lilypad
					secondlilypad2x + 40 == xfrog+i && lilypad2y == yfrog || 
					thirdlilypad2x + 40 == xfrog+i && lilypad2y == yfrog)
					{
					// move frog along with the lilypad
					drawCanvas();
					xfrog -= 6;
					drawImage(frogPtr, 60, 60, xfrog, yfrog);
				
				// collisions with turtles (any 1 of 5 pixels)
				} else if (turtlesx + 30 == xfrog+i && turtlesy == yfrog ||
						   turtlesx + 90 == xfrog+i && turtlesy == yfrog ||
						   turtlesx + 150 == xfrog+i && turtlesy == yfrog ||
						   turtlesx + 210 == xfrog+i && turtlesy == yfrog ||
						   turtlesx + 270 == xfrog+i && turtlesy == yfrog ) 
					{
					// move frog along with the lilypad
					drawCanvas();
					xfrog -= 12;
					drawImage(frogPtr, 60, 60, xfrog, yfrog);
				}

						   
				/* something wrong here
				else if (yfrog<=240) {
					// frog drowns in water :( reset to starting place
					lives--;		
					xfrog = 610;		
					yfrog = 660;
					drawCanvas();
					drawImage(frogPtr, 60, 60, xfrog, yfrog);
				} 
				*/
			
			}






			for (int i=1;i<=16;i++) {		// loop through each button
        		Wait(6);
        		Write_Clock(0);
    			Wait(6);
    			int b = Read_Data();
    			buttons[i] = b;

				if (b==0) {					// if the button is pressed
					InterpretButtons(i);	// call interpretButtons to update the screen based on what button is pressed
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
 return ((int)rand()/RAND_MAX) * (max - min) + min;
}


/* main function */
int main() {

	moves = 40;
	lives = 4;	// initializing moves to 40 and lives to 4
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

	int cursor = Menu_Read_SNES();	// reading controller input

	if (cursor == 1) { // cursor is on 'QUIT GAME'
		drawImage(blackscreenPtr, 1280, 720, 0, 0); // print a black screen
		drawCanvas();
		exit(0);
	}

	// else, starting the level

	drawImage(level1_2Ptr, 1280, 720, 0, 0); // drawing the level background
	drawImage(frogPtr, 60, 60, 610, 660); // drawing frog icon in center of the level
	drawCanvas();
	xfrog = 610;	// init variables for starting place coordinates of frog
	yfrog = 660;

	Game_Read_SNES();

	/*
	drawImage(car1Ptr, 100, 60, car1x, car1y);
	// car animation test
	while(1) {
		drawImage(car1Ptr, 100, 60, car1x, car1y);
		car1x = car1x - 10;
		delayMicroseconds(30000);
		drawImage(level1_2Ptr, 1280, 720, 0, 0);
		}
	*/

	return 0;
}
