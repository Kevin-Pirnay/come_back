#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define VOWELS "aeiouy"
#define CONSONANTS "bcdfghjklmnpqrstvwxz"

#define NB_CONSONANTS 20
#define NB_VOWELS 6

#define NB_ALPHA 26
#define NB_CHARACTERS 28

#define C 0
#define V 1
#define W 2
#define O 3

#define NB_TYPES 4
#define NB_TRANS_STATES 16

//******************************************************************************************/

double generate_random_number()
{
    double result = (double) rand() / RAND_MAX;

    return result;
}

char get_type_char(char c)
{
    c = tolower(c);

    for ( char i = 0; i < NB_VOWELS; i++ ) if ( c == VOWELS[i] ) return V;

    for ( char i = 0; i < NB_CONSONANTS; i++ ) if ( c == CONSONANTS[i] ) return C;
        
    if ( isspace(c) ) return W;

    else return O;
}

//******************************************************************************************/

typedef struct
{
    int trans_states[NB_TRANS_STATES];

    int count;

    char states_labels[NB_TRANS_STATES][3];
} 
Markov_Transitions;

void initalise_Markov_transitions_states(Markov_Transitions *m)
{
   const char *labels[NB_TRANS_STATES] = 
   {
        "cc","cv","cw","co",
        "vc","vv","vw","vo",
        "wc","wv","ww","wo",
        "oc","ov","ow","oo"
    };

    for (int i = 0; i < NB_TRANS_STATES; i++) 
    {
        strcpy(m->states_labels[i], labels[i]);
    }
}

void print_markov_transitions(const Markov_Transitions *m, const char mode)
{
    if ( mode == 0 )
    {
        for ( char i = 0; i < NB_TRANS_STATES; i++)
        {
            printf("%s: %d\n", m->states_labels[i], m->trans_states[i]);
        }

        return;
    }

    for ( char i = 0; i < NB_TRANS_STATES; i++)
    {
        printf("%s: %f\n", m->states_labels[i], (double) m->trans_states[i]/m->count);
    }    
}

void set_markov_transitions_count_states(Markov_Transitions *m, const char current_type, char previous_type)
{
    if ( previous_type < 0 || previous_type >= NB_TYPES ) return;

    char index = previous_type * NB_TYPES + current_type;

    m->trans_states[index]++;

    m->count++;
}

//******************************************************************************************/

typedef struct
{
    int chars_count[NB_CHARACTERS];//a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,w,y,z,space,special
    int count;
    char chars_labels[NB_CHARACTERS];
    char i_special;//index
    char i_space;//index
}
Alphabet_Distribution;

void initalise_Alphabet_Distribution(Alphabet_Distribution *a)
{
    char labels[NB_CHARACTERS] = 
    {
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', ' ', '#'
    };

    for (int i = 0; i < NB_CHARACTERS; i++) 
    {
        a->chars_labels[i] = labels[i];
    }

    a->i_space = 26;
    a->i_special = 27;
}

void set_alphabet_distributon(Alphabet_Distribution *ac, char c)
{
    if ( isspace(c) ) ac->chars_count[ac->i_space]++;

    else
    {
        c = tolower(c);

        char index = c - 'a';

        if ( index >= 0 && index < NB_ALPHA ) (*((int *)ac + index))++;

        else ac->chars_count[ac->i_special]++;
    }

    ac->count++;
}

void print_Alphabet_Distribution(const Alphabet_Distribution *ac, const char mode) 
{
    if ( mode == 0 )
    {
        for (int i = 0; i < NB_ALPHA; i++) 
        {
            printf("%c: %d\n", 'a' + i, *((int *)ac + i));
        }
        printf("space %d\n", ac->chars_count[ac->i_space]);
        printf("special %d\n", ac->chars_count[ac->i_special]);

        return;
    }

    for (int i = 0; i < NB_ALPHA; i++) 
    {
        printf("%c: %f\n", 'a' + i, (ac->count > 0) ? ((double) *((int *)ac + i) / ac->count) : 0.0f);
    }
    printf("space %f\n", (double) ac->chars_count[ac->i_space] / ac->count);
    printf("special %f\n", (double) ac->chars_count[ac->i_special] / ac->count);
    
}

