#include <stdio.h>
#include <locale.h>

struct str
{
    int x;
    int y;
} defaultStruct = {3, 6};

typedef struct str Data;

void change(int arr[5])
{
    arr[2] = 500;
}

void main()
{
    //setlocale(LC_ALL,"");
    char text[] = "Привет мир!";
    int i = 0;
    for (; i < 13; i++)
        printf("%d\n", (int)text[i]);
}
