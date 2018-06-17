//
// Created by Tenton on 5/12/2018.
//

#include <stdio.h>
#include "../../header/instruction_list.h"
#include "../../header/global_value.h"
#include "../../header/interrupt_list.h"

// All the control instructions only allow immediate as dst_operand



unsigned char sf_value() {
    return flag_register & 0x80;  // & 1000 0000
}

unsigned char zf_value() {
    return flag_register & 0x40;  // & 0100 0000
}

void ins_int() {
    switch (dst_operand_value.value) {
        case 0x00: int_00h(); break;
        case 0x02: int_02h(); break;
        case 0x10: int_10h(); break;
        case 0x11: int_11h(); break;
    }
    instruction_pointer ++;
}


void ins_loop() {
    data_table[2].value --;
    if (data_table[2].value > 0) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}


void ins_jmp() {
    instruction_pointer = dst_operand_value.value;
}

void ins_je() {
    if (zf_value() == 1) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}

void ins_jne() {
    if (zf_value() == 0) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}

void ins_jg() {
    if (sf_value() == 0) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}

void ins_jl() {
    if (sf_value() == 1) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}

void ins_jge() {
    if (zf_value() == 1 || sf_value() == 0) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}

void ins_jle() {
    if (zf_value() == 1 || sf_value() == 1) {
        instruction_pointer = dst_operand_value.value;
    } else {
        instruction_pointer ++;
    }
}