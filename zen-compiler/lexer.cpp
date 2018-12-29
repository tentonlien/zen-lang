// Zen Lexer Class

#include <algorithm>
#include <cstring>
#include <deque>
#include <fstream>
#include <iostream>
#include <vector>
#include "lexer.h"
#include "zenc.h"

std::string keyword[] = {
    "asm", "break", "case", "catch", "class", "continue", "def", "do","else", 
    "extends", "for", "fun", "if", "import", "private", "protected", "public",
    "return", "switch", "this", "try", "trait", "val", "var", "void", "while"
};

std::string types[] = {
    "bool", "char", "byte", "ubyte", "short", "ushort", "int", "uint", "long",
    "ulong", "float", "double"
};

std::string operators[] = {"->", "+", "-", "*", "/", "="};
char puncs[] = {',', ';', '(', ')', '.', ':', '{', '}'};

enum tokenType {
    tok_keyword = 0,
    tok_type = 1,
    tok_identifier = 2,
    tok_punc = 3,
    tok_operator = 4,
    tok_number = 5,
    tok_char = 6,
    tok_string = 7,
};

std::deque<Token> tokens;
std::vector<std::string> files;
unsigned int fileId = -1;

std::vector<std::string> split(const std::string &str,const std::string &pattern) {
    // const char* convert to char*
    char* strc = new char[strlen(str.c_str())+1];
    strcpy(strc, str.c_str());
    std::vector<std::string> resultVec;
    char* tmpStr = strtok(strc, pattern.c_str());
    while (tmpStr != NULL) {
        resultVec.push_back(std::string(tmpStr));
        tmpStr = strtok(NULL, pattern.c_str());
    }
    delete[] strc;
    return resultVec;
}


bool Token::isKeyword() { return checkTokenAttribute(tok_keyword, ""); }
bool Token::isKeyword(std::string value) { return checkTokenAttribute(tok_keyword, value); }
bool Token::isType() { return checkTokenAttribute(tok_type, ""); }
bool Token::isType(std::string value) { return checkTokenAttribute(tok_type, value);}
bool Token::isIdentifier() { return checkTokenAttribute(tok_identifier, ""); }
bool Token::isIdentifier(std::string value) { return checkTokenAttribute(tok_identifier, value); }
bool Token::isPunc() { return checkTokenAttribute(tok_punc, "");}
bool Token::isPunc(std::string value) { return checkTokenAttribute(tok_punc, value); }
bool Token::isOperator() { return checkTokenAttribute(tok_operator, ""); }
bool Token::isOperator(std::string value) { return checkTokenAttribute(tok_operator, value); }
bool Token::isNumber() { return checkTokenAttribute(tok_number, ""); }
bool Token::isNumber(std::string value) { return checkTokenAttribute(tok_number, value); }
bool Token::isChar() { return checkTokenAttribute(tok_char, ""); }
bool Token::isChar(std::string value) { return checkTokenAttribute(tok_char, value); }
bool Token::isString() { return checkTokenAttribute(tok_string, ""); }
bool Token::isString(std::string value) { return checkTokenAttribute(tok_string, value); }


Token::Token(unsigned int type, std::string value, unsigned int fileId, unsigned int lineNumber) {
    this -> type = type;
    this -> value = value;
    this -> fileId = fileId;
    this -> lineNumber = lineNumber;
}


bool Token::checkTokenAttribute(unsigned int type, std::string value) {
    if (value == "") {
        if (this -> type == type) {
            return true;
        }
    } else {
        if (this -> type == type && this -> value == value) {
            return true;
        }
    }
    return false;
}


void printLines() {
    for (auto token: tokens) {
        switch (token.type) {
        case tok_keyword:
            std::cout << "Keyword\t";
            break;
        case tok_type:
            std::cout << "Type\t";
            break;
        case tok_identifier:
            std::cout << "Identifier\t";
            break;
        case tok_punc:
            std::cout << "Punc\t";
            break;
        case tok_operator:
            std::cout << "Operator\t";
            break;
        case tok_number:
            std::cout << "Number\t";
            break;
        case tok_char:
            std::cout << "Char\t";
            break;
        case tok_string:
            std::cout << "String\t";
            break;
        }
        std::cout << token.value << "\t";
        std::cout << files.at(token.fileId) << "\t";
        std::cout << token.lineNumber << std::endl;
    }
}


