/* COP 3502C PA1
This program is written by: Richard Magiday */
/******************************************************************************
Richard Magiday
cop3502c_cmb_26
01/14/26
problem: CS1 PA1

cd PA1
gcc mian.c
Get-Content in1.txt | .\a.exe > out_test1.txt
*******************************************************************************/
// testing github repo link

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXLEN 25

typedef struct Cat
{
    char *name;   // dynamically allocated space for the name without wasting space
    int age;      // specifies the age of the cat
    float weight; // stores the decimal value in weight for this specific cat
    char *breed;  // points to an already allocated breed string (Ragdoll, Siamese, Maincoone, etc..). No malloc/calloc for this property
    int status;   // specifies if a cat is adopted, pending, or available, ranges from 0 to 2, initially set to available for newly added cats
} Cat;

typedef struct Kennel
{
    char *location;  // dynamically allocated space for the location without wasting space
    int occupancy;   // stores the current number of cats in the kennel
    Cat **cats;      // dynamically allocated array of pointers to Cats that reside in the kennel
    int maxCapacity; // specifies the max cap to the kennel
} Kennel;

typedef struct CatStore
{
    int **capacities; // dynamically allocated double int array stores the breeds constraints for all kennels
    int numKenels;    // specifies the total number of kennels this store owns
    Kennel *kennels;  // dynamically allocated array of kennels
} CatStore;

const char STATUS_CAT[3][20] = {"ADOPTED", "PENDING", "AVAILABLE"};

char **readBreeds(int *count);
char *getCharPtrByBreed(char **dictionary, char *breedName, int breedCount);

CatStore *createStore(int kennelCount, int breedCount, char **dictionary);
Kennel *createKennels(int **constraints, int kennelCount, int breedCount, char **dictionary);

Cat **createCats(char **dictionary, int breedCount, int count);
Cat *createSingleCat(char **dictionary, int breedCount);

int canMoveTo(CatStore *s, char *location, char *breed, char **dictionary, int breedCount);
Kennel *getKennelByCat(CatStore *s, Cat *cat);
int getCatPosi(Kennel *home, Cat *cat);
Cat *getCatByName(CatStore *s, char *catName);

void removeCatFromKennel(Kennel *k, Cat *cat);
void runQueries(CatStore *s, char **dictionary, int breedCount, int numQueries);

void freeBreeds(char **dictionary, int breedCount);
void freeStore(int count, CatStore *store);

int main()
{
    char **dictionary; // to store array of dynamically allocated strings for breeds types
    int breedCount;

    dictionary = readBreeds(&breedCount); // store breads

    int kennelCount;
    scanf("%d", &kennelCount); // read kennl

    CatStore *store = createStore(kennelCount, breedCount, dictionary); // create store using

    int q;
    scanf("%d", &q);

    runQueries(store, dictionary, breedCount, q);

    freeStore(kennelCount, store);
    freeBreeds(dictionary, breedCount);

    return 0;
}

char **readBreeds(int *count)
{
    scanf("%d", count); // read nuber of breeds

    char **dictionary = malloc((*count) * sizeof(char *)); // dynamically make space in dictionary with char ptr

    for (int i = 0; i < *count; i++)
    {
        char temp[MAXLEN + 1];                                     // max length 25 + '\0', temp string using make possible length
        scanf("%s", temp);                                         // reads one word
        dictionary[i] = malloc((strlen(temp) + 1) * sizeof(char)); // making malloc of exact size of temp
        strcpy(dictionary[i], temp);                               // copy into exact-sized allocation
    }
    return dictionary;
}

char *getCharPtrByBreed(char **dictionary, char *breedName, int breedCount)
{
    for (int i = 0; i < breedCount; i++)
    {
        if (strcmp(dictionary[i], breedName) == 0) // compares given breedname with dictanary
            return dictionary[i];
    }
    return NULL;
}

