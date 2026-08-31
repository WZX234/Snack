#include <time.h>    //for srand()
#include <stdio.h>   //for input and output
#include <conio.h>   //for getch() and kbhit()
#include <stdlib.h>  //for rand(), system(), exit() and srand()
#include <string.h>  //for strcat() and sprintf()
#include <windows.h> //for Sleep()

//===========================macro definitions===================================
#define FOODS 16        //quantity of food
#define SLEEP_TIME 300  //pause time
#define DIFFI 0.85      //difficulty level

//===========================function declarations===================================
void init        (void)     ;    //initialize the game
void span_food   (void)     ;    //spawn food
char get_dir     (void)     ;    //get the direction of snack
int  run         (int dir)  ;    //run one frame
int  opp_dir     (int dir)  ;    //get the opposite direction of snack
void redr_screen (void)     ;    //redraw screen
int  cont_play   (void)     ;    //continue playing ?

//===========================type definitions===================================
enum //snack state
{
	WELL = 0,
	GETED,
	OVER = 4
};

enum //snack direction
{
	UP = 0,
	RIGHT,
	DOWN,
	LEFT
};

enum //snack type
{
	NULL_ = 0,
	SNACK,
	FOOD
};

typedef struct //snack body
{
	unsigned int about: 2;  //about: 0-null, 1-snack, 2-food
	unsigned int dir  : 2;  //direction: 0-up, 1-right, 2-down, 3-left
}snack_body;

typedef struct //snack head and tail
{
	unsigned int x: 4;  //x coordinate
	unsigned int y: 5;  //y coordinate
}where;

//===========================global variables===================================
int snack_len;             //length of snack
char buffer[6144];          //screen buffer
int last_dir;              //previous direction
snack_body snack[16][32];  //snack body
where head, tail;          //snack head and tail

int main (void)
{
	//welcome screen
	puts("\033[93m=================================\033[0m");
	puts("\033[93m|| This is a game called SNACK ||\033[0m");
	puts("\033[93m=================================\033[0m");
	puts("\n\033[33mControl:\033[0m");
	puts("\033[33m   W   : up\n   A   : left\n   S   : down\n   D   : right\n SPACE : pause\n\033[0m");
	puts("\nPress any key to start:");

	//wait for key press
	while (kbhit())
	    Sleep(100);
	while (!kbhit())
	    Sleep(100);
	
	int shack_state, next_dir;
	_init: //label for re-initialization
		init(); //initialize the game

//===========================main game loop===================================
		int sleep_time;
		while(snack_len < 512)  //run until the snack length reaches 512
		{
			sleep_time = SLEEP_TIME - snack_len * DIFFI;  //calculate the sleep time based on the snack length
			Sleep(sleep_time > 10 ? sleep_time : 10);     //sleep for a while

			process_input:
				switch(get_dir())   //process the input direction
				{
					case('W'):
					case('w'):
						next_dir = UP;
						break;
					case('S'):
					case('s'):
						next_dir = DOWN;
						break;
					case('A'):
					case('a'):
						next_dir = LEFT;
						break;
					case('D'):
					case('d'):
						next_dir = RIGHT;
						break;
					case(' '):
						//pause the game until a key is pressed
						while (kbhit())
						{
							getch();
						    Sleep(100);
						}
						while (!kbhit())
						    Sleep(100);
						while (kbhit())
						{
							getch();
						    Sleep(100);
						}
						goto process_input;
					default:
						next_dir = last_dir;
						break;
				}
				
				//prevent the snack from moving in the opposite direction
				if (opp_dir(next_dir) == last_dir)
				    next_dir = last_dir;
				shack_state = run(next_dir);

				//check the state of the snack
				if (shack_state == OVER)
				{
					//if the player wants to continue playing, re-initialize the game
					if (cont_play())
						goto _init;
					//else, exit the game
					return 0; 
				}
				redr_screen();
				//if the snack has eaten food, play a sound
				if (shack_state == GETED)
					putchar('\a');
		}
		puts("YOU WIN! ");
		//if the player wants to continue playing, re-initialize the game
		if (cont_play())
			goto _init;
		//else, exit the game
		return 0;
}

//===========================function definitions===================================
void init (void)
{
	fputs("\033[?25l\033[2J", stdout);//initialize the console cursor to be invisible and clear the screen
	snack_len = 3; //initialize the length of the snack
	last_dir = UP; //initialize the previous direction
	head.x = 10; head.y = 16; //initialize the head position
	tail.x = 12; tail.y = 16; //initialize the tail position

	//mould a shack
	for (int i = 0; i < 16; i++)
	    for (int j = 0; j < 32; j++)
	        snack[i][j].about = snack[i][j].dir = UP;
	snack[10][16].about = snack[11][16].about = snack[12][16].about = SNACK;

	//spawn food
	srand((unsigned int)time(NULL));
	for (int i = 0; i < FOODS; i++)
		span_food();

	//redraw the screen
	redr_screen();
}

void span_food (void)
{
	int i = 0;
	int x, y;
	//spawn food in a random position that is not occupied by the snack or other food
	do
	{
		x = rand() % 16;
		y = rand() % 32;
		i++;
	}
	while ( (snack[x][y].about == SNACK || snack[x][y].about == FOOD) && i < 1000);
	
	//if the position is valid, place food there
	if (i < 1000)
		snack[x][y].about = FOOD;
}

char get_dir(void)
{
	//if a key is pressed, return the character of the key
	if(kbhit())
	    return getch();
	return 0;
}

