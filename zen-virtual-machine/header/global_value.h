//
// Created by tentonlien on 5/25/18.
//

#ifndef ZENVM_GLOBALVALUE_H
#define ZENVM_GLOBALVALUE_H

struct data_unit {
    long long type;
    unsigned long long value;
};

unsigned short stack_size;

unsigned int* instruction_table;
struct data_unit* stack_table;
struct data_unit* data_table;


// Registers
struct data_unit* ax;
struct data_unit* bx;
struct data_unit* cx;
struct data_unit* dx;
struct data_unit* di;
struct data_unit* si;

unsigned char flag_register;
unsigned short stack_pointer;
unsigned long long base_pointer;
unsigned int instruction_pointer;

#endif //ZENVM_GLOBALVALUE_H