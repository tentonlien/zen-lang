//
// Created by Tenton on 6/23/2018.
//

#ifndef ZEN_VIRTUAL_MACHINE_INTERRUPT_LIST_H
#define ZEN_VIRTUAL_MACHINE_INTERRUPT_LIST_H

// System
void int_00h();  // Quit the program
void int_02h();  // System time

// IO
void int_10h();  // Input
void int_11h();  // Output

#endif //ZEN_VIRTUAL_MACHINE_INTERRUPT_LIST_H
