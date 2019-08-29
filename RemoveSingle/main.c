// DeleteSingle.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(int argc,char * argv[])
{
	FILE * fp;
	FILE * dp;

	char lineone[256];
	char linetwo[256];
	char linethree[256];
	char linefour[256];
	char linefive[256];

	unsigned int set_line1 = 0;
	unsigned int count = 0;

	if (argc < 3)
	{
		printf("Not enough arguments!!!:  Call app with:  <input file> <output file>");
		exit(1);
	}


	//if ((fopen_s(&fp, argv[1], "r+")) != 0)
    if((fp=fopen((const char *)argv[1],"r+"))==NULL)
	{
		perror("\n\rError1: ");
		exit(1);
	}
	else
	{
		//if ((fopen_s(&dp, argv[2], "w+")) != 0)
        if((dp=fopen((const char *)argv[2],"w+"))==NULL)
		{
			perror("\n\rError: ");
			fclose(fp);
			exit(1);
		}
		else
		{
			while (fgets(lineone, 256, fp))
			{
				count = sizeof(lineone);
				if (strcmp("G0Z3F400\n", lineone) == 0)
				{
					if (fgets(linetwo, 256, fp) == 0)
					{
						fputs(lineone, dp);
						break;
					}
					if (fgets(linethree, 256, fp) == 0)
					{
						fputs(linetwo, dp);
						break;
					}
					if (fgets(linefour, 256, fp) == 0)
					{
						fputs(linethree, dp);
						break;
					}
					if (fgets(linefive, 256, fp) == 0)
					{
						fputs(linefour, dp);
						break;
					}
					if (strcmp(linefive, lineone) == 0)
					{
						//fputs(linefive, dp);
						//
						fseek(fp, -10, SEEK_CUR);
					}
					else
					{
						fputs(lineone, dp);
						fputs(linetwo, dp);
						fputs(linethree, dp);
						fputs(linefour, dp);
						fputs(linefive, dp);
					}
				}
				else
				{
					fputs(lineone, dp);
				}
			}
		}
	}


	fclose(fp);
	fclose(dp);
	return 0;
}
