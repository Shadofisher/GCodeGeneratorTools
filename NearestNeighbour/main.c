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


typedef struct _new_pixformat
{
	char x_start[64];
	char y_start[64];
	char x_end[64];
	char y_end[64];
	unsigned int path_used;
	unsigned int path_start_index;   //An index into FileData wher the path starts
	unsigned int path_end_index;   //An index into FileData wher the path starts
} NewPixformat;


unsigned int counter;
Pixformat * ptrStart;
Pixformat * ptrEnd;

unsigned int paths = 0;

//unsigned char write_path(char * searchpattern, unsigned int mode);

unsigned int  search_nerestneighbour(char * x, char * y, unsigned int count, unsigned int element);

FILE * fp;
FILE * dp;


char FileData[1000000][128];

NewPixformat * GC_Paths;

static unsigned int findPaths(unsigned int number_of_lines)
{
    static unsigned int n,reverse,m;
    static unsigned int index;
    static unsigned int paths=0;
    char * pos;

    GC_Paths =  (NewPixformat *)(malloc(10000000));
    memset(GC_Paths,0,10000000);
    for (n=0;n<number_of_lines;n++)
    {
        if (n!=0)
        {
            if ((strstr((char *)FileData[n], "F800") != NULL) && ((strstr((char *)FileData[n], "G1X") == NULL) && (strstr((char *)FileData[n], "G0X") == NULL)))
            {
                index=0;
                if ((strstr((char *)FileData[n-1], "G0Z") == NULL))
				{
                    if ((pos = (char *)strchr((char *)FileData[n-1], 'X')) != NULL)
					{
						while (*pos != 'Y')
						{
							pos++;
							if (*pos != 'Y')
								GC_Paths[paths].x_end[index++] = *pos;
								//ptrEnd[paths].x[index++] = *pos;
						}
						GC_Paths[paths].x_end[index] = 0;
						index = 0;
                    }
                    if ((pos = (char *)strchr((char *)FileData[n-1], 'Y')) != NULL)
					{
						while (*pos != 'F')
						{
							pos++;
							if (*pos != 'F')
								GC_Paths[paths].y_end[index++] = *pos;
						}
						GC_Paths[paths].y_end[index] = 0;
						index = 0;
					}
					GC_Paths[paths].path_end_index = n-1;
					reverse = GC_Paths[paths].path_end_index;
					for(m = reverse; m > 0; m--)
					{
                        index = 0;
                        if (strstr((char *)FileData[m], "G0Z3F400") != NULL)
                        {
                            if ((pos = (char *)strchr((char *)FileData[m+1], 'X')) != NULL)
                            {
                                while (*pos != 'Y')
                                {
                                    pos++;
                                    if (*pos != 'Y')
                                        GC_Paths[paths].x_start[index++] = *pos;
								//ptrEnd[paths].x[index++] = *pos;
                                }
                                GC_Paths[paths].x_start[index] = 0;
                                index = 0;
                            }
                            if ((pos = (char *)strchr((char *)FileData[m+1], 'Y')) != NULL)
                            {
                                while (*pos != 'F')
                                {
                                    pos++;
                                    if (*pos != 'F')
                                        GC_Paths[paths].y_start[index++] = *pos;
                                }
                                GC_Paths[paths].y_start[index] = 0;
                                index = 0;
                            }
                            GC_Paths[paths].path_start_index = m+1;
                            m=1; //break out of loop
                        }
					}
					GC_Paths[paths].path_used=1;
					paths++;
                }
            }

        }
    }
    printf("\n\rThere are %d paths",paths);
    for (n = 0; n < paths; n++)
    {
        //printf("\n\rSTART: %s %s Index %d",GC_Paths[n].x_start,GC_Paths[n].y_start,GC_Paths[n].path_start_index);
        //printf("\n\rEND: %s %s Index %d",GC_Paths[n].x_end,GC_Paths[n].y_end,GC_Paths[n].path_end_index);
    }
    return paths;
}

