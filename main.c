#include <stdio.h>
#include <stdlib.h>

#include "lexTotoken.h"
#include "parser.h"

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		printf("Incorrect use of args.\n");
		printf("The correct usage is:\n");
		printf("./space <input>.sp\n");
		return 1;
	}
	FILE *fl = fopen(argv[1], "r");
	fseek(fl, 0, SEEK_END);
	long size = ftell(fl);
	rewind(fl);

	char *thestr = malloc(size + 1);
	fread(thestr, 1, size, fl);
	thestr[size] = '\0';
	fclose(fl);

	// Generate the C code!
	char* final_code = tokeToC(tokenizer(thestr), &assembpoiners);

	// Write it to a .c file instead of .asm
	FILE *ftr;
	ftr = fopen("code.c","w");
	fprintf(ftr,"%s", final_code);
	fclose(ftr);

	// Compile the C code using GCC and run it!
	system("gcc code.c -o code");
	system("./code");

	// Optional: Clean up the generated files so the folder stays clean!
	remove("code.c");

	return 0;
}
