#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "caliptra_api.h"

int main(int argc, char **argv)
{
    caliptra_init_params params;

    int r = caliptra_init(&params);

    if (r)
    {
        printf("Init was not successful\n");
        return 1;
    }
    
    printf("OK\n");
    return 0;
}
