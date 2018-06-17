//
// Created by tentonlien on 5/25/18.
//

#ifndef ZENVM_EXECUTEMODULE_H
#define ZENVM_EXECUTEMODULE_H

/*
struct instruction {
    unsigned int opcode;
    unsigned int addressing;
    unsigned long long dst_operand;
    unsigned long long src_operand;
};
*/
//unsigned char opcode;
//unsigned char addressing;
unsigned long long dst_operand;
struct data_unit dst_operand_value;
unsigned long long src_operand;
struct data_unit src_operand_value;


// Transfer Instructions
void ins_mov();
void ins_xchg();
void ins_push();
void ins_pop();

// Arithmetic Instructions
void ins_add();
void ins_inc();
void ins_sub();
void ins_dec();
void ins_cmp();
void ins_mul();
void ins_div();
void ins_sqrt();

// Logic Instructions
void ins_and();
void ins_or();
void ins_not();
void ins_xor();
void ins_shl();
void ins_shr();
void ins_sal();
void ins_sar();
void ins_rol();
void ins_ror();

// Control Instructions
void ins_int();
void ins_loop();
void ins_jmp();
void ins_je();
void ins_jne();
void ins_jg();
void ins_jl();
void ins_jge();
void ins_jle();

#endif //ZENVM_EXECUTEMODULE_H