void Lexer(std::string codes, std::string source) {
    files.push_back(source);
    fileId ++;
    unsigned int lineNumber = 1;
    unsigned int codesCursor = 0;
    //std::vector<Token> tokens;
    unsigned int keywordAmount = sizeof(keyword) / sizeof(*keyword);
    unsigned int typeAmount = sizeof(types) / sizeof(*types);
    unsigned int operatorAmount = sizeof(operators) / sizeof(operators[0]);
    unsigned int puncAmount = sizeof(puncs) / sizeof(puncs[0]);

    while (codesCursor <= codes.length()) {
        std::string temp = "", temp2 = "";
        
        // scan numbers
        if (isdigit(codes[codesCursor])) {
            temp += codes[codesCursor];
            while (isdigit(codes[codesCursor + 1])) {
                temp += codes[codesCursor + 1];
                codesCursor ++;
            }
            Token token(tok_number, temp, fileId, lineNumber);
            tokens.push_back(token);
            codesCursor ++;
        }

        // scan words
        else if (codes[codesCursor] == '_' || isalpha(codes[codesCursor])) {
            Token token;
            temp += codes[codesCursor];
            while(codes[codesCursor + 1] == '_' || isalnum(codes[codesCursor + 1])) {
                temp += codes[codesCursor + 1];
                codesCursor ++;
            }

            // scan keyword
            if (find(keyword, keyword + keywordAmount, temp) - keyword != keywordAmount) {
                token.type = tok_keyword;

                // import imported library code
                if (temp == "import") {
                    int ii = codesCursor;
                    while (codes[ii] != '\n' && codes[ii] != 0) {
                        ii ++;
                    }
                    std::vector<std::string> result = split(codes.substr(codesCursor + 2, ii - codesCursor - 3), ".");
                    std::string relativePath = "";
                    for (unsigned int codesCursor = 0; codesCursor < result.size(); codesCursor ++) {
                        relativePath +=  "/" + result.at(codesCursor);    
                    }
                    extern std::string compilerPath;
                    std::string libraryPath = compilerPath + "/lib" + relativePath + ".zen";

                    // read source code file
                    std::ifstream libFile(libraryPath);

                    if (!libFile) {
                        showError(0, 0, "Library importing Error", "Library \"" + libraryPath + "\" not found");
                    }

                    std::istreambuf_iterator<char> begin(libFile);
                    std::istreambuf_iterator<char> end;
                    std::string libCodes(begin, end);
    
                    libFile.close(); 
                    Lexer(libCodes, libraryPath);

                    codesCursor ++;
                    continue;
                } 
            }
            
            // scan type
            else if (find(types, types + typeAmount, temp) - types != typeAmount) {
                token.type = tok_type;
            }
            
            // scan identifier
            else {
                token.type = tok_identifier;
            }

            token.value = temp;
            token.lineNumber = lineNumber;
            token.fileId = fileId;
            tokens.push_back(token);
            codesCursor ++;
        }

        // scan operators
        else if (find(operators, operators + operatorAmount, std::string(1,
                        codes[codesCursor])) - operators != 
                        operatorAmount && codes[codesCursor + 1] != '/') {
            temp = codes[codesCursor];
            while (find(operators, operators + operatorAmount, std::string(1, 
                        codes[codesCursor + 1])) - operators != operatorAmount) {
                temp += codes[codesCursor + 1];
                codesCursor ++;
            }
            Token token(tok_operator, temp, fileId, lineNumber);
            tokens.push_back(token);
            codesCursor ++;
        }

        // scan punctuations
        else if (std::find(puncs, puncs + puncAmount, codes[codesCursor]) - puncs != puncAmount) {
            temp = codes[codesCursor];
            Token token(tok_punc, temp, fileId, lineNumber);
            tokens.push_back(token);
            codesCursor ++;
        }

        // scan chars and strings
        else if (codes[codesCursor] == '\'' || codes[codesCursor] == '\"') {    
            char stopSign = codes[codesCursor];
            int pos = codesCursor;
            while (codes[codesCursor + 1] != stopSign) {
                temp += codes[codesCursor + 1];
                codesCursor ++;
            }
            Token token(tok_string, temp, fileId, lineNumber);
            
            // detach char from string 
            if (codesCursor - pos == 1) {
                token.type = tok_char;
            }

            tokens.push_back(token);
            codesCursor += 2;
        }

        // scan comments
        else if (codes[codesCursor] == '/' && codes[codesCursor + 1] == '/') {
            while (codes[codesCursor] != '\n') {
                codesCursor ++;
            }
        }

        // seperate each line of codes
        else if (codes[codesCursor] == '\n' || codesCursor == codes.length()) {
            lineNumber ++;
            codesCursor ++;
        }

        else {
            codesCursor ++;
        }
    }

    Token tmp1(tok_keyword, "class", 0, 0);
    Token tmp2(tok_identifier, "main", 0, 0);
    Token tmp3(tok_punc, "{", 0, 0);
    Token tmp4(tok_punc, "}", 0, 0);
    tokens.push_front(tmp3);
    tokens.push_front(tmp2);
    tokens.push_front(tmp1);
    tokens.push_back(tmp4);
}