#include <stdlib.h>
#include <stdio.h>


FILE * ptr;

int main()
{
    ptr = fopen("test.png", "rb");
    char tab[100] = {0};
    fread(tab, 1, 100, ptr);

    for (int i = 0; i < sizeof(tab); i++)
    {
        printf("%c", tab[i]);
    }
    
    
    return 0;
}