#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "gol.h"

void read_in_file(FILE *infile, struct universe *u)
{
    char symbol;
    int columns = 0;
    int rows = 0;
    int rowCount = 0;

    if (infile != NULL)
    {
        // work out how many rows and columns
        do
        {
            symbol = fgetc(infile);

            // verify that the symbol is valid
            if (symbol != '*' && symbol != '.' && symbol != '\n' && symbol != EOF)
            {
                fprintf(stderr, "ERROR: Input file is invalid.\n");
                exit(1);
            }
            if (rows == 0 && symbol != '\n')
            {
                columns++;
            }
            if (symbol == '\n' || feof(infile))
            {
                rows++;
            }
        }
        while (symbol != EOF);

        if (rows == 1)
        {
            columns--;
        }

        if (columns > 512)
        {
            fprintf(stderr, "ERROR: Input file has more than 512 columns.\n");
            exit(1);
        }

        u->width = columns;
        u->height = rows;

        if (ferror(infile))
        {
            fprintf(stderr, "ERROR: Error while reading from file.\n");
            exit(1);
        }

        rewind(infile);

        // check that the input is a rectangular grid
        do
        {
            symbol = fgetc(infile);
            if (symbol != '\n')
            {
                rowCount++;
            }
            else if (symbol == '\n')
            {
                if (rowCount == columns)
                {
                    rowCount = 0;
                }
                else
                {
                    fprintf(stderr, "ERROR: Input file is invalid.\n");
                    exit(1);
                }
            }
        } while (symbol != EOF);

        // go back to the beginning
        rewind(infile);

        u->grid = malloc(rows * sizeof(char *));

        if (u->grid == NULL)
        {
            fprintf(stderr, "ERROR: malloc() failed.\n");
            exit(1);
        }

        for (int i = 0; i < rows; i++)
        {
            u->grid[i] = malloc(columns * sizeof(char *));
        }

        for (int i = 0; i < rows; i++)
        {
            fscanf(infile, "%s\n", u->grid[i]);
        }

        fclose(infile);

        u->cumulative_alive = 0;
        u->cumulative_total = 0;
    }

    else
    {
        fprintf(stderr, "ERROR: Error while opening the input file.\n");
        exit(1);
    }
}

void write_out_file(FILE *outfile, struct universe *u)
{
    if (outfile != NULL)
    {
        // printf("%d", u->height);
        for (int i = 0; i < u->height; i++)
        {
            // printf("%s", u->grid[i]);
            fputs(u->grid[i], outfile);
            fputs("\n", outfile);
        }
        fclose(outfile);
    }
    else
    {
        fprintf(stderr, "ERROR: Error while writing the output file.\n");
        exit(1);
    }
}

int is_alive(struct universe *u, int column, int row)
{
    return (u->grid[row][column] == '*') ? 1 : 0;
}

int will_be_alive(struct universe *u, int column, int row)
{
    int sum = 0;
    for (int i = row-1; i <= row+1; i++)
        for (int j = column-1; j <= column+1; j++)
            if (i < 0 || j < 0 || i >= u->height-1 || j >= u->width-1)
                sum += 0;
            else
                sum += (u->grid[i][j] == '*');

    sum -= (u->grid[row][column] == '*');

    if (u->grid[row][column] == '*')
        return (sum == 2 || sum == 3);
    else
        return sum == 3;
}

int will_be_alive_torus(struct universe *u, int column, int row)
{
    int sum = 0;
    for (int i = row - 1; i <= row + 1; i++)
    {
        for (int j = column - 1; j <= column + 1; j++)
        {
            if (u->grid[(i+u->height) % u->height][(j+u->width) % u->width] == '*')
            {
                sum += 1;
            }
        }
    }
    if (u->grid[row][column] == '*')
    {
        sum -= 1;
    }

    if (u->grid[row][column] == '*')
        return (sum == 2 || sum == 3);
    else
        return sum == 3;
}

void evolve(struct universe *u, int (*rule)(struct universe *u, int column, int row))
{
    // count number of alive cells before evolving
    int current_alive = 0;
    for (int i = 0; i < u->height; i++)
        for (int j = 0; j < u->width; j++)
            if (u->grid[i][j] == '*')
                current_alive += 1;

    u->cumulative_alive += current_alive;
    u->cumulative_total += (int)((u->height)*(u->width));

    char new_grid[u->height][u->width];

    for (int i = 0; i < u->height; i++)
        for (int j = 0; j < u->width; j++)
            rule(u, j, i) ? (new_grid[i][j] = '*') : (new_grid[i][j] = '.');
            
    for (int i = 0; i < u->height; i++)
    {
        for (int j = 0; j < u->width; j++)
        {
            u->grid[i][j] = new_grid[i][j];
        }
    }
}

void print_statistics(struct universe *u)
{
    int current_alive = 0;
    for (int i = 0; i < u->height; i++)
        for (int j = 0; j < u->width; j++)
            if (u->grid[i][j] == '*')
                current_alive += 1;

    u->cumulative_alive += current_alive;
    u->cumulative_total += (int)((u->height) * (u->width));

    int total_cells = (u->height) * (u->width);
    float current_percent = 100.0 * current_alive / total_cells;
    float average_percent = 100.0 * (float)u->cumulative_alive / u->cumulative_total;

    printf("%.3f%% of cells currently alive\n", current_percent);
    printf("%.3f%% of cells alive on average\n", average_percent);
}
