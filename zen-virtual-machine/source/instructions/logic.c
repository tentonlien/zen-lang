//
// Created by Tenton on 7/8/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include "../../header/instruction_list.h"
#include "../../header/global_value.h"

void ins_and() {
    data_table[dst_operand].value = data_table[dst_operand].value & src_operand_value.value;
    instruction_pointer ++;
}


void ins_or() {
    data_table[dst_operand].value = data_table[dst_operand].value | src_operand_value.value;
    instruction_pointer ++;
}


void ins_not() {
    data_table[dst_operand].value = !data_table[dst_operand].value;
    instruction_pointer ++;
}