#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "syntax.h"
#include "semantic.h"
#include "intercode.h"
#include "targetcode.h"


// TODO: This method for showing more details about the file content
// when encountering errors is not very neat and needed to be optimized
char file_name[50];
char *file_content[1024];

void read_file(FILE * source_file)
{
    for(int i = 0; !feof(source_file); i++)
    {
        file_content[i] = (char * )malloc(1024 * sizeof(char));
        fgets(file_content[i], 1024, source_file);
        file_content[i][strlen(file_content[i]) - 1] = '\0';
    }
    fseek(source_file, 0, 0);
}

int main(int argc, char ** argv)
{
    clock_t start_time = clock();

    FILE * source_file = NULL;
    char syntax_file_name[100];
    char ir_file_name[100];

    // read from file
    if(argc > 1)
    {
        strncpy(file_name, argv[1], sizeof(file_name));
        strncpy(syntax_file_name, argv[1], sizeof(syntax_file_name));
        strncpy(ir_file_name, argv[1], sizeof(ir_file_name));
        source_file = fopen(argv[1], "r");
        if(source_file == NULL)
        {
            printf("File doesn't exits.\n");
            return 1;
        }
    }
    else
    {
        printf("Usage: scc <input> [Options]\n\n");
        printf("Options:\n");
        printf("    %-20s Output intermediate code\n", "-i");
        printf("    %-20s Output syntax tree\n", "-s");
        printf("    %-20s Output assembly code\n", "-o <file>");
        return 1;
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
            if(i + 1 >= argc || argv[i + 1][0] == '-')
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
    read_file(source_file);

    // TODO: macro expansion

    // syntax analysis
    Syntax * top_level = NULL;
    top_level = syntax_analysis(source_file);
    if(top_level == NULL)
        return 0;

    if(output_syntax_tree)
    {
        char * pos = strstr(syntax_file_name, ".c");
        strncpy(pos, ".syntax", sizeof(".syntax"));
        FILE * fp = fopen(syntax_file_name, "w");
        print_syntax(fp, top_level);
    }

    // semantic analysis
    if(semantic_analysis(top_level) == false)
        return 0;

    // generate intermidiate code
    List * code_list = list_new();
    generate_intermediate_code(code_list, top_level);

    if(output_ir_code)
    {
        char * pos = strstr(ir_file_name, ".c");
        strncpy(pos, ".ir", sizeof(".ir"));
        FILE * fp = fopen(ir_file_name, "w");
        print_quad_list(fp, code_list);
    }

    // generate target code from intermidiate code
    if(out_file == NULL)
    {
        char * pos = strstr(file_name, ".c");
        strncpy(pos, ".asm", ".asm");
        out_file = fopen(file_name, "w");
    }
    generate_target_code(out_file, code_list);

    clock_t end_time = clock();
    printf("\033[1;32mCompile Succeeded in %g seconds!\033[0m\n", (float)(end_time - start_time) / CLOCKS_PER_SEC);
    return 0;
}

