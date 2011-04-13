#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    size_t size=1024*1024*1024;
    if (argc>1)
    {
        size_t s = atol(argv[1]);
        if (s>0)
            size = s;
    }

    if (sizeof(long)<sizeof(size_t))
    {
        printf("Тип long меньше типа size_t. Возможны потери точности. Уточняйте документацию про размеры long и size_t, а так же функцию atol().\n");
    }

    char *d = calloc(size, sizeof(char));
    printf("Размер char: %d байт\n", sizeof(char));
    printf("Размер int: %d байт\n", sizeof(int));
    printf("Размер short: %d байт\n", sizeof(short));
    printf("Размер long: %d байт\n", sizeof(long));
    printf("Размер float: %d байт\n", sizeof(float));
    printf("Размер double: %d байт\n", sizeof(double));
    printf("Размер size_t (агрументы malloc и calloc): %d байт\n", sizeof(size_t));
    printf("Размер указателя: %d байт\n", sizeof(d));

    if (d==NULL)
    {
        printf("Не могу выделить память (%.0f байт, %.2f Мб, %.2f Гб)!\n", ((double)size)*sizeof(char), sizeof(char)*((double)size)/(1024.0*1024.0), sizeof(char)*((double)size)/(1024.0*1024.0*1024.0));
        return -1;
    }
    else
    {
        printf("Выделено памяти (%.0f байт, %.2f Мб, %.2f Гб)\n", ((double)size)*sizeof(char), sizeof(char)*((double)size)/(1024.0*1024.0), sizeof(char)*((double)size)/(1024.0*1024.0*1024.0));
        free(d);
    }

    return 0;
}