int* get_distribution_in_contiguous_space(Alphabet_Distribution *a, const char *lables, int size_of_distribution)
{
    int *result = malloc(sizeof(int) * size_of_distribution);

    char ptr = 0;

    for ( char i = 0; i < NB_ALPHA; i++ )
    {
        if ( a->chars_labels[i] == lables[ptr] ) 
        {
            result[ptr] = a->chars_count[i];

            ptr++;
        }
    }

    return result;
}

//******************************************************************************************/

char set_alphabet_distribution_and_markov_transitions(char c, Markov_Transitions *m, char previous_type_c, Alphabet_Distribution *a)
{
    char type_c = get_type_char(c); 

    set_markov_transitions_count_states(m, type_c, previous_type_c);

    set_alphabet_distributon(a, c);

    return type_c;
}

//******************************************************************************************/

typedef struct 
{
    Markov_Transitions m;

    Alphabet_Distribution a;
}

Text_Data;

Text_Data initialise_text_data()
{
    Text_Data data = { 0 }; 
    
    initalise_Markov_transitions_states(&data.m); 
    
    initalise_Alphabet_Distribution(&data.a);

    return data;
}

Text_Data extract_data_from_file(FILE *fp)
{
    Text_Data data = initialise_text_data();

    char previous_c_type = -1;

    while (1)
    {
       char c = fgetc(fp);  if ( c == EOF ) break;

        char current_c_type = set_alphabet_distribution_and_markov_transitions(c, &data.m, previous_c_type, &data.a);

        previous_c_type = current_c_type;
    }

    return data;
}

Text_Data extract_data_from_buffer(char *fp, char end_char)
{
    Text_Data data = initialise_text_data();

    char previous_c_type = -1;

    int ptr = 0;

    while (1)
    {
        char c = fp[ptr];  if ( c == end_char ) break;

        char current_c_type = set_alphabet_distribution_and_markov_transitions(c, &data.m, previous_c_type, &data.a);

        previous_c_type = current_c_type;

        ptr++;
    }

    return data;
}

//******************************************************************************************/

int get_total_dist(const int *distribution, int nb_variables, int begin_index_of_dist)
{
    int total = 0;

    for ( int i = begin_index_of_dist; i < begin_index_of_dist + nb_variables; i++ )
    {
        total += distribution[i];
    }

    return total;
}

int get_random_variable_from_distributon(const int *distribution, int nb_variables, int begin_index_of_dist)
{
    double random = generate_random_number();

    double cumul_proba = 0.0f;

    int total_dist = get_total_dist(distribution, nb_variables,begin_index_of_dist);

    for (int i = begin_index_of_dist; i < begin_index_of_dist + nb_variables; i++)
    {
        double normalised = (double) distribution[i] / total_dist;

        cumul_proba += normalised;

        if ( cumul_proba >= random ) return i;
    }

    return -1;//possible bug if random close to 1; maybe set the last value as default return
}

char generate_next_type_based_on_the_current_type_and_markov_transitions(const char current_tye, Markov_Transitions *m)
{
    char index = get_random_variable_from_distributon(m->trans_states, NB_TYPES, current_tye * NB_TYPES);

    int next_type = index % NB_TYPES;

    return next_type;
}

char generate_random_letter(Alphabet_Distribution *a)
{
    int *distribution = (int *)a;

    return 'a' + get_random_variable_from_distributon(distribution, NB_ALPHA, 0);
}

//******************************************************************************************/

typedef struct
{
    int *dist;
    int nb_variables;
}
Distribution_;

void free_dstribution(Distribution_ *d)
{
    free(d->dist);
}

