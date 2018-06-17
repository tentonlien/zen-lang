//
// Created by Tenton on 6/26/2018.
//

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../../header/global_value.h"

// INT 10H : Input
void int_10h() {
    union {
        long long _int;
        double _float;
        char _string[100];
    } var;

    switch(ax -> value) {
        case 0x00:  // int
            scanf("%lld", &var._int);
            dx -> value = var._int;
            break;

        case 0x01:  // float
            scanf("%lf", &var._float);
            dx -> value = var._float;
            break;

        case 0x02:  // char
            scanf("%c", &var._int);
            dx -> value = var._int;
            break;

        case 0x03:  // string
            scanf("%s", var._string);
            dx -> value = var._string;
            break;

        default:
            printf("\nError: Invalid argument \"0x%x\" for INT 10H in AX register.\n", ax -> value);
            exit(0);
    }
}

// INT 11H : Output
void int_11h() {
    switch(ax -> value) {
        case 0x00:  // int
            printf("%lld", dx -> value);
            break;

        case 0x01:  // float
            printf("%lf", dx -> value);
            break;

        case 0x02:  // char
            printf("%c", dx -> value);
            break;

        case 0x03:  // string
            printf("%s", (char*)(dx -> value));
            break;

        default:
            printf("\nError: Invalid argument \"0x%x\" for INT 10H in AX register.\n", ax -> value);
            exit(0);
    }

    if (bx -> value == 1) {
        printf(" ");
    } else if (bx -> value == 2) {
        printf("\n");
    }
}