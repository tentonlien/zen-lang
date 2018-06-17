//
// Created by tentonlien on 10/2/18.
//

#ifndef ZC_H
#define ZC_H
#include <string>
#include <vector>

// Token stream
struct tok {
    std::string type;
    std::string value;
    int lineNumber = 0;
};
extern struct tok token;
extern std::vector<struct tok> tokens;
extern std::vector<std::vector<struct tok>> lines;

void Lexer(std::string);
void Parser();
void Generator();

#endif