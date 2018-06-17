//
// Created by Tenton on 5/12/2018.
//

#include <stdio.h>
#include "../../header/instruction_list.h"
#include "../../header/global_value.h"
#include "../../header/interrupt_list.h"

void ins_int() {
    switch (destination_operand) {
        case 0x00: int_01h(); break;
    }
}

/*
int ins_print()
    if(method == 0) {
        switch (operand2) {
            case 0: printf("%lld", ax); break;
            case 1: printf("%lld", bx); break;
            case 2: printf("%lld", cx); break;
            case 3: printf("%lld", dx); break;
        }
    } else {
        if(memory[operand2] == 0x00) {
            printf("%x",memory[operand2 + 1]);
        } else {
            //printf("%x\n",memory[operand2]);
        }
    }
    return 0;
}

int ins_ios() {
    // Standard input
    if(bytecode_data[bytecode_pointer + 1] == 0x00) {
        switch(memory[bytecode_data[bytecode_pointer + 2]]) {
            case 0x80:  // Input byte
                scanf("%c",memory[bytecode_data[bytecode_pointer + 2] + 1]);
                break;
            case 0x81: ;  // Input short
                short* _short = memory + bytecode_data[bytecode_pointer + 2] + 1;
                scanf("%d", _short);
                break;
            case 0x82: ;  // Input integer
                int* _int = memory + bytecode_data[bytecode_pointer + 2] + 1;
                scanf("%d", _int);
                break;
            case 0x83: ;  // Input long
                long long* _long = memory + bytecode_data[bytecode_pointer + 2] + 1;
                scanf("%ld", _long);
                break;
            default:
                return 1;
        }
    }
    // Standard output
    else if(bytecode_data[bytecode_pointer + 1] == 0x80) {
        switch(memory[bytecode_data[bytecode_pointer + 2]]) {
            case 0x00: case 0x80:  // Print byte
                printf("%d",memory[bytecode_data[bytecode_pointer + 2] + 1]);
                break;
            case 0x01: case 0x81: ;  // Print short
                short *_short = memory + bytecode_data[bytecode_pointer + 2] + 1;
                printf("%d",*_short);
                break;
            case 0x02: case 0x82: ;  // Print integer
                int *_int = memory + bytecode_data[bytecode_pointer + 2] + 1;
                printf("%d",*_int);
                break;
            case 0x03: case 0x83: ;  // Print long
                long long *_long = memory + bytecode_data[bytecode_pointer + 2] + 1;
                printf("%lld",*_long);
                break;
            case 0x06: ;  // Print string
                int _count = 1;
                while(memory[bytecode_data[bytecode_pointer + 2] + _count] != 0) {
                    printf("%c",memory[bytecode_data[bytecode_pointer + 2] + _count]);
                    _count ++;
                }
                break;
            default:
                return 1;
        }
    }
    instructionCount ++;
    //bytecode_pointer += 3;
    bytecode_pointer_forward(3);
    return 0;
}

int ins_cjmp() {
    switch(memory[bytecode_data[bytecode_pointer + 1]]) {
        case 0x80:
            if(memory[bytecode_data[bytecode_pointer + 1] + 1] != 0) {
                bytecode_pointer = instructionStart + bytecode_data[bytecode_pointer + 2] * 3;
                return 0;
            }
            break;
        case 0x82: ;
            int* _int = memory + bytecode_data[bytecode_pointer + 1] + 1;
            if(*_int != 0) {
                bytecode_pointer = instructionStart + bytecode_data[bytecode_pointer + 2] * 3;
                return 0;
            }
            break;
    }
    instructionCount ++;
    //bytecode_pointer += 3;
    bytecode_pointer_forward(3);
    return 0;
}
*/