CatStore *createStore(int kennelCount, int breedCount, char **dictionary)
{
    CatStore *store = malloc(sizeof(CatStore)); // dynaccly of size struct cat store

    store->numKenels = kennelCount; // stored struct store attributes

    store->capacities = malloc(kennelCount * sizeof(int *));

    for (int i = 0; i < kennelCount; i++)
    {
        store->capacities[i] = malloc(breedCount * sizeof(int));

        for (int j = 0; j < breedCount; j++)
            scanf("%d", &store->capacities[i][j]);
    }

    store->kennels = createKennels(store->capacities, kennelCount, breedCount, dictionary);

    return store;
}

Kennel *createKennels(int **constraints, int kennelCount, int breedCount, char **dictionary)
{
    Kennel *kennels = malloc(kennelCount * sizeof(Kennel)); // dynamc array of size struct kennel

    for (int i = 0; i < kennelCount; i++)
    {
        char tempLoc[MAXLEN + 1]; // temp string again
        int c;
        scanf("%s %d", tempLoc, &c);

        kennels[i].location = malloc((strlen(tempLoc) + 1) * sizeof(char));
        strcpy(kennels[i].location, tempLoc); // same exact string copy as before

        kennels[i].occupancy = c; // nummber of spaces

        int sum = 0;
        for (int j = 0; j < breedCount; j++)
            sum += constraints[i][j];

        kennels[i].maxCapacity = sum;

        kennels[i].cats = createCats(dictionary, breedCount, c);
    }
    return kennels;
}

Cat **createCats(char **dictionary, int breedCount, int count)
{
    Cat **arr = malloc(count * sizeof(Cat *)); // dynamic array of cat struc

    // Fill each pointer with a newly created Cat
    for (int i = 0; i < count; i++)
        arr[i] = createSingleCat(dictionary, breedCount);

    return arr;
}

Cat *createSingleCat(char **dictionary, int breedCount)
{
    Cat *cat = malloc(sizeof(Cat));

    char tempName[MAXLEN + 1]; // max 25 + '\0' temp string
    int tempAge;
    float tempWeight;
    char tempBreed[MAXLEN + 1]; // max 25 + '\0'temp string

    scanf("%s %d %f %s", tempName, &tempAge, &tempWeight, tempBreed);

    cat->name = malloc(strlen(tempName) + 1);
    strcpy(cat->name, tempName); // string copy of exact size again

    cat->age = tempAge;
    cat->weight = tempWeight;

    cat->breed = getCharPtrByBreed(dictionary, tempBreed, breedCount);

    cat->status = 2;

    return cat;
}

int canMoveTo(CatStore *s, char *location, char *breed, char **dictionary, int breedCount)
{
    int dest = 0;
    for (int i = 0; i < s->numKenels; i++)
        if (strcmp(s->kennels[i].location, location) == 0)
            dest = i;

    Kennel *k = &s->kennels[dest];

    if (k->occupancy >= k->maxCapacity) // comparing capacity to see if cat fit
        return 0;

    int breedIndex = 0;
    for (int j = 0; j < breedCount; j++) // looking for cat by bread
        if (dictionary[j] == breed)
            breedIndex = j;

    int currentBreedCount = 0; // keeping track of breads in kennel
    for (int t = 0; t < k->occupancy; t++)
        if (k->cats[t]->breed == breed)
            currentBreedCount++;

    if (currentBreedCount >= s->capacities[dest][breedIndex]) //
        return 0;

    return 1;
}

Kennel *getKennelByCat(CatStore *s, Cat *cat)
{
    for (int i = 0; i < s->numKenels; i++)
    {
        if (getCatPosi(&s->kennels[i], cat) != -1) // check index till kennel is found
            return &s->kennels[i];
    }
    return NULL;
}

int getCatPosi(Kennel *home, Cat *cat)
{
    for (int i = 0; i < home->occupancy; i++) // find cat
    {
        if (home->cats[i] == cat)
            return i;
    }
    return -1;
}

Cat *getCatByName(CatStore *s, char *catName) // search cat by given name
{
    for (int i = 0; i < s->numKenels; i++)
    {
        Kennel *k = &s->kennels[i];

        for (int j = 0; j < k->occupancy; j++)
        {
            if (strcmp(k->cats[j]->name, catName) == 0)
                return k->cats[j];
        }
    }
    return NULL;
}

