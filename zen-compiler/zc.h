//
// Created by tentonlien on 10/2/18.
//

#ifndef ZC_H
#define ZC_H
#include <string>
#include <vector>

using namespace std;

// Token stream
struct tok {
    std::string type;
    std::string value;
    std::string file;
    int line;
};
extern struct tok token;
extern std::vector<struct tok> tokens;
extern std::vector<std::vector<struct tok>> lines;

struct zen_function {
    string name;
    string returnType;
    int startLine;
    int endLine;
    vector<pair<string, string>> parameterList;

};
extern struct zen_function Function;
extern vector<struct zen_function> Functions;

struct zen_method {
    string accessModifier;
    string name;
    string returnType;
    int startLine;
    int endLine;
    vector<pair<string, string>> parameterList;
};
extern struct zen_method Method;

struct zen_class {
    string name;
    int startLine;
    int endLine;
    vector<struct zen_method> methods;
    string file;
};
extern struct zen_class Class;
extern vector<struct zen_class> Classes;

void Lexer(std::string, std::string);
void Parser();
void Generator();

#endif