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

void append_gcode(char * line,char * path1)
{
    unsigned int n;
    unsigned int length;

    char new_line[1024];
    char x_string[64];
    char y_string[64];
    char c;
    if((dp=fopen(path1,"a+"))==NULL)
    {
        perror("\n\rError:");
        exit(1);
    }
    else
    {
        if (line[0] == 'M')
        {
            sscanf(line,"%c %s %s",&c,x_string,y_string);
            //printf("%c %s %s",c,x_string,y_string);
            fputs("F800\n", dp);
            fputs("G0Z3F400\n", dp);
            sprintf(new_line,"G1X%sY%sF800\n",x_string,y_string);
            fputs(new_line,dp);
            fputs("G0Z-1F400\n", dp);
        }else
        if (line[0] == 'L')
        {
            sscanf(line,"%c %s %s",&c,x_string,y_string);
            //printf("%c %s %s",c,x_string,y_string);
            sprintf(new_line,"G0X%sY%sF800\n",x_string,y_string);
            fputs(new_line, dp);

        }else
        {
            printf("\n\rInvalid line");
        }

        //fputs("F800\n", dp);
		//fputs("G0Z3F400\n", dp);
		//length = (GC_Paths[index].path_end_index - GC_Paths[index].path_start_index)+1;
		//printf("\n\rNumber of new gcode lines to write %d",length);
		//for (n = 0; n < length; n++)
       // {
        //    fputs((char *)(FileData[GC_Paths[index].path_start_index + n]), dp);
        //}
        fclose(dp);
    }

}
int main(int argc, char * argv[])
{
	FILE * fp;
	unsigned int count;
	char oneline[1024];

	//fgets((char *)FileData[index], 1024, gcodefile) != 0)

	if (argc < 3)
	{
		printf("Not enough arguments!!!:  Call app with:  <input file> <output file>");
		exit(1);
	}

    if((fp=fopen(argv[1],"r+"))==NULL)
    {
        perror("\n\rError:");
        exit(1);
    }
    else
    {
   		while (count = fgets(oneline, 1024, fp) != 0)
   		{
   		    printf("%s",oneline);
   		    append_gcode(oneline,argv[2]);
   		}
    }
    fclose(fp);
	return (0);
}




