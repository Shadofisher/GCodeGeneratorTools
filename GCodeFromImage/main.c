// Win32Project3.cpp : Defines the entry point for the console application.
//

//#include <stdafx.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//#define XRES 354
//#define YRES 354
//#define xmm 100
//#define ymm 100



unsigned int start_gcode;
typedef struct _pixformat
{
	unsigned int x;
	unsigned int y;
	unsigned int used;
} Pixformat;

unsigned int counter;
Pixformat * ptr;

unsigned int search(Pixformat * srch, unsigned int count);
unsigned int count(Pixformat * srch, unsigned int count);
int main(int argc, char **argv[])
{
	unsigned short data;
	unsigned int n, index;
	Pixformat * current_ptr;
	char tmp[102400];
	start_gcode = 0;
	FILE * fp;
	FILE * dp;
	counter=0;
	unsigned int XRES;
	unsigned int YRES;
	unsigned int xmm;
	unsigned int ymm;

	printf("\n\rArumnets: %d", argc);

	if (argc < 7)
	{
		printf("Not enough arguments!!!:  Call app with:  xwidth(pixel) ywidth(pixel) xwidth(mm) ywidth(mm) <input file> <output file>");
		exit(1);
	}

	XRES = atoi((const char *)argv[1]);
	YRES = atoi((const char *)argv[2]);
	xmm =  atoi((const char *)argv[3]);
	ymm =  atoi((const char *)argv[4]);
	ptr = (Pixformat *)malloc(40000000);

//	if ((fopen_s(&fp, "C:\\Users\\graeme\\Pictures\\test.data", "rb+")) != 0)
	//if ((fopen(&fp,(const char *)argv[5],"rb+")) != 0)
    if((fp=fopen((const char *)argv[5],"rb+"))==NULL)
	{
		perror("\n\rFile Error1");
		free(ptr);
		exit(1);
	}else
	{
		for (int y = 0; y < YRES; y++)
		{
			for(int x = 0; x < XRES; x++)
			{
				fread(&data,1,1,fp);
				//if (data > 0)
					//printf("\n\rData: 0x%x", data);
				if (data == /*0xff00*/0)
				{
					ptr[counter].used=1;
					ptr[counter].x=x;
					ptr[counter].y=y;
					counter++;
				}
			}
		}

	}


//	if ((fopen_s(&dp, (const char *)argv[6], "w+")) != 0)
    if((dp=fopen((const char *)argv[6],"w+"))==NULL)
	{
		perror("\n\rFile Error2");
		free(ptr);
		exit(1);
	}
	else
	{
        printf("\n\rNumber of pixels: %d",counter);
		for (n = 0; n < counter; n++)
		{
			current_ptr = &ptr[n];
			if (current_ptr->used == 1)
			{
				do
				{
					printf("\n\rCurrent: %d %d",current_ptr->x,current_ptr->y);

					if (start_gcode == 0)
					{
						start_gcode = 1;
						printf("\n\rStarting gcode");
						fputs("F800\n", dp);
						fputs("G0Z3F400\n", dp);
						sprintf(tmp, "G1X%fY%fF800\n", (double)(current_ptr->x * xmm)/(XRES), (double)(current_ptr->y * ymm) / (YRES));
						fputs(tmp,dp);
						fputs("G0Z-1F400\n", dp);
					}
					if ((index = search(current_ptr, counter)) != 0)
					{
						//printf("\n\rLink found at: 0x%d", index);
						//printf("\n\rNumber of pixels connected: %d", count(current_ptr, counter));
						current_ptr->used = 0;
						current_ptr = (&ptr[index]);
						sprintf(tmp, "G0X%fY%fF800\n", (double)(current_ptr->x * xmm)/(XRES), (double)(current_ptr->y * ymm) / (YRES));
						fputs(tmp, dp);
						printf("\n\rCurrent Y Value: %f", (double)(current_ptr->y * ymm) / (YRES));
					}
					else
					{
						if (start_gcode == 1)
						{
							start_gcode = 0;
							//fputs("G0Z3", dp);
							printf("\n\rEnding gcode");
						}
					}
				} while (index > 0);
			}
		}

	}


	fclose(dp);
	fclose(fp);
	free(ptr);
	printf("\n\rDone!!!");
	return 0;
}

