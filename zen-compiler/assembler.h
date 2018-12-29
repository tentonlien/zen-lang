#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include <vector>

extern void Assembler(std::vector<std::string>);

class AsmToken {
    bool checkTokenAttribute(unsigned int, std::string);

public:
    AsmToken() {}
    AsmToken(unsigned int, std::string, unsigned int);  // type, vlaue, lineNumber
    bool isKeyword();
    bool isKeyword(std::string);
    bool isType();
    bool isType(std::string);
    bool isIdentifier();
    bool isIdentifier(std::string);
    bool isPunc();
    bool isPunc(std::string);
    bool isOperator();
    bool isOperator(std::string);
    bool isNumber();
    bool isNumber(std::string);
    bool isChar();
    bool isChar(std::string);
    bool isString();
    bool isString(std::string);

    unsigned int type;
    std::string value;
    unsigned int lineNumber;
};

extern std::vector<AsmToken> asmTokens;
extern void analyse(std::string, std::string);

#endif