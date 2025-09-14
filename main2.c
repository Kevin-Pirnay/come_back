#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NB_CHARACTERS 100

#define PREVIOUS_CHECKED 5

//******************************************************************************************/

typedef struct
{
    char label;
    int count;
}
Character_Data;

typedef struct
{
    char label;
    Character_Data *distribution_; // composed of count of previous characters
    int count;
}
Character_;

Character_ initialise_character_()
{
    Character_ c = { 0 };

    c.distribution_ = malloc(sizeof(Character_Data) * NB_CHARACTERS);

    return c;
}

void free_Character(Character_ c)
{
    free(c.distribution_);
}

typedef struct
{
    Character_ *characters;
    int count;
}
Characters_Distribution_;

void free_Characters_Distribution_(Characters_Distribution_ *d)
{
    for ( int i = 0; i < d->count; i++ )
    {
        free_Character(d->characters[i]);
    }

    free( d->characters);
}

Characters_Distribution_ initialise_Characters_Distribution_()
{
    Characters_Distribution_ data = { 0 };

    data.characters = malloc(sizeof(Character_) * NB_CHARACTERS);

    return data;
}

char* initialise_previous_()
{
    char *previous_ = malloc(sizeof(char) * NB_CHARACTERS);

    for ( int i = 0; i < NB_CHARACTERS; i++)
    {
        previous_[i] = -1;
    }

    return previous_;
}

void add_character_to_previous(char *previous, int *ptr, const char c)
{
    (*ptr)++;

    if ( *ptr == PREVIOUS_CHECKED ) *ptr = 0;

    previous[*ptr] = c;
}

//loop through dist
//if label already there -> check dist ->> if label already there increment else create and increment
//else create and get last for pre

Characters_Distribution_ extract_data_from_file(FILE *fp)
{
    Characters_Distribution_ dist_ = initialise_Characters_Distribution_();

    char *previous_ = initialise_previous_();
    int ptr_previous = 0;

    char label_found_in_characters_distribution = 0; //character already encounter and taken in account in the overall dist
    char label_found_in_character_distribution = 0; //previous for a specific character

    while (1)
    {
        char c = fgetc(fp);  if ( c == EOF ) break;

        for (int i = 0; i < dist_.count; i++)//pass through the existing entry of the distribution
        {
            if ( dist_.characters[i].label == c )//if the charater already exist in the distribution
            {
                label_found_in_characters_distribution = 1;//set bool to one

                Character_ *character = &dist_.characters[i];//select the existing character

                for (int l = 0; l < PREVIOUS_CHECKED; l++)//pass in review the previous characters
                {
                    char c_in_review = previous_[l];

                    if ( c_in_review == -1 ) continue; //if the entry is not yet initialised, continue -> it will append at the very begnning of the text

                    for ( int j = 0; j < character->count; j++)//pass through the existing entry of character
                    {
                        if ( character->distribution_[j].label == c_in_review )//if the entry already exist for the character
                        {
                            label_found_in_character_distribution = 1;//set bool to one

                            Character_Data *data = &character->distribution_[j];//select the existing entry

                            data->count++;//incrementing count of an existing entry
                        }
                    }

                    //after passing through the existing entry of a charater

                    if ( label_found_in_character_distribution == 0 )//if the entry was not found for the character
                    {
                        Character_Data new_data = { 0 }; // create a new entry

                        new_data.label = c;////creation of a new entry

                        new_data.count++;//creation of a new entry

                        character->distribution_[character->count] = new_data;//adding a new entry at the last index

                        character->count++;//incrementing number of entries to pass through for a the character 
                    }
                }
            }

        }

        //after passing through the existing entry of the distribution

        if ( label_found_in_characters_distribution == 0 ) //if the character doesn't already exist in the distribution
        {
            Character_ new_character = initialise_character_();//create a new entry in for the distribution

            new_character.label = c;

            for ( int k = 0; k < PREVIOUS_CHECKED; k++)//passing in review the previous character
            {
                char prev_c = previous_[k];

                if ( prev_c == -1 ) continue;

                Character_Data n_data = { 0 };//creating the new entry

                n_data.label = prev_c;

                n_data.count++;

                new_character.distribution_[new_character.count] = n_data;//adding a new entry at the last index

                new_character.count++;
            }

            dist_.characters[dist_.count] = new_character;//add the new character in the overall dist

            dist_.count++;
        }

        add_character_to_previous(previous_, &ptr_previous, c);//add c in previous

        //reset bool
        label_found_in_character_distribution = 0;
        label_found_in_characters_distribution = 0;
    }

    //free memory allocated
    free(previous_);

    return dist_;
}

char* initialise_first_letters_()
{
    char *previous = malloc(sizeof(char) * PREVIOUS_CHECKED);

    for ( int i = 0; i < PREVIOUS_CHECKED; i++ )
    {
        previous[i] = ' ';
    }

    return previous;
}

//pass through the distribution
//for each letter get a score according to the previous letter array

typedef struct
{
    int count;
}
Character_score;


char* generate_random_text_(Characters_Distribution_ *data, int size)
{
    char * result_ = malloc(sizeof(char) * size);

    int reader_ptr = 0;

    char *previous_ = initialise_first_letters_();
    int previous_ptr = 0;

    Character_score *scores_ = malloc(sizeof(Character_score) * data->count);

    for (int i = 0; i < data->count; i++)
    {
        Character_ *character = &data->characters[i];//selecter a character

        Character_score score = { 0 };//score each character

        scores_[i] = score;

        for ( int l = 0; l < PREVIOUS_CHECKED; l++ )//pass through previous characters
        {
            for ( int j = 0; i < character->count; j++ )//pass through the distribution of previous character of the current character
            {
                Character_Data *data = &character->distribution_[j];

                if ( data->label == previous_[l] ) score.count += data->count;
            }
        }
    }

    for ( int n = 0; n < size; n++)
    {
        //select the next letter based on the score obtained
        int current_max = 0;

        int win_index = 0;

        for ( int k = 0; k < data->count; k++ )
        {
            if ( scores_[k].count > current_max ) current_max = scores_[k].count;

            win_index = k;
        }

        char next_letter = data->characters[win_index].label;

        result_[reader_ptr] = next_letter;

        reader_ptr++;

        add_character_to_previous(previous_, &previous_ptr, next_letter);
    }

    free(previous_);
    free(scores_);

    return result_;
}

int main()
{
    FILE *fp = fopen("text.txt", "r"); if ( fp == NULL ) { printf("Failed to open file.\n"); return 1; }

    fseek(fp, 0, SEEK_SET);

    Characters_Distribution_ data = extract_data_from_file(fp);

    char *text_ = generate_random_text_(&data, 1000);

    printf("\n%s\n",text_);

    free(text_);

    free_Characters_Distribution_(&data);

    return 0;
}