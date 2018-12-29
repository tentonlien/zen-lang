#ifndef LEXER_H
#define LEXER_H

#include <deque>
#include <string>
#include <vector>

class Token {
    bool checkTokenAttribute(unsigned int, std::string);

public:
    Token() {}
    Token(unsigned int, std::string, unsigned int, unsigned int);  // type, vlaue, file, lineNumber
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
    unsigned int fileId;
    unsigned int lineNumber;
};

extern std::deque<Token> tokens;
extern std::vector<std::string> files;
extern void Lexer(std::string, std::string);

#endif