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




static unsigned int findIndex(char * cuurent_x,char * cuurent_y,unsigned int numpaths, unsigned char * reverse)
{
    unsigned int n;
    FILE * start;
    double shortest_distance;
    double distance,end_distance;
    double total_distance;
    unsigned int index;
    double current_x = atof(cuurent_x);
    double current_y = atof(cuurent_y);
    double path_x ;
    double path_y ;
    double path_end_x ;
    double path_end_y ;
    char buf1[256];
    char buf2[256];
    char buf3[256];

    unsigned char rev;

    printf("\n\rStart %f %f",current_x,current_y);
    shortest_distance = 199999999.00000;

    for (n = 0; n < numpaths; n++)
    {
        if(GC_Paths[n].path_used == 1)
        {
            path_x = atof(GC_Paths[n].x_start);
            path_y = atof(GC_Paths[n].y_start);
            path_end_x = atof(GC_Paths[n].x_end);
            path_end_y = atof(GC_Paths[n].y_end);


//            printf("\n\rPath Start %f %f",path_x,path_y);
            //distance = sqrt((x_end-x_start)*(x_end - x_start) + (y_end-y_start)*(y_end-y_start));
            distance = sqrt(pow((current_x - path_x),2) + pow((current_y - path_y),2));
            end_distance = sqrt(pow((current_x - path_end_x),2) + pow((current_y - path_end_y),2));

            //printf("\n\rDistance to new path = %f",distance);
            if (end_distance < distance)
            {
                //printf("\n\rPossiblle further optimisattion here!!!! %f %f",distance,end_distance);
                total_distance = end_distance;
                rev = 1;
                //*reverse = 1;
            }else
            {
                total_distance = distance;
                rev = 0;
                //*reverse = 0;
            }
            if (total_distance < shortest_distance)
            {
                shortest_distance = total_distance;
                index = n;
                *reverse = rev;
            }
        }
    }

    path_x = atof(GC_Paths[index].x_start);
    path_y = atof(GC_Paths[index].y_start);
    path_end_x = atof(GC_Paths[index].x_end);
    path_end_y = atof(GC_Paths[index].y_end);

    distance = sqrt(pow((current_x - path_x),2) + pow((current_y - path_y),2));
    end_distance = sqrt(pow((current_x - path_end_x),2) + pow((current_y - path_end_y),2));
    sprintf(buf1,"Reverse %d Current %f %f Start %s %s  End %s %s Distance %f",*reverse,current_x,current_y,GC_Paths[index].x_start,GC_Paths[index].y_start,GC_Paths[index].x_end,GC_Paths[index].y_end,shortest_distance);
    sprintf(buf2,"\n\rDistance to end %f Distance to start %f",end_distance,distance);


    if((start=fopen("starts.txt","a+"))==NULL)
    {
      perror("\n\rError:");
      exit(1);
    }
    else
    {
        fputs(buf1, start);
        fputs(buf2, start);
        fputs("\n\r", start);
        fclose(dp);
    }


    //printf("\n\rShortest dstance: %f at index %d",shortest_distance,index);
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


void append_gcode_join_chain(unsigned int index,char * path1)
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
		length = (GC_Paths[index].path_end_index - GC_Paths[index].path_start_index)+1;
		printf("\n\rNumber of new gcode lines to write %d",length);
		FileData[GC_Paths[index].path_start_index][1] = '0';

		for (n = 0; n < length; n++)
        {
            if (strstr(FileData[GC_Paths[index].path_start_index + n],"G0Z-1F400") == NULL)
                fputs((char *)(FileData[GC_Paths[index].path_start_index + n]), dp);
        }
        fclose(dp);
    }
}



void append_gcode_reverse(unsigned int index,char * path1)
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

		FileData[GC_Paths[index].path_end_index][1] = '1';     //Change G0 move to G1;
        fputs((char *)(FileData[GC_Paths[index].path_end_index]), dp);
		fputs("G0Z-1F400\n", dp);
		length = (GC_Paths[index].path_end_index - GC_Paths[index].path_start_index);
		printf("\n\rNumber of new gcode lines to write %d",length);
		for (n = 0; n < length; n++)
        {
            if (strstr(FileData[GC_Paths[index].path_end_index - (n+1)],"G0Z-1F400") == NULL)
            {
                if (n == (length-1))
                {
                    FileData[GC_Paths[index].path_end_index - (n+1)][1] = '0'; //Change G1 to G0
                    fputs((char *)(FileData[GC_Paths[index].path_end_index - (n+1)]), dp);
                }else
                {
                    fputs((char *)(FileData[GC_Paths[index].path_end_index - (n+1)]), dp);
                }
            }
        }
        fclose(dp);
    }
}




void append_gcode_join(unsigned int index,char * path1,unsigned char chain_start)
{

    //if chain start is 1, then write gcode index normally, else append it.
    if (chain_start == 1)
    {
        //only write path if it has not been written
        if (GC_Paths[index].path_used == 1)
        {
            //Append path as per normal
            append_gcode(index,path1);
            GC_Paths[index].path_used = 0;
        }
    }else
    {
        if (GC_Paths[index].path_used == 1)
        {
            append_gcode_join_chain(index,path1);
            GC_Paths[index].path_used = 0;
        }
    }
}


int main(int argc, char * argv[])
{
	FILE * fp;
	FILE * dp;
	char tmp[128];
	unsigned int count = 0;
	unsigned int n = 0;
	unsigned int reverse = 0;
	char * pos;
	unsigned int alternate = 0;
	char xEnd[64];
	char yEnd[64];
	unsigned int num_paths = 0;

	unsigned int num_lines = 0;

	unsigned char flagged = 0;


	unsigned int firstLine = 0;
	unsigned int index     = 0;
	unsigned int start_index     = 0;
	double distance,start_x,start_y,end_x,end_y;
	unsigned char chain_start = 0;

	if (argc < 3)
	{
		printf("Not enough arguments!!!:  Call app with:  <input file> <output file>");
		exit(1);
	}


	num_lines = fillArray((const char *)argv[1]);
	num_paths = findPaths(num_lines);
    paths_stil_valid(num_paths);

    start_index = 0;

    for (n = 0; n < (num_paths - 1); n++)
    {
        start_x = atof(GC_Paths[n].x_end);
        start_y = atof(GC_Paths[n].y_end);
        end_x   = atof(GC_Paths[n+1].x_start);
        end_y   = atof(GC_Paths[n+1].y_start);
        distance = sqrt(pow((start_x - end_x),2) + pow((start_y - end_y),2));

        if (distance < atof(argv[3]))     //Join paths less than 2mm apart
        {
            chain_start = 1;
            printf("\n\r Distances: %f",distance);
            printf("\n\rJoining path %d C and %d",n,n+1);
            append_gcode_join(n,argv[2],1);         //start of chain
            append_gcode_join((n + 1),argv[2],0);   //join chain
        }else
        {
            append_gcode_join(n,argv[2],1);         //These are always chain starts
        }

    }
    paths_stil_valid(num_paths);

	return (0);
}




