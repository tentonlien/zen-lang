//
// Created by Tenton on 6/23/2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../../header/global_value.h"

extern double timer_stop();
extern double storeTime;

// INT 00H
void int_00h() {
    //free(stack_table); 
    //free(data_table);
    //free(instruction_table);

    // Output time cost information
    if (storeTime >= 0) {
        timer_stop();
    }
    
    exit(0); 
}

// INT 02H: Date and time
void int_02h() {
    time_t current_time = time(NULL);
    struct tm *tm = localtime(&current_time);
    // char* str = asctime(tm);
    // dx -> value = str;

    switch (ax -> value) {
        case 0x00: dx -> value = asctime(tm); break;
        case 0x01: dx -> value = tm -> tm_sec; break;
        case 0x02: dx -> value = tm -> tm_min; break;
        case 0x03: dx -> value = tm -> tm_hour; break;
        case 0x04: dx -> value = tm -> tm_mday; break;
        case 0x05: dx -> value = tm -> tm_mon + 1; break;
        case 0x06: dx -> value = tm -> tm_year + 1900; break;
        case 0x07: dx -> value = tm -> tm_wday; break;
        case 0x08: dx -> value = tm -> tm_isdst; break;
        case 0x09: dx -> value = tm -> tm_gmtoff; break;
        case 0x0A: dx -> value = tm -> tm_zone; break;
        default:
            printf("Error: Invalid argument for INT 02H in AX register.\n");
    }
}