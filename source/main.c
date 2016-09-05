#include <stdio.h>
#include "y.tab.h"

int main(int argc, char ** argv)
{
	if(argc > 1)
	{
		yyin = fopen(argv[1], "r");
		if(yyin == NULL)
		{
			printf("File doesn't exits.\n");
			return 1;
		}
		strcpy(file_name, argv[1]);
		
		if(argc > 2)
		{
			if(strcmp(argv[2], "-print-tree") == 0)
				is_to_print = true;
			
		}
	}	
	read_file();
	yyparse();
	if(!has_error)
		printf("\033[1;32mCompile Success!\033[0m\n");
    return 0;
}