Distribution_ get_distribution_of_the_type(const char type, Alphabet_Distribution *a)
{
    Distribution_ d_;

    switch (type)
    {
    case C:
        d_.dist = get_distribution_in_contiguous_space(a, CONSONANTS, NB_CONSONANTS);
        d_.nb_variables = NB_CONSONANTS;
        break;

    case V:
        d_.dist = get_distribution_in_contiguous_space(a, VOWELS, NB_VOWELS);
        d_.nb_variables = NB_VOWELS;
        break;

    case W:
        d_.dist = malloc(sizeof(int));
        *d_.dist = a->chars_count[a->i_space];
        d_.nb_variables = 1;
        break;

    case O:
        d_.dist = malloc(sizeof(int));
        *d_.dist = a->chars_count[a->i_special];
        d_.nb_variables = 1;
        break;
    
    default:
        break;
    }

    return d_;
}

char generate_next_letter_based_on_the_type(const char type, Alphabet_Distribution *a)
{
    Distribution_ d_ = get_distribution_of_the_type(type, a);

    int random_variable = get_random_variable_from_distributon(d_.dist, d_.nb_variables, 0);

    free_dstribution(&d_);

    if (type == O) return '#';

    else if (type == W) return ' ';

    else if (type == C) return CONSONANTS[random_variable];

    else if (type == V) return VOWELS[random_variable];

    else return -1;
}

//******************************************************************************************/

char* generate_random_text_(Text_Data *data, int size)
{
    char *result = malloc(sizeof(char) * size + 1);

    char first_letter = generate_random_letter(&data->a);

    char current_type = get_type_char(first_letter);

    for (int i = 0; i < size -1; i++)
    {
        char next_type = generate_next_type_based_on_the_current_type_and_markov_transitions(current_type, &data->m); 

        char next_letter = generate_next_letter_based_on_the_type(next_type, &data->a);

        current_type = next_type;

        result[i] = next_letter;
    }    

    result[size] = '\0';

    return result;
}


int main()
{
    srand(time(NULL));

    FILE *fp = fopen("text.txt", "r"); if ( fp == NULL ) { printf("Failed to open file.\n"); return 1; }

    fseek(fp, 0, SEEK_SET);
    
    Text_Data data = extract_data_from_file(fp);

    char *result = generate_random_text_(&data, 10000);

    printf("\n%s\n", result);

    free(result);

    return 0;
}



//*********************test//******************************************************************************************/

// char* generate_fake_text_()
// {
//     char *t = malloc(sizeof(char) * (NB_ALPHA + 1));

//     int ptr = 0;

//     for ( char i = 0; i < NB_ALPHA; i++ )
//     {
//         for ( char j = 0; j < i; j++ )
//         {
//             t[ptr] = 'a' + i;

//             ptr++;
//         }
//     }

//     t[ptr] = '\0';

//     return t;
// }

// void set_fake_alphabet_number(Alphabet_Distribution *a, const char *text)
// {
//     int ptr = 0;

//     while (1)
//     {
//         char c = text[ptr];

//         if ( c == '\0' ) break;

//         set_alphabet_distributon(a, c);

//         ptr++;
//     }
// }

// //******************************************************************************************/

// int main()
// {
//     srand(time(NULL));

//     double a = generate_random_number();
//     double b = generate_random_number();
//     double c = generate_random_number();

//     printf("\nrandom numbers generated are : %f, %f, %f and must be different\n", a,b,c);

//     /***************************************************************************************/ 

//     char d[] = "at -";

//     for (char i = 0; i < 4; i++)
//     {
//         char t = get_type_char(d[i]);

//         printf("\nthe type of %c is %d\n", d[i], t);
//     }

//     printf("\nmust be 0 1 2 3\n\n");

//     /***************************************************************************************/

//     Markov_Transitions m = { 0 };

//     initalise_Markov_transitions_states(&m);

//     char types[15] = { 1,1,0,1,2,0,1,2,0,2,1,0,1,1,3 };

//     char previous = -1;

//     for (char i = 0; i < 15; i++)
//     {
//         char current = types[i];

//         set_markov_transitions_count_states(&m, current, previous);

//         previous = current;
//     }

//     print_markov_transitions(&m, 0);

//     printf("\ncc: 0 cv: 3 cw: 1 co: 0 vc: 2 vv: 2 vw: 2 vo: 1 wc: 2 wv: 1 ww: 0 wo: 0 oc: 0 ov: 0 ow: 0 oo: 0 \n");


//     /***************************************************************************************/

