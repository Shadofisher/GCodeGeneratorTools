// Nearest neighbour.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <math.h>

void append_gcodepath(unsigned int index, char * path1, char * path2);

typedef struct _pixformat
{
	char x[64];
	char y[64];
	unsigned int used;
} Pixformat;

unsigned int counter;
Pixformat * ptrStart;
Pixformat * ptrEnd;

unsigned int paths = 0;

//unsigned char write_path(char * searchpattern, unsigned int mode);

unsigned int  search_nerestneighbour(char * x, char * y, unsigned int count, unsigned int element);

FILE * fp;
FILE * dp;

int main(int argc, char * argv[])
{
	FILE * fp;
	FILE * dp;
	unsigned int count = 0;
	unsigned int n = 0;
	char * pos;
	unsigned int alternate = 0;
	char xEnd[64];
	char yEnd[64];


	unsigned int firstLine = 0;
	unsigned int index     = 0;


	char oneline[1024];
	char previous_oneline[1024];

	char  current_substring[1024];
	char previous_substring[1024];

	ptrStart = (Pixformat *)malloc(10000000);
	ptrEnd   = (Pixformat *)malloc(10000000);

	if (argc < 3)
	{
		printf("Not enough arguments!!!:  Call app with:  <input file> <output file>");
		exit(1);
	}


//	if ((fopen_s(&fp, argv[1], "r+")) != 0)
    if((fp=fopen((const char *)argv[1],"r+"))==NULL)
	{
		perror("\n\rError:");
		exit(1);
	}
	else
	{
		while (count = fgets(oneline, 1024, fp) != 0)
		{
			if (firstLine == 1)
			{
				if ((strstr(oneline, "F800") != NULL) && ((strstr(oneline, "G1X") == NULL) && (strstr(oneline, "G0X") == NULL)))
				{
					index = 0;
					if ((strstr(previous_oneline, "G0Z") == NULL))
					{
						if ((pos = (char *)strchr(previous_oneline, 'X')) != NULL)
						{
							while (*pos != 'Y')
							{
								pos++;
								if (*pos != 'Y')
									ptrEnd[paths].x[index++] = *pos;
							}
							ptrEnd[paths].x[index] = 0;
							index = 0;
						}
						if ((pos = (char *)strchr(previous_oneline, 'Y')) != NULL)
						{
							while (*pos != 'F')
							{
								pos++;
								if (*pos != 'F')
									ptrEnd[paths].y[index++] = *pos;
							}
							ptrEnd[paths].y[index] = 0;
							index = 0;
						}
						//ptrStart[paths].used = 1;
						ptrEnd[paths].used = 1;
						paths++;
					}
					strcpy(previous_oneline, oneline);

				}
				else
				{
					strcpy(previous_oneline, oneline);
				}
			}
			else
			{
				strcpy(previous_oneline, oneline);
				firstLine = 1;
			}
		}
		rewind(fp);
		//printf("\n\rNumber of ends = %d", paths);
		//for (n = 0; n < paths; n++)
		//{
		//	printf("\n\rENDS: X %s  Y %s", ptrEnd[n].x, ptrEnd[n].y);
		//}
		printf("\n\rNumber of path ends: %d", paths);
		paths = 0;
		index = 0;
		while (count = fgets(oneline, 1024, fp) != 0)
		{
			if (firstLine == 1)
			{
				if ((strstr(oneline, "F800") != NULL) && ((strstr(oneline, "G1X") != NULL) || (strstr(oneline, "G0X") != NULL)))
				{
					index = 0;
					if ((strstr(previous_oneline, "G0Z3F400") != NULL))
					{
						if ((pos = (char *)strchr(oneline, 'X')) != NULL)
						{
							while (*pos != 'Y')
							{
								pos++;
								if (*pos != 'Y')
									ptrStart[paths].x[index++] = *pos;
							}
							ptrStart[paths].x[index] = 0;
							index = 0;
						}
						if ((pos = (char *)strchr(oneline, 'Y')) != NULL)
						{
							while (*pos != 'F')
							{
								pos++;
								if (*pos != 'F')
									ptrStart[paths].y[index++] = *pos;
							}
							ptrStart[paths].y[index] = 0;
							index = 0;
						}
						ptrStart[paths].used = 1;
						paths++;
					}
					strcpy(previous_oneline, oneline);

				}
				else
				{
					strcpy(previous_oneline, oneline);
				}
			}
			else
			{
				strcpy(previous_oneline, oneline);
				firstLine = 1;
			}
		}

	}
	fclose(fp);
	//
	//for (n = 0; n < paths; n++)
	//{
	//	printf("\n\rStarts: X %s  Y %s End: X %s Y %s", ptrStart[n].x, ptrStart[n].y, ptrEnd[n].x, ptrEnd[n].y);
	//}
	printf("\n\rNumber of path Starts: %d",paths);

	//Distance beatween twoe points

	double x_end = atof(ptrStart[0].x);
	double y_end = atof(ptrStart[0].y);
	double x_start = atof(ptrStart[50].x);
	double y_start = atof(ptrStart[50].y);


	double distance = sqrt((x_end-x_start)*(x_end - x_start) + (y_end-y_start)*(y_end-y_start));

	//printf("\n\rDistance: %f  %f %f %f %f",distance, x_end,y_end,x_start,y_start);

	index = 0;
	for (n = 0; n < (paths - 1); n++)
	{
		if (ptrEnd[index].used)
		{
			ptrEnd[index].used = 0;
			index = search_nerestneighbour(ptrEnd[index].x, ptrEnd[index].y, paths, n);
			//printf("\n\rNearest Neighbour at: %s", ptrEnd[index].x,ptrStart[index].y);
			printf("\n\rAppending...");
			append_gcodepath(index,argv[1],argv[2]);

		}
	}


	unsigned int paths_left = 0;
	for (n = 0; n < (paths - 1); n++)
	{
		if (ptrStart[n].used)
			paths_left++;
	}

	printf("\n\rNumber: Number of paths left: %d ", paths_left);


}