unsigned int search(Pixformat * srch, unsigned int count)
{
	static Pixformat tmp;
	unsigned int found = 0;
	unsigned int n,index;

	found = 0;
	index = 0;
	for (n = 0; n < count; n++)
	{
		if ((srch->x != 0) && (srch->y!=0))
		{
			if ((((srch->x - 1) == ptr[n].x) && (srch->y == ptr[n].y)) && (srch->used == 1) && (ptr[n].used==1))     // Left.
			{
				//printf("\n\rWe have connected pixel:LEFT");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x - 1) == ptr[n].x) && (srch->y - 1 == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))  //Top left
			{
				//printf("\n\rWe have connected pixel:Top Left");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x) == ptr[n].x) && (srch->y - 1 == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))	//Top
			{
				//printf("\n\rWe have connected pixel: Top");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x + 1) == ptr[n].x) && (srch->y - 1 == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))	//Top Right
			{
			//	printf("\n\rWe have connected pixel: Top Right");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x + 1) == ptr[n].x) && (srch->y == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))	//Right
			{
				//printf("\n\rWe have connected pixel: Right");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x + 1) == ptr[n].x) && (srch->y + 1 == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))	//Bottom Right
			{
				//printf("\n\rWe have connected pixel: Bottom Right");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x) == ptr[n].x) && (srch->y + 1 == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))	//Bottom
			{
				//printf("\n\rWe have connected pixel: Bottom");
				index = n;
				n = count;
				found = 1;
			}else
			if ((((srch->x-1) == ptr[n].x) && (srch->y + 1 == ptr[n].y)) && (srch->used == 1) && (ptr[n].used == 1))	//Bottom Left
			{
				//printf("\n\rWe have connected pixel: Bottom Left");
				index = n;
				n = count;
				found = 1;
			}
		}
		else
		{

		}
	}

	if (found)
		return (index);
	else
		return (0);
}

unsigned int count(Pixformat * srch, unsigned int count)
{
	static Pixformat tmp;
	unsigned int found = 0;
	unsigned int n, index,number_of_pixels;

	found = 0;
	index = 0;
	number_of_pixels = 0;
	for (n = 0; n < count; n++)
	{
		if ((srch->x != 0) && (srch->y != 0))
		{
			if ((((srch->x - 1) == ptr[n].x) && (srch->y == ptr[n].y)) && (srch->used == 1))     // Left.
			{
				//printf("\n\rWe have connected pixel:LEFT");
				number_of_pixels++;
			}
			if ((((srch->x - 1) == ptr[n].x) && (srch->y - 1 == ptr[n].y)) && (srch->used == 1))  //Top left
			{
				//printf("\n\rWe have connected pixel:Top Left");
				number_of_pixels++;
			}
			if ((((srch->x) == ptr[n].x) && (srch->y - 1 == ptr[n].y)) && (srch->used == 1))	//Top
			{
				//printf("\n\rWe have connected pixel: Top");
				number_of_pixels++;
			}

			if ((((srch->x + 1) == ptr[n].x) && (srch->y - 1 == ptr[n].y)) && (srch->used == 1))	//Top Right
			{
				//printf("\n\rWe have connected pixel: Top Right");
				number_of_pixels++;
			}

			if ((((srch->x + 1) == ptr[n].x) && (srch->y == ptr[n].y)) && (srch->used == 1))	//Right
			{
				//printf("\n\rWe have connected pixel: Right");
				number_of_pixels++;
			}
			if ((((srch->x + 1) == ptr[n].x) && (srch->y + 1 == ptr[n].y)) && (srch->used == 1))	//Bottom Right
			{
				//printf("\n\rWe have connected pixel: Bottom Right");
				number_of_pixels++;
			}

			if ((((srch->x) == ptr[n].x) && (srch->y + 1 == ptr[n].y)) && (srch->used == 1))	//Bottom
			{
				//printf("\n\rWe have connected pixel: Bottom");
				number_of_pixels++;
			}
			if ((((srch->x - 1) == ptr[n].x) && (srch->y + 1 == ptr[n].y)) && (srch->used == 1))	//Bottom Left
			{
				//printf("\n\rWe have connected pixel: Bottom Left");
				number_of_pixels++;
			}

		}
		else
		{

		}
	}

	return number_of_pixels;
}


