static unsigned int fillArray(const char * filename )
{
    FILE * gcodefile;
    static unsigned int index = 0;
    static unsigned int count = 0;
    if((gcodefile=fopen(filename,"r+"))==NULL)
	{
		perror("\n\rError:");
		exit(1);
	}else
	{
        printf("\n\rRead from file....");
   		while (count = fgets((char *)FileData[index], 1024, gcodefile) != 0)
   		{
            index++;
   		}
   		printf("File contained %d lines",index);
	}
	fclose(gcodefile);
	return index;
}




static unsigned int findIndex(char * cuurent_x,char * cuurent_y,unsigned int numpaths)
{
    unsigned int n;

    double shortest_distance;
    double distance;
    unsigned int index;
    double current_x = atof(cuurent_x);
    double current_y = atof(cuurent_y);
    double path_x ;
    double path_y ;

    printf("\n\rStart %f %f",current_x,current_y);
    shortest_distance = 199999999.00000;

    for (n = 0; n < numpaths; n++)
    {
        if(GC_Paths[n].path_used == 1)
        {
            path_x = atof(GC_Paths[n].x_start);
            path_y = atof(GC_Paths[n].y_start);
            printf("\n\rPath Start %f %f",path_x,path_y);
            //distance = sqrt((x_end-x_start)*(x_end - x_start) + (y_end-y_start)*(y_end-y_start));
            distance = sqrt(pow((current_x - path_x),2) + pow((current_y - path_y),2));
            printf("\n\rDistance to new path = %f",distance);
            if (distance < shortest_distance)
            {
                shortest_distance = distance;
                index = n;
            }
        }
    }
    printf("\n\rShortest dstance: %f at index %d",shortest_distance,index);
    GC_Paths[index].path_used = 0;  //Don't evaluate path again
    return index;
}

unsigned int paths_stil_valid(unsigned int paths)
{
    unsigned int n,count;

    count = 0;
    for (n = 0; n < paths; n++)
    {
        if(GC_Paths[n].path_used == 1)
        {
            count++;
        }
    }
    printf("\n\rPaths left: %d",count);
    return (count);
}

void append_gcode(unsigned int index,char * path1)
{
    unsigned int n;
    unsigned int length;
    if((dp=fopen(path1,"a+"))==NULL)
    {
        perror("\n\rError:");
        exit(1);
    }
    else
    {
        fputs("F800\n", dp);
		fputs("G0Z3F400\n", dp);
		length = (GC_Paths[index].path_end_index - GC_Paths[index].path_start_index)+1;
		printf("\n\rNumber of new gcode lines to write %d",length);
		for (n = 0; n < length; n++)
        {

            fputs((char *)(FileData[GC_Paths[index].path_start_index + n]), dp);
        }
        fclose(dp);
    }
}


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
	unsigned int num_paths = 0;

	unsigned int num_lines = 0;


	unsigned int firstLine = 0;
	unsigned int index     = 0;
	unsigned int start_index     = 0;


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


	num_lines = fillArray((const char *)argv[1]);
	num_paths = findPaths(num_lines);
    GC_Paths[0].path_used=1;
    paths_stil_valid(num_paths);

    start_index = findIndex("0.00","0.00",num_paths);
    printf("\n\rClosest to origin: %d",start_index);

    append_gcode(start_index,argv[2]);


//    findIndex(GC_Paths[start_index].x_start,GC_Paths[start_index].y_start,num_paths);
//    printf("\n\rClosest to origin: %d",start_index);


    index = start_index;
#if 1
    while((paths_stil_valid(num_paths)) > 0)
    {
        start_index = findIndex(GC_Paths[index].x_end,GC_Paths[index].x_end,num_paths);
        append_gcode(start_index,argv[2]);
        index = start_index;
    }
#endif

  //  findIndex(GC_Paths[0].x_end,GC_Paths[0].y_end,num_paths);


    paths_stil_valid(num_paths);

	exit(1);


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



