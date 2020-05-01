#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gol.h"

int main(int argc, char *argv[])
{

    int (*rule)(struct universe *, int, int) = will_be_alive;
    int stats = 0;
    char input_name[128];
    char output_name[128];
    FILE *input;
    FILE *output;
    int gens = 5;
    int in = 0;
    int o = 0;
    int g = 0;

    for (int i = 1; i < argc; i++)
    {
        // printf("%s\n", argv[i]);
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {
            case 'i':
                // check if input has already been defined
                if (in) {
                    // new input same as last one
                    if (!strcmp(input_name, argv[++i]))
                    {
                        // do nothing
                    }
                    // new input different
                    else
                    {
                        fprintf(stderr, "ERROR: Conflicting parameters.\n");
                        exit(1);
                    }
                }
                else
                {
                    in = 1;
                    strcat(input_name, argv[++i]);
                }
                break;
            case 'o':
                // check if output has already been defined
                if (o)
                {
                    // new output same as last one
                    if (!strcmp(output_name, argv[++i]))
                    {
                        // do nothing
                    }
                    // new output different
                    else
                    {
                        fprintf(stderr, "ERROR: Conflicting parameters.\n");
                        exit(1);
                    }
                }
                else
                {
                    o = 1;
                    strcat(output_name, argv[++i]);
                }
                break;
            case 'g':

                // check if gens has already been defined
                if (g)
                {
                    // new gens same as last one
                    if (gens == atoi(argv[++i]))
                    {
                        // do nothing
                    }
                    // new gens different
                    else
                    {
                        fprintf(stderr, "ERROR: Conflicting parameters.\n");
                        exit(1);
                    }
                }
                else
                {
                    g = 1;
                    gens = atoi(argv[++i]);
                }
                break;
            case 's':
                stats = 1;
                break;
            case 't':
                rule = will_be_alive_torus;
                break;
            default:
                fprintf(stderr, "ERROR: Invalid parameters.\n");
                exit(1);
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Invalid parameters.\n");
            exit(1);
        }
    }

    input = fopen(input_name, "r");

    if (input == NULL)
    {
        input = stdin;
    }

    output = fopen(output_name, "w+");

    if (output == NULL)
    {
        output = stdout;
    }

    struct universe v;
    read_in_file(input, &v);
    for (int i = 0; i < gens; i++)
    {
        evolve(&v, rule);
    }
    write_out_file(output, &v);

    if (stats)
    {
        print_statistics(&v);
    }
    return 0;
}
