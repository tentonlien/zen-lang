//
// Created by tentonlien on 10/2/18.
//

#ifndef ZASM_H
#define ZASM_H
#include <string>
#include <vector>

namespace zasm{
    extern std::vector<std::string> lines;
}


void ShowError(int, std::string);
void Initializer(std:: string);

#endif