#include <stdlib.h>
#include <stdio.h>


FILE * ptr;

int main()
{
    ptr = fopen("test.png", "a");
    for (int i = 0; i < 100; i++)
    {
        printf("%c", fgetc(ptr));
    }
    
    return 0;
}