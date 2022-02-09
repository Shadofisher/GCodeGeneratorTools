#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

static FILE * fp,*dp;


typedef struct
{

	char line[20000][128];
	uint32_t len;
	uint32_t valid;
	uint32_t index;
	bool direction;
	float distance;

} gcode_t;


static char tmpstr[128];


void print_data(uint32_t list_len,gcode_t * gc)
{
    uint32_t m,n;

    for (n = 0; n <list_len; n++)
    {
        for (m = 0; m < gc[n].len; m++)
        {
            printf("%s",gc[n].line[m]);
        }
    }
}

uint32_t rangeLow;
uint32_t rangeHigh;

char * outfile;


void calc_distance(uint32_t list_len,gcode_t * gc)
{
	static float distance,distance_f,distance_l = 0.0;
	static float old_distance = 0.0;
	static float x1,y1,x2,y2;
	static char tmp[2];
	//static char tmp1[2];
	static char c,d;
	static uint32_t  n,m,k;
	static uint32_t  closest_index = 0;
	static bool direction;
	static bool old_direction = false;
	static bool complete = false;
	char dist[128];
	uint32_t index_count = 0;

	old_distance = 1000000;

    sscanf(gc[0].line[gc[0].len-1],"%s %c%f %c%f;",tmp,&c,&x1,&d,&y1);  // first comparison
	printf("\n\r----> %f %f",x1,y1);
	gc[0].direction = true;
	gc[0].index = 0;
	gc[0].valid = 1;

	while (complete == false)
    {
        complete = true;
        old_distance = 1000000;
        for (m = 0; m < list_len; m++)
        {
            //if ((gc[m].valid == 0) && (gc[m].len > 50))
            if ((gc[m].valid == 0) && ((gc[m].len > rangeLow) && (gc[m].len <= rangeHigh)))
            {
                complete = false;
                sscanf(gc[m].line[0],"%s %c%f %c%f;",tmp,&c,&x2,&d,&y2);  // first comparison
                //printf("\n\r%s",gc[m].line[0]);
                distance_f = sqrt(pow(x1-x2,2) + (pow(y1-y2,2)));
                printf("\n\rDistance (F) = %f %f %f",distance_f,x2,y2);

                sscanf(gc[m].line[gc[m].len-1],"%s %c%f %c%f;",tmp,&c,&x2,&d,&y2);  // first comparison
                //printf("\n\r%s",gc[m].line[gc[m].len-1]);
                distance_l = sqrt(pow(x1-x2,2) + (pow(y1-y2,2)));
                printf("\n\rDistance (L) = %f %f %f",distance_l,x2,y2);
                if (distance_f == distance_l)
                {
                    direction =true;
                    distance = distance_f;
                }else
                if (distance_f < distance_l)
                {
                    direction = true;
                    distance = distance_f;
                }else
                {
                    direction = false;
                    distance = distance_l;
                }
                if (distance < old_distance)
                {
                    old_distance = distance;
                    closest_index = m;
                    old_direction = direction;
                }

                //printf("\n\rOld Distance: %f %d",old_distance,m);
            }//else
            //printf("\n\rInvalid");

        }

        //printf("\n\rClosest index = %d %d %f",closest_index,direction,old_distance);
        if (direction == true)
        {
            printf("\nDirection is forward");
        }else
        {
            printf("\nDirection is reverse");
        }

        gc[closest_index].valid = 1;
        index_count++;
        gc[closest_index].index = index_count;
        gc[closest_index].direction = old_direction;
        gc[closest_index].distance = old_distance;
        if (gc[closest_index].direction==true)
        {
            sscanf(gc[closest_index].line[gc[closest_index].len-1],"%s %c%f %c%f;",tmp,&c,&x1,&d,&y1);
        }else
        {
            sscanf(gc[closest_index].line[0],"%s %c%f %c%f;",tmp,&c,&x1,&d,&y1);
        }
        printf("\n\r new X,Y %f %f %d",x1,y1,m);

    }

    printf("\n\rDone %d",index_count);

    if((dp = fopen(outfile,"w+")) == NULL)
    {
        perror("fopen");
        exit(1);
    }else
    {
        printf("Writing gcode file.....(list_len: %d)",list_len);

        for (n = 0; n < list_len;n++)
        {
            for (m = 0; m < list_len; m++)
            {
                if ((gc[m].index == n))
                {
                    fputs("G1 Z5\n",dp);
                    printf("\nDirection: %d",gc[m].direction);

                    if (gc[m].direction == 1)
                    {
                        for (k = 0; k < gc[m].len; k++)
                        {
                            if (k == 1)
                            {
                                fputs("G1 Z-2\n",dp);
                            }
                            fputs(gc[m].line[k],dp);
                            //fputs("\n",dp);
                        }
                    }else
                    {
                        for (k = 0; k < gc[m].len; k++)
                        {
                            if (k == 1)
                            {
                                fputs("G1 Z-2\n",dp);
                            }
                            fputs(gc[m].line[(gc[m].len - k)-1],dp);
                            //fputs("\n",dp);
                        }
                    }
                    //sprintf(dist,";Distance: %f\n\r",gc[m].distance);
                    //fputs(dist,dp);
                    //fputs("\n\r;;;;;;;\n\r",dp);

                    break;
                }
            }
        }

        //fputs("G1 Z5\n\r",dp);
        fclose(dp);
    }

}

