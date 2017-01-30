#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char * argv[])
{
	FILE *f = fopen(argv[1], "r");
	//char *a = (char *)malloc(sizeof(char));
	int b;
	while (!feof(f))
	{
		int MAXLEN = 4;
		int len = 0;
		char *a = (char *)malloc(MAXLEN*sizeof(char));
		while (1)
		{
			//char *a = (char *)malloc(sizeof(char));
			fread(&b, 1, 1, f);
			if (b == 9 || (b >=32 && b <= 126))
			{
				if (len == MAXLEN)
				{
					MAXLEN*=2;
					a = (char *)realloc(a, MAXLEN*sizeof(char));
				}
				sprintf(a+len, "%c", (char)b);
				len++;
			}
			else if (len >= 4)
			{
				printf("%s\n", a);
				free(a);
				a = NULL;
				break;
			}
			else
			{
				free(a);
				a = NULL;
				break;
			}
		}
		//printf("%c", (char)b);
		//strcat(a, b);
	}
	//printf("%s", a);
	return 0;
}