void removeCatFromKennel(Kennel *k, Cat *cat)
{
    int pos = getCatPosi(k, cat);

    for (int i = pos; i < k->occupancy - 1; i++)
        k->cats[i] = k->cats[i + 1];

    k->occupancy--;

    if (k->occupancy == 0)
    {
        free(k->cats); // feree cat if found
        k->cats = NULL;
    }
    else
    {
        k->cats = realloc(k->cats, k->occupancy * sizeof(Cat *)); // resize
    }
}

void runQueries(CatStore *s, char **dictionary, int breedCount, int numQueries)
{
    for (int qi = 0; qi < numQueries; qi++)
    {
        int type;
        scanf("%d", &type);
        // quiryOne
        if (type == 1)
        {
            char breedName[MAXLEN + 1];
            scanf("%s", breedName);

            char *breedPtr = getCharPtrByBreed(dictionary, breedName, breedCount);
            int printed = 0;

            for (int i = 0; i < s->numKenels; i++)
            {
                Kennel *k = &s->kennels[i];
                for (int j = 0; j < k->occupancy; j++)
                {
                    Cat *c = k->cats[j];
                    if (c->breed == breedPtr)
                    {
                        printf("%s %.2f %d %s %s\n",
                               c->name, c->weight, c->age, k->location, STATUS_CAT[c->status]);
                        printed = 1;
                    }
                }
            }

            if (!printed)
                printf("No cat with breed %s\n", breedName);
        }
        // quiryTwo
        else if (type == 2)
        {
            int newStatus;
            char name[MAXLEN + 1];
            scanf("%d %s", &newStatus, name);

            Cat *cat = getCatByName(s, name);
            cat->status = newStatus;

            printf("%s is now %s!\n", cat->name, STATUS_CAT[newStatus]);

            if (newStatus == 0) // ADOPTED -> remove entirely
            {
                Kennel *home = getKennelByCat(s, cat);
                if (home != NULL)
                    removeCatFromKennel(home, cat);

                /* free the cat itself */
                free(cat->name);
                free(cat);
            }
        }
        // quiryThree
        else if (type == 3)
        {
            char name[MAXLEN + 1];
            char destLoc[MAXLEN + 1];
            scanf("%s %s", name, destLoc);

            Cat *cat = getCatByName(s, name);
            Kennel *src = getKennelByCat(s, cat);

            Kennel *dst = NULL;
            for (int i = 0; i < s->numKenels; i++)
            {
                if (strcmp(s->kennels[i].location, destLoc) == 0)
                    dst = &s->kennels[i];
            }

            if (canMoveTo(s, destLoc, cat->breed, dictionary, breedCount))
            {
                /* remove from source */
                removeCatFromKennel(src, cat);

                /* add to destination: grow cats array by 1 */
                dst->cats = realloc(dst->cats, (dst->occupancy + 1) * sizeof(Cat *));
                dst->cats[dst->occupancy] = cat;
                dst->occupancy++;

                printf("%s moved successfully to %s\n", cat->name, destLoc);
            }
            // sombody broke somthing
            else
            {
                printf("%s cannot take a %s cat!\n", destLoc, cat->breed);
            }
        }
    }
}

void freeBreeds(char **dictionary, int breedCount)
{
    for (int i = 0; i < breedCount; i++)
    {
        free(dictionary[i]); // free each breed string
    }
    free(dictionary); // free the array of pointers
}

void freeStore(int count, CatStore *store)
{
    /* Free all kennels */
    for (int i = 0; i < count; i++)
    {
        Kennel *k = &store->kennels[i];

        /* Free cats in this kennel */
        for (int j = 0; j < k->occupancy; j++)
        {
            Cat *c = k->cats[j];
            free(c->name);
            free(c); // free the Cat struct
        }

        /* Free the cats array */
        free(k->cats);

        /* Free the location string */
        free(k->location);
    }

    /* Free kennel array */
    free(store->kennels);

    /* Free capacities matrix */
    for (int i = 0; i < count; i++)
    {
        free(store->capacities[i]);
    }
    free(store->capacities);

    /* Finally free the store */
    free(store);
}