//     char *text = generate_fake_text_();

//     Alphabet_Distribution ac = { 0 };

//     set_fake_alphabet_number(&ac, text);

//     int co = get_total_number_of_chars_of_a_type(&ac, NB_CONSONANTS, CONSONANTS);

//     int vo = get_total_number_of_chars_of_a_type(&ac, NB_VOWELS, VOWELS);

//     printf("\nnumber of vowels : %d and number of consonants : %d\n\n", vo,co);

//     free(text);

//     /***************************************************************************************/   

//     int try = 11;

//     char larr[try];

//     for (int i = 0; i < try - 1; i++)
//     {
//         larr[i] = generate_random_letter(&ac);
//     }

//     larr[try - 1] = '\0';

//     printf("`\nrandom letter generated is : %s\n\n", larr);

//     /***************************************************************************************/   

//     char *text1 = generate_fake_text_();

//     Alphabet_Distribution ac1 = { 0 };   

//     Markov_Transitions m1 = { 0 };

//     initalise_Markov_transitions_states(&m1);

//     initalise_Alphabet_Distribution(&ac1);

//     int ptr = 0;

//     char previous_t = -1;

//     while (1)
//     {
//         char c1 = text1[ptr]; if ( c1 == '\0' ) break;

//         char type_c = get_type_char(c1); 

//         char current_type = set_alphabet_distribution_and_markov_transitions(c1, &m1, previous_t, &ac1);

//         previous_t = type_c;

//         ptr++;
//     }

//     print_markov_transitions(&m1,0);

//     print_Alphabet_Distribution(&ac1,0);

//     free(text1);

//     /***************************************************************************************/  

//     char *text2 = generate_fake_text_();

//     Text_Data data = extract_data_from_buffer(text2,'\0');

//     free(text2);

//     print_markov_transitions(&data.m,0);

//     print_Alphabet_Distribution(&data.a,0);

//     /***************************************************************************************/ 

//     int distribution[10] = { 7, 19, 22, 2, 34, 13, 43, 15, 6 , 49};

//     for (int i = 0; i < 10; i++)
//     {
//         char index = get_random_variable_from_distributon(distribution, 10, 0);

//         printf("\nrandom variable selected is : %d\n", distribution[index]);
//     }

//     /***************************************************************************************/ 

//     char *text3 = generate_fake_text_();

//     Text_Data data3 = extract_data_from_buffer(text3,'\0');

//     free(text3);

//     char first_letter = generate_random_letter(&data3.a);

//     char type = get_type_char(first_letter);

//     printf("\nThe current type is : %d and the current letter is : %c\n", type, first_letter);

//     data3.m.trans_states[5] += 200; //to even the distribution for debug purpose
//     data3.m.trans_states[1] += 200; //to even the distribution for debug purpose
//     data3.m.trans_states[4] += 200; //to even the distribution for debug purpose

//     print_markov_transitions(&data3.m,0);

//     for (char i = 0; i < 5; i++)
//     {
//         char next_type = generate_next_type_based_on_the_current_type_and_markov_transitions(type, &data3.m); 

//         printf("The next type is : %d\n", next_type);
//     }

//     /***************************************************************************************/

//     char *text4_ = generate_fake_text_();

//     Text_Data data4 = extract_data_from_buffer(text4_,'\0');

//     free(text4_);

//     char first_letter4 = generate_random_letter(&data4.a);

//     char type4 = get_type_char(first_letter4);

//     char next_type4 = generate_next_type_based_on_the_current_type_and_markov_transitions(type4, &data4.m); 

//     printf("The next type is : %d\n", next_type4);

//     for (char i = 0; i < 5; i++)
//     {
//         char next_letter4 = generate_next_letter_based_on_the_type(next_type4, &data4.a);

//         printf("\nThe next letter is : %c\n", next_letter4);
//     }

//     /***************************************************************************************/

//     char text5[] = "this is a test";

//     Text_Data data5 = extract_data_from_buffer(text5,'\0');

//     char *result5 = generate_random_text_(&data5, 100);

//     printf("\n%s\n", result5);

//     free(result5);

//     return 0;
// }
