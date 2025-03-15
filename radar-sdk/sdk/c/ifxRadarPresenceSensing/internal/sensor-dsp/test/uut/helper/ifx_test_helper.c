#include <stdio.h>
#include <stdlib.h>
#include "ifx_test_helper.h"

/*******************************************************************************
 * Helper functions for data read and write
 ****************************************************************************//**
 */
uint32_t count_lines(const char* filename)
{
    FILE* fptr;
    uint32_t lines = 0;
    if ((fptr = fopen(filename, "r")) != NULL)
    {
        char ch;
        lines = 1;
        while (!feof(fptr))
        {
            ch = fgetc(fptr);
            if (ch == '\n')
            {
                lines++;
            }
        }
        fclose(fptr);
    }
    return (lines-1);
}


uint32_t read_float32_from_file(const char* filename, float32_t* array, uint32_t length)
{
    FILE* fptr;
    uint32_t values = 0;
    if ((fptr = fopen(filename, "r")) != NULL)
    {
        for (values = 0; values < length; values++)
        {
            if (feof(fptr))
            {
                break;
            }
            fscanf(fptr, "%f\n", array + values);
        }
        fclose(fptr);
    }
    return values;
}


void write_float32_to_file(const char* filename, float32_t* array, uint32_t length, uint8_t append)
{
    FILE* fptr;
    if (!append)
    {
        fptr = fopen(filename, "w");
    }
    else
    {
        fptr = fopen(filename, "a");
    }

    for (uint32_t count = 0; count < length; count++)
    {
        fprintf(fptr, "%f\n", array[count]);
    }
    fclose(fptr);
}


void write_uint32_to_file(const char* filename, uint32_t* array, uint32_t length, uint8_t append)
{
    FILE* fptr;
    if (!append)
    {
        fptr = fopen(filename, "w");
    }
    else
    {
        fptr = fopen(filename, "a");
    }

    for (uint32_t count = 0; count < length; count++)
    {
        fprintf(fptr, "%d\n", array[count]);
    }
    fclose(fptr);
}
