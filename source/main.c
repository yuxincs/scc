#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "list.h"
#include "syntax.h"
#include "scc_yacc.h"
#include "semantic.h"
#include "intercode.h"
#include "targetcode.h"

extern FILE * yyin;
// TODO: This method for showing more details about the file content
// when encountering errors is not very neat and needed to be optimized
char file_name[50];
char *file_content[1024];
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
    clock_t start_time = clock();

    char syntax_file_name[50];
    char ir_file_name[50];

    // read from file
    if(argc > 1)
    {
        strcpy(file_name, argv[1]);
        strcpy(syntax_file_name, argv[1]);
        strcpy(ir_file_name, argv[1]);
        yyin = fopen(argv[1], "r");
        if(yyin == NULL)
        {
            printf("File doesn't exits.\n");
            return 1;
        }
    }

    // parse the options
    bool output_syntax_tree = false;
    bool output_ir_code = false;
    bool is_to_optimize = false;
    FILE * out_file = NULL;
    for(int i = 2; i < argc; ++i)
    {
        if(strcmp(argv[i], "-s") == 0)
            output_syntax_tree = true;
        else if(strcmp(argv[i], "-i") == 0)
            output_ir_code = true;
        else if(strcmp(argv[i], "-opt") == 0)
            is_to_optimize = true;
        else if(strcmp(argv[i], "-o") == 0)
        {
            if(i + 1 >= argc)
            {
                printf("Error! Missing file name after -o\n");
                return 0;
            }
            else
            {
                out_file = fopen(argv[i + 1], "w");
                if(out_file == NULL)
                {
                    printf("Error! Cannot open output file.\n");
                    return 0;
                }
                ++i;
            }
        }
    }
    
    // read file
    read_file();

    // TODO: macro expansion

    // syntax analysis
    if(yyparse() != 0)
        return 0;

    // semantic analysis
    if(semantic_analysis(top_level) == false)
        return 0;

    // generate intermidiate code
    List * code_list = list_new();
    generate_intermediate_code(code_list, top_level);

    // generate target code from intermidiate code
    if(out_file == NULL)
    {
        char * pos = strstr(file_name, ".c");
        strcpy(pos, ".asm");
        out_file = fopen(file_name, "w");
    }
    generate_target_code(out_file, code_list);

    // print syntax tree and ir code
    if(output_syntax_tree)
    {
        char * pos = strstr(syntax_file_name, ".c");
        strcpy(pos, ".syntax");
        FILE * fp = fopen(syntax_file_name, "w");
        print_syntax(fp, top_level);
    }
    if(output_ir_code)
    {
        char * pos = strstr(ir_file_name, ".c");
        strcpy(pos, ".ir");
        FILE * fp = fopen(ir_file_name, "w");
        print_quad_list(fp, code_list);
    }

    clock_t end_time = clock();
    printf("\033[1;32mCompile Succeeded in %g seconds!\033[0m\n", (float)(end_time - start_time) / CLOCKS_PER_SEC);
    return 0;
}