int main (int argc, char *argv[])
{
	static uint32_t line_count = 0;
	static uint32_t line_start = 0;
	static uint32_t list_count = 0;
	static uint32_t list_start = 0;
	uint32_t n,m;
	//static gcode_t gc_lines[10000];
	gcode_t * gc_lines;

	uint8_t start_new_list;

	char * filename;

	filename = argv[1];

	rangeLow = atoi(argv[2]);
	rangeHigh = atoi(argv[3]);
    outfile = argv[4];
	printf("Filename  = %s %d %d %s", argv[1],rangeLow,rangeHigh,outfile);
    //exit(1);
	gc_lines = malloc(10000 * sizeof(gcode_t));

	float a;
	a = sqrt(8);
	printf("Gcode optimizer %f",a);
	start_new_list = 0;
	#if 0
    if ((fp=fopen(filename,"a")) == NULL)
	{
		perror("fopen:");
		exit(1);
	}else
	{
        fputs("G1 Z5;",fp);
        fputs("\nG1 Z0;",fp);
        fputs("\nG1 Z5;",fp);
        fclose(fp);
    }
    #endif
    //exit(1);
	if ((fp=fopen(filename,"r")) == NULL)
	{
		perror("fopen:");
		exit(1);
	}else
	{
		printf("File open ok\n\r");
		while (fgets(tmpstr,128,fp) != NULL)
		{
			//printf(tmpstr);
			if ((strstr(tmpstr,"G1 Z5") != NULL) || (start_new_list))
			{
				if (line_start == 0)
				{
					line_start = 1;
				}
				else
				{
					//line_start = 0;
					gc_lines[list_count].len = line_count;
					gc_lines[list_count].valid = 0;
					//printf("\n\rLen: %d",gc_lines[list_count].len);
					line_count = 0;
					list_count++;
					start_new_list = 0;
				}
			}else
			{
				if (1)
				{
					if (strstr(tmpstr,"Z") == NULL)
                    {
                        strcpy(gc_lines[list_count].line[line_count],tmpstr);
                        line_count++;
                        if (line_count >= 15000)
                        {
                            printf("---------------ERROR-------------------\n");
                            start_new_list = 1;
                        }
                    }
				}
			}
		}
		fclose(fp);

	}
#if 0
	for (n = 0; n < list_count; n++)
	{
		printf("-------------------------------------%d\n\r",gc_lines[n].len);
		for (m = 0; m < gc_lines[n].len; m++)
			printf("\n\r %d %d %s ",n,gc_lines[n].len,gc_lines[n].line[m]);
	}
#endif

//    for (m = 0; m < list_count; m++)
//    {
//       gc_lines[m].index =  list_count-1;      // initialise all
//       gc_lines[m].valid =  0;      // initialise all

//    }
	calc_distance(list_count,gc_lines);
	//print_data(list_count,gc_lines);
	return (0);
}
