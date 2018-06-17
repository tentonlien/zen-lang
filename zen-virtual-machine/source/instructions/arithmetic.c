//
// Created by Tenton on 5/12/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../../header/instruction_list.h"
#include "../../header/global_value.h"


void ins_add() {
    data_table[dst_operand].value += src_operand_value.value;
    instruction_pointer ++;
}


void ins_inc() {
    data_table[dst_operand].value ++;
    instruction_pointer ++;
}


void ins_sub() {
    data_table[dst_operand].value -= src_operand_value.value;
    instruction_pointer ++;
}


void ins_dec() {
    data_table[dst_operand].value --;
    instruction_pointer ++;
}


void ins_cmp() {
    long long _long;
    _long = data_table[dst_operand].value - src_operand_value.value;

    unsigned char sf = flag_register & 0x80;  // & 1000 0000
    unsigned char zf = flag_register & 0x40;  // & 0100 0000
    
    if (_long == 0) {
        zf = 1;
    } else if (_long < 0) {
        sf = 1;
    }

    flag_register = (sf << 7) + (zf << 6);
    instruction_pointer ++;
}


void ins_mul() {
    data_table[dst_operand].value *= src_operand_value.value;
    instruction_pointer ++;
}


void ins_div() {
    data_table[dst_operand].value /= src_operand_value.value;
    instruction_pointer ++;
}


void ins_sqrt() {
    // TODO
    instruction_pointer ++;
}