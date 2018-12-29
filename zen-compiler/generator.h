#ifndef GENERATOR_H
#define GENERATOR_H

#include <string>
#include <vector>
#include "parser.h"

std::vector<std::string> Generator(std::shared_ptr<ClassExpr>);

#endif