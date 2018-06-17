/* ZenVM v0.1
 * Last Modified: 10/5/2018
 * Bytecode Interpreter of Zen Programming Language
 * Zenlang: An interpreted language created at 8:28 PM, 4/26/2018.
 * Copyright Tenton Lien. All Rights Reserved.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../header/global_value.h"
#include "../header/instruction_list.h"
#include "../header/interrupt_list.h"

unsigned int* bytecode_stream;
unsigned long long bytecode_pointer;

void header_handler() {
    // Check magic number
    if ((bytecode_stream[bytecode_pointer]) == 0x214e455a) {
        bytecode_pointer ++;

    } else {
        printf("Error: Not a valid Zen executable file.\n");
        exit(0);
    }

    // Check bytecode version
    if (bytecode_stream[bytecode_pointer] != 0x00000001) {
        printf("Error: Unsupported bytecode version. ZenVM v%.1f required.\n",(float)bytecode_stream[bytecode_pointer]/10);  // Bug!
        exit(0);
    } else {
        bytecode_pointer ++;
    }
}


void data_section_handler() {
    // Allocate memory for stack table
    stack_size = bytecode_stream[bytecode_pointer];
    stack_table = malloc(bytecode_stream[bytecode_pointer] * 8);
    bytecode_pointer ++;

    // Allocate memory for data table
    data_table = malloc((bytecode_stream[bytecode_pointer] & 0x0000FFFF) * 16);
    unsigned short constant_amount = (bytecode_stream[bytecode_pointer] & 0xFFFF0000) >> 16;
    
    // Reference the locations of virtual registers
    ax = data_table;
    bx = data_table + 1;
    cx = data_table + 2;
    dx = data_table + 3;
    di = data_table + 4;
    si = data_table + 5;

    bytecode_pointer ++;

    // Write Constant into data segment
    unsigned long long* constant;
    for (int _i = 0; _i < constant_amount; _i ++) {
        constant = (unsigned long long*)(bytecode_stream + bytecode_pointer);
        data_table[_i + 6].value = *constant;
        bytecode_pointer += 2;
    }
}


void code_section_handler() {
    // Allocate memory for instruction segment
    instruction_table = malloc(bytecode_stream[bytecode_pointer] * sizeof(int));
    int instruction_amount = bytecode_stream[bytecode_pointer];
    bytecode_pointer ++;

    int _i;
    instruction_pointer = 0;

    for (_i = 0; _i < instruction_amount; _i ++) {
        instruction_table[instruction_pointer] = bytecode_stream [bytecode_pointer];
        instruction_pointer ++;
        bytecode_pointer ++;
    }

    instruction_pointer = 0;
}


int execute() {
    unsigned char opcode = (instruction_table[instruction_pointer] & 0xFC000000) >> 26;
    unsigned char addressing = (instruction_table[instruction_pointer] & 0x03000000) >> 24;

    switch (addressing) {
        // immediate only
        case 0:
            dst_operand_value.value = instruction_table[instruction_pointer] & 0x00FFFFFF;
            break;

        // memory only
        case 1:
            dst_operand = instruction_table[instruction_pointer] & 0x00FFFFFF;
            dst_operand_value = data_table[instruction_table[instruction_pointer] & 0x00FFFFFF];
            break;

        // memory-immediate
        case 2:
            dst_operand = (instruction_table[instruction_pointer] & 0x00FFF000) >> 12;
            src_operand_value.value = instruction_table[instruction_pointer] & 0x00000FFF;
            break;

        // memory-memory
        case 3:
            dst_operand = (instruction_table[instruction_pointer] & 0x00FFF000) >> 12;
            src_operand = instruction_table[instruction_pointer] & 0x00000FFF;
            src_operand_value = data_table[instruction_table[instruction_pointer] & 0x00000FFF];
            break;

        default:
            printf("Error: Invalid addressing mode.");
            exit(1);
    }

    switch (opcode) {
        case 0x00: ins_mov(); break;
        case 0x01: ins_xchg(); break;
        case 0x02: ins_push(); break;
        case 0x03: ins_pop(); break;

        case 0x10: ins_add(); break;
        case 0x11: ins_inc(); break;
        case 0x12: ins_sub(); break;
        case 0x13: ins_dec(); break;
        case 0x14: ins_cmp(); break;
        case 0x15: ins_mul(); break;
        case 0x16: ins_div(); break;

        case 0x20: ins_and(); break;
        case 0x21: ins_or(); break;
        case 0x22: ins_not(); break;

        case 0x30: ins_int(); break;
        case 0x31: ins_loop(); break;
        case 0x32: ins_jmp(); break;
        case 0x33: ins_je(); break;
        case 0x34: ins_jne(); break;
        case 0x35: ins_jg(); break;
        case 0x36: ins_jl(); break;
        case 0x37: ins_jge(); break;
        case 0x38: ins_jle(); break;
        
        default:
            printf("Error: Invalid instruction: 0x%x\n", opcode);
            exit(1);
    }
    execute();
    return 0;
}

double storeTime = -1;

int timer_start() {
    struct timespec timeStamp;
    timespec_get(&timeStamp, TIME_UTC);
    storeTime = (long long)timeStamp.tv_sec + (double)timeStamp.tv_nsec / 1000000000;
    return 0;
}

void timer_stop() {
    struct timespec timeStamp;
    timespec_get(&timeStamp, TIME_UTC);
    double currentTime = (long long)timeStamp.tv_sec + (double)timeStamp.tv_nsec / 1000000000;
    printf("\n");
    for(int _count = 0; _count < 40; _count ++) {
        printf("-");
    }
    printf("\nProcess exited after %lf seconds\n", currentTime - storeTime);
}


int main(int argc, char* argv[]) {
    if (argc == 2 && argv[1][0] != '-') {}

    // If retrieving no args
    else if (argc == 1) {
        printf("zvm [-options] *.zef\n");
        return 0;
    }

    // Return version info
     else if (strcmp(argv[1], "-version") == 0) {
        printf("Zen VM v0.0.1\nCopyright 2018 Tenton Lien. All Rights Reserved.\n");
        return 0;
    }

    // Calculate the executing time
    else if (argc == 3 && strcmp(argv[1], "-t") == 0) {
        argv[1] = argv[2];
        timer_start();
    }
    
    else {
        printf("Invalid argument. Type -help for more info.\n");
        return 0;
    }

    FILE *fp;
    long long bytecode_length;

    if ((fp = fopen(argv[1],"rb")) == NULL) {
        printf("Error: Load bytecode file failed...\n");
        return 0;

    } else {
        // Load the bytecode into bytecode_stream
        fseek(fp,0,SEEK_END);
        bytecode_length = ftell(fp);
        bytecode_stream = malloc(bytecode_length);
        fseek(fp,0,SEEK_SET);
        fread(bytecode_stream,sizeof(char),bytecode_length,fp);
        fclose(fp);
    }

    header_handler();  // Check magic number and bytecode version
    data_section_handler();  // Loading data table
    code_section_handler();  // Loading code table
    free(bytecode_stream);
    execute();  // Execute the instructions

    return 0;
}