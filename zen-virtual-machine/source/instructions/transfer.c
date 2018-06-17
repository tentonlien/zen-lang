//
// Created by Tenton on 6/26/2018.
//

#include <stdio.h>
#include "../../header/instruction_list.h"
#include "../../header/global_value.h"
#include "../../header/interrupt_list.h"

void ins_mov() {
    data_table[dst_operand] = src_operand_value;
    instruction_pointer ++;
}


void ins_xchg() {
    struct data_unit swap;
    swap = data_table[dst_operand];
    data_table[dst_operand] = data_table[src_operand];
    data_table[src_operand] = swap;
    instruction_pointer ++;
}


void ins_push() {
    if (stack_pointer >= stack_size) {
        printf("Error: Stack overflow.\n");
        exit(0);
    }
    stack_table[stack_pointer] = dst_operand_value;
    stack_pointer ++;
    instruction_pointer ++;
}


void ins_pop() {
    if (stack_pointer <= 0) {
        printf("Error: Stack underflow.");
        exit(0);
    }

    data_table[dst_operand] = stack_table[stack_pointer - 1];
    stack_pointer --;
    instruction_pointer ++;
}