void append_gcodepath(unsigned int index,char * path1,char * path2)
{
	char search_string[1024];
	char oneline[1024];
	unsigned int count;
	unsigned int start;

	start = 0;

	sprintf(search_string,"X%sY%s", ptrStart[index].x, ptrStart[index].y);


//	if ((fopen_s(&fp, path1, "r+")) != 0)
    if((fp=fopen(path1,"r+"))==NULL)
	{
		perror("\n\rError:");
		exit(1);
	}
	else
	{
//		if ((fopen_s(&dp, path2, "a+")) != 0)
        if((dp=fopen(path2,"a+"))==NULL)
		{
			perror("\n\rError:");
			exit(1);
		}
		else
		{
			while (count = fgets(oneline, 1024, fp) != 0)
			{
				if (start == 0)
				{
					if ((strstr(oneline, search_string) != NULL))
					{
						fputs("F800\n", dp);
						fputs("G0Z3F400\n", dp);
						fputs(oneline, dp);
						start = 1;
					}
				}
				else
				{
					if ((strcmp(oneline, "F800\n") == NULL))
					{
						start = 0;
					}
					else
					{
						fputs(oneline, dp);
					}
				}
			}
		}


		fclose(fp);
		fclose(dp);
	}
}


unsigned int search_nerestneighbour(char * x, char * y,unsigned int count,unsigned int element)
{
	unsigned int n;
	double x_pos;
	double y_pos;
	double distance;
	double lowest_distance;
	char tmp[1024];
	unsigned int index;
	static char nearest[1024];
	double x_end;
	double y_end;
	double x_start;
	double y_start;

	index = 0;
	distance = 0;
	lowest_distance = 9999999999999.0;
	x_pos = atof(x);
	y_pos = atof(y);


	for (n = 1; n < count; n++)
	{
		x_start = atof(ptrStart[n].x);
		y_start = atof(ptrStart[n].y);

		if ((ptrStart[n].used == 1) && (element != n))
		{
			distance = sqrt((x_pos - x_start)*(x_pos - x_start) + (y_pos - y_start)*(y_pos - y_start));
			if (distance < lowest_distance)
			{
				lowest_distance = distance;
				index = n;
			}
		}
	}

	//sprintf(nearest, "%s%s", ptrStart[index].x, ptrStart[index].y);
	strcpy(nearest, ptrStart[index].x);
	strcat(nearest, ptrStart[index].y);
	ptrStart[index].used = 0;	//dont use this path again!!!
	ptrStart[0].used = 0;	//dont use this path again!!!
	//printf("\n\rLowest distance: Index %d %s %s", index, ptrStart[index].x, ptrStart[index].y);
	return (index);

}


#if 0
unsigned char write_path(char * searchpattern, unsigned int mode, unsigned int count)
{
	FILE * dp;
	unsigned int n;
	char oneline[1024];
	unsigned int num;

	if ((fopen_s(&dp, "C:\\Users\\graeme\\Pictures\\test_gf.gc1", "w+")) != 0)
	{
		perror("\n\rError:");
		exit(1);
	}
	else
	{

		while (num = fgets(oneline, 1024, dp) != 0)
		{
			if ((strstr(oneline, searchpattern) != NULL))
			{
				printf("\n\rAppending path to file.....");
			}
		}
	}

	fclose(dp);
}
#endif



