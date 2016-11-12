#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "syntax.h"
#include "scc_yacc.h"
#include "semantic.h"

extern FILE * yyin;
// TODO: This method for showing more details about the file content
// when encountering errors is not very neat and needed to be optimized
char *file_content[1024];
char file_name[1024];
extern int yyparse();

Syntax * top_level;

void read_file()
{
    for(int i = 0; !feof(yyin); i++)
    {
        file_content[i] = (char * )malloc(1024 * sizeof(char));
        fgets(file_content[i], 1024, yyin);
        file_content[i][strlen(file_content[i]) - 1] = '\0';
    }
    fseek(yyin, 0, 0);
}

int main(int argc, char ** argv)
{
    // read from file
    if(argc > 1)
    {
        strcpy(file_name, argv[1]);
        yyin = fopen(argv[1], "r");
        if(yyin == NULL)
        {
            printf("File doesn't exits.\n");
            return 1;
        }
    }
    // read file
    read_file();

    // TODO: macro expansion

    // syntax analysis
    if(yyparse() != 0)
        return 0;

    //print_syntax(top_level);

    // semantic analysis
    if(semantic_analysis(top_level) == false)
        return 0;

    // TODO: generate intermidiate code

    // TODO: generate target code from intermidiate code
    
    printf("\033[1;32mCompile Success!\033[0m\n");
    return 0;
}

