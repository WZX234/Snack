#include<time.h>
#include<stdio.h>
#include<conio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>

//settings
#define FOODS 16
#define SLEEP_TIME 300

void init (void);
void make_food (void);
char get_drct(void);
int run (int tdrct);
int not_drct (int a);
void draw_screen (void);
int is_play (void);

typedef struct
{
	unsigned int about: 2;    //0:NULL, 1:snack, 2:food
	unsigned int drct : 2;
}snack_body;

typedef struct
{
	unsigned int x: 4;
	unsigned int y: 5;
}where;

int drct_2;
int slong;
char buffer[643];
int mem_drct;
snack_body snack[16][32];
where head, tear;

int main (void)
{
	puts("=================================");
	puts("|| This is a game called SNACK ||");
	puts("=================================");
	puts("\nControl:");
	puts(" W : up\n A : left\n S : down\n D : right");
	puts("\nPress any key to start:");
	while (kbhit())
	    Sleep(100);
	while (!kbhit())
	    Sleep(100);
	int is_over, next_drct;
	_init:
		init();
		while(slong < 512)
		{
			Sleep(SLEEP_TIME);
			switch(get_drct())
			{
				case('W'):
				case('w'):
					next_drct = 0;
					break;
				case('S'):
				case('s'):
					next_drct = 2;
					break;
				case('A'):
				case('a'):
					next_drct = 3;
					break;
				case('D'):
				case('d'):
					next_drct = 1;
					break;
				default:
					next_drct = mem_drct;
					break;
			}
			if (not_drct(next_drct) == mem_drct)
			    next_drct = mem_drct;
			is_over = run(next_drct);
			if (is_over == 4)
			{
				if (is_play())
					goto _init;
				return 0;
			}
			draw_screen();
			if (is_over)
				putchar('\a');
		}
		puts("YOU WIN! ");
		if (is_play())
			goto _init;
		return 0;
}

void init (void)
{
	system("cls");
	slong = 3;
	mem_drct = 0;
	head.x = 10; head.y = 16;
	tear.x = 12; tear.y = 16;
	for (int i = 0; i < 16; i++)
	    for (int j = 0; j < 32; j++)
	        snack[i][j].about = snack[i][j].drct = 0;
	snack[10][16].about = snack[11][16].about = snack[12][16].about = 1;
	srand((unsigned int)time(NULL));
	for (int i = 0; i < FOODS; i++)
		make_food();
	draw_screen();
}

void make_food (void)
{
	int i = 0;
	int x, y;
	do
	{
		x = rand() % 16;
		y = rand() % 32;
		i++;
	}
	while ( (snack[x][y].about == 1 || snack[x][y].about == 2) && i < 1000);
	
	if (i < 1000)
		snack[x][y].about = 2;
}

char get_drct(void)
{
	if(kbhit())
	    return getch();
	return 0;
}

int run (int tdrct)
{
	int ret = 0;
	mem_drct = snack[head.x][head.y].drct = tdrct;
	int hx = head.x;
	int hy = head.y;
	switch(tdrct)
	{
		case(0):
			head.x--;
			break;
		case(1):
			head.y++;
			break;
		case(2):
			head.x++;
			break;
		case(3):
			head.y--;
			break;
		default:
			putchar('1');
			exit(1);
	}
	if ( (snack[head.x][head.y].about == 1  &&  head.x != tear.x  &&  head.y != tear.y) || head.x - hx == 15 || hx - head.x == 15 || head.y - hy == 31 || hy - head.y == 31)
    {
    	puts("GAME OVER");
    	return 4;
	}
	if (snack[head.x][head.y].about == 2)
	{
		snack[head.x][head.y].about = 1;
		slong++;
		ret = 1;
		make_food();
	}
	else if (snack[head.x][head.y].about == 0)
	{
		snack[head.x][head.y].about = 1;
		snack[tear.x][tear.y].about = 0;
		switch(snack[tear.x][tear.y].drct)
		{
			case(0):
				tear.x--;
				break;
			case(1):
				tear.y++;
				break;
			case(2):
				tear.x++;
				break;
			case(3):
				tear.y--;
				break;
			default:
				putchar('2');
				exit(1);
		}
	}
	else
	{
		snack[head.x][head.y].about = 1;
		switch(snack[tear.x][tear.y].drct)
		{
			case(0):
				tear.x--;
				break;
			case(1):
				tear.y++;
				break;
			case(2):
				tear.x++;
				break;
			case(3):
				tear.y--;
				break;
			default:
				putchar('2');
				exit(1);
		}
	}
	return ret;
}

int not_drct (int a)
{
	switch(a)
	{
		case(0):
			return 2;
		case(1):
			return 3;
		case(2):
			return 0;
		case(3):
			return 1;
		default:
			putchar('3');
			exit(1);
	}
}

void draw_screen (void)
{
	COORD pos = {0, 0};
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	buffer[0] = 0;
	char slong_format[5];
	sprintf(slong_format, "%d", slong);
	strcat(buffer, " SNACK: ");
	strcat(buffer, slong_format);
	strcat(buffer, "\nDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");
	for(int i = 0; i < 16; i++)
	{
		strcat(buffer, "D");
		for(int j = 0; j < 32; j++)
		{
			switch(snack[i][j].about)
			{
				case(0):
					strcat(buffer, " ");
					break;
				case(1):
					strcat(buffer, "@");
					break;
				case(2):
					strcat(buffer, "$");
					break;
				default:
					putchar('4');
					exit(1);
			}
		}
		strcat(buffer, "D\n");
	}
	strcat(buffer, "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD\n");
	printf("%s", buffer);
	fflush(stdout);
}

int is_play (void)
{
	puts("Play again ? (y/n):");
	char c = getchar();
	if (c == 'y' || c == 'Y')
	{
		while(getchar() != '\n')
		    continue;
		return 1;
	}
	if (c != '\n')
		while(getchar() != '\n')
			continue;
	puts("\n\n-------------------------");
	puts("Press any key to quit:");
	while (kbhit())
	    Sleep(100);
	while (!kbhit())
	    Sleep(100);
	return 0;
}
