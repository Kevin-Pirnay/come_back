#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VOWELS "aeiouy"
#define CONSONANTS "bcdfghjklmnpqrstvwxz"

#define NB_ALPHA 26
#define NB_CHARACTERS 28

//******************************************************************************************/

typedef struct
{
    char label;
    int count;
}
Characters_Distribution;

typedef struct
{
    char label;
    Characters_Distribution *distribution;
}
Character;

typedef struct
{
    Character *dist
}
Distribution;

Distribution extract_data_from_file(FILE *fp)
{
    Distribution data = { 0 };

    while (1)
    {
        char c = fgetc(fp);  if ( c == EOF ) break;


    }

    return data;
}