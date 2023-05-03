#include <stdlib.h>
#include <stdio.h>

FILE * ptr;

int main()
{
    ptr = fopen("test.png", "rb");
    int tab[64] = {0};
    fread(tab, 8, 64, ptr);

    for (int i = 0; i < sizeof(tab); i++)
    {
        printf("%d ", tab[i]);
    }
    
    return 0;
}