int run (int dir)
{
	int ret = WELL;  //return value: 0-well, 1-geted, 4-over
	last_dir = snack[head.x][head.y].dir = dir;  //update the direction of the head
	int head_x_sto = head.x;  //store the previous x coordinate of the head
	int head_y_sto = head.y;  //store the previous y coordinate of the head

	//update the position of the head based on the direction
	switch(dir)
	{
		case(UP):
			head.x--;
			break;
		case(RIGHT):
			head.y++;
			break;
		case(DOWN):
			head.x++;
			break;
		case(LEFT):
			head.y--;
			break;
		default:
			puts("E1");
			exit(1);
	}

	//check if the snack has collided with itself or the wall
	if ( (snack[head.x][head.y].about == SNACK  &&  ( head.x != tail.x  ||  head.y != tail.y ) ) || head.x - head_x_sto == 15 || head_x_sto - head.x == 15 || head.y - head_y_sto == 31 || head_y_sto - head.y == 31)
    {
    	puts("GAME OVER");
    	return OVER;
	}

	//check if the snack has eaten food or moved to an empty space
	if (snack[head.x][head.y].about == FOOD)
	{
		snack[head.x][head.y].about = SNACK; //update the position of the head to be occupied by the snack
		snack_len++;  //increase the length of the snack
		ret = GETED;  //set the return value to indicate that food has been eaten
		span_food();  //spawn new food
	}
	//if the snack has moved to an empty space, update the tail position
	else if (snack[head.x][head.y].about == NULL_)
	{
		//update the position of the head to be occupied by the snack and the position of the tail to be empty
		snack[head.x][head.y].about = SNACK;
		snack[tail.x][tail.y].about = NULL_;

		//update the position of the tail based on its direction
		switch(snack[tail.x][tail.y].dir)
		{
			case(UP):
				tail.x--;
				break;
			case(RIGHT):
				tail.y++;
				break;
			case(DOWN):
				tail.x++;
				break;
			case(LEFT):
				tail.y--;
				break;
			default:
				//if the direction is invalid, print an error message and exit
				puts("E2");
				exit(1);
		}
	}
	else//if (snack[head.x][head.y].about == SNACK)
	{
		//update the position of the head to be occupied by the snack and the position of the tail to be empty
		snack[head.x][head.y].about = SNACK;
		//update the position of the tail based on its direction
		switch(snack[tail.x][tail.y].dir)
		{
			case(UP):
				tail.x--;
				break;
			case(RIGHT):
				tail.y++;
				break;
			case(DOWN):
				tail.x++;
				break;
			case(LEFT):
				tail.y--;
				break;
			default:
				//if the direction is invalid, print an error message and exit
				puts("E3");
				exit(1);
		}
	}
	return ret;
}

int opp_dir (int dir)
{
	//return the opposite direction of the given direction
	switch(dir)
	{
		case(UP):
			return DOWN;
		case(RIGHT):
			return LEFT;
		case(DOWN):
			return UP;
		case(LEFT):
			return RIGHT;
		default:
		    //if the direction is invalid, print an error message and exit
			puts("E4");
			exit(1);
	}
}

void redr_screen (void)
{
	//move the cursor to the top-left corner of the console window
	fputs("\033[H", stdout);

	//clear the buffer and prepare to redraw the screen
	buffer[0] = 0;
	//convert the length of the snack to a string and append it to the buffer
	char snack_len_format[5];
	sprintf(snack_len_format, "%d", snack_len); //convert the length of the snack to a string
	strcat(buffer, "\033[94m SNACK: "); //append the string " SNACK: " to the buffer
	strcat(buffer, snack_len_format); //append the length of the snack to the buffer
	//draw the snack and food on the screen
	strcat(buffer, "\n\033[91mDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");
	for(int i = 0; i < 16; i++)
	{
		strcat(buffer, "\033[91mD");
		for(int j = 0; j < 32; j++)
		{
			switch(snack[i][j].about)
			{
				case(NULL_): //if the position is empty, append a space to the buffer
					strcat(buffer, " ");
					break;
				case(SNACK): //if the position is occupied by the snack, append "@" to the buffer
					strcat(buffer, "\033[32m@");
					break;
				case(FOOD): //if the position is occupied by food, append "$" to the buffer
					strcat(buffer, "\033[33m$");
					break;
				default:
					//if the position is invalid, print an error message and exit
					puts("E5");
					exit(1);
			}
		}
		strcat(buffer, "\033[91mD\n");
	}
	strcat(buffer, "\033[91mDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\033[0m\n");
	//print the buffer to the console and flush the output
	printf("%s", buffer);
	fflush(stdout);
}

int cont_play (void)
{
	//make the console cursor visible again
	fputs("\033[?25h", stdout);

	//ask the player if they want to continue playing
	puts("Play again ? (y/n):");
	//wait for the player to press a key
	char c = getchar();
	//if the player wants to continue playing, clear the input buffer and return 1
	if (c == 'y' || c == 'Y')
	{
		while(getchar() != '\n')
		    continue;
		return 1;
	}
	//if the player does not want to continue playing, clear the input buffer and return 0
	if (c != '\n')
		while(getchar() != '\n')
			continue;
	
	//return 0 to indicate that the player does not want to continue playing
	puts("\n\n-------------------------");
	puts("Press any key to quit:");
	//wait for the player to press a key
	while (kbhit())
	    Sleep(100);
	while (!kbhit())
	    Sleep(100);
	return 0;
}
