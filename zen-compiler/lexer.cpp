#include <algorithm>
#include <cstring>
#include <fstream>
#include <string>
#include "zc.h"
#include "zasm/zasm.h"

using namespace std;

struct tok token;
vector<struct tok> tokens;
vector<vector<struct tok>> lines;

string keyword[] = {
    "asm", "break", "case", "catch", "class", "continue", "def", "do",
    "else", "extends", "for", "fun", "if", "import", "private", 
    "protected", "public", "return", "switch", "this", "try", 
    "trait", "val", "var", "void", "while"
};

string types[] = {
    "bool", "char", "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64", "f32", "f64"
};

string operators[] = {">", "+", "-", "*", "/", "="};
char puncs[] = {',', ';', '(', ')', '.', ':', '{', '}'};
string variables[] = {};

string charToString(char ch) {
    string str;
    str.push_back(ch);
    return str;
}


vector<string> split(const string &str,const string &pattern)
{
    //const char* convert to char*
    char * strc = new char[strlen(str.c_str())+1];
    strcpy(strc, str.c_str());
    vector<string> resultVec;
    char* tmpStr = strtok(strc, pattern.c_str());
    while (tmpStr != NULL) {
        resultVec.push_back(string(tmpStr));
        tmpStr = strtok(NULL, pattern.c_str());
    }
    delete[] strc;
    return resultVec;
}


void Lexer(string codes, string fileName) {
    unsigned int i = 0, lineNumber = 1;
    tokens.clear();
    while (i <= codes.length()) {
        string temp = "", temp2 = "";
        // Scan numbers
        if (isdigit(codes[i])) {
            temp += codes[i];
            while (isdigit(codes[i + 1])) {
                temp += codes[i + 1];
                i ++;
            }
            token.type = "num";
            token.value = temp;
            token.line = lineNumber;
            token.file = fileName;
            tokens.push_back(token);
            i ++;
        }

        // Scan words
        else if (codes[i] == '_' || isalpha(codes[i])) {
            temp += codes[i];
            while(codes[i + 1] == '_' || isalnum(codes[i + 1])) {
                temp += codes[i + 1];
                i ++;
            }
            if (find(keyword, keyword + sizeof(keyword) / sizeof(*keyword), temp) - keyword != sizeof(keyword) / sizeof(*keyword)) {
                token.type = "keyword";
                if (temp == "import") {
                    int ii = i;
                    while (codes[ii] != '\n' && codes[ii] != 0) {
                        ii ++;
                    }
                    vector<string> result = split(codes.substr(i + 2, ii - i - 3), ".");
                    string relativePath = "";
                    for (unsigned int i = 0; i < result.size(); i ++) {
                        relativePath +=  "/" + result.at(i);    
                    }
                    extern string compilerPath;
                    string libraryPath = compilerPath + "/lib" + relativePath + ".zen";

                    // Read source code file
                    std::ifstream libFile(libraryPath);

                    if (!libFile) {
                        ShowError(0, "Library \"" + libraryPath + "\" not found");
                    }

                    istreambuf_iterator<char> begin(libFile);
                    istreambuf_iterator<char> end;
                    string libCodes(begin, end);
    
                    libFile.close(); 
                    Lexer(libCodes, libraryPath);

                    i ++;
                    continue;
                } 
                
                else if (temp == "val") {
                    token.value = "val";
                    i ++;
                }
                
                else if (temp == "var") {
                    token.value = "var";
                    i ++;
                }
            } else if (find(types, 
                types + sizeof(types) / sizeof(*types), temp) - types != sizeof(types) / sizeof(*types)) {
                token.type = "type";
            } else if (find(operators, operators + sizeof(operators) / sizeof(*operators), temp) - operators != sizeof(operators) / sizeof(*operators)) {
                token.type = "operator";
            } else {
                token.type = "word";
            }
            token.value = temp;
            token.line = lineNumber;
            token.file = fileName;
            tokens.push_back(token);
            i ++;
        }

        // Scan operators
        else if (find(operators, operators + sizeof(operators) / sizeof(operators[0]), charToString(codes[i])) - operators != sizeof(operators) / sizeof(*operators) && codes[i + 1] != '/') {
            temp = codes[i];
            while (find(operators, operators + sizeof(operators) / sizeof(operators[0]), charToString(codes[i + 1])) - operators != sizeof(operators) / sizeof(*operators)) {
                temp += codes[i + 1];
                i ++;
            }
            token.type = "operator";
            token.value = temp;
            token.line = lineNumber;
            token.file = fileName;
            tokens.push_back(token);
            i ++;
        }

        // Scan punctuations
        else if (find(puncs, puncs + sizeof(puncs) / sizeof(puncs[0]), codes[i]) - puncs != sizeof(puncs) / sizeof(*puncs)) {
            temp = codes[i];
            //while ( i + 1 < codes.length() && find(puncs, puncs + sizeof(puncs) / sizeof(puncs[0]), codes[i + 1]) - puncs != sizeof(puncs) / sizeof(*puncs)) {
            //    temp += codes[i + 1];
            //    i ++;
            //}
            token.type = "punc";
            token.value = temp;
            token.line = lineNumber;
            token.file = fileName;
            tokens.push_back(token);
            i ++;
        }

        // Scan chars and strings
        else if (codes[i] == '\'' || codes[i] == '\"') {
            char stopSign = codes[i];
            int pos = i;
            while (codes[i + 1] != stopSign) {
                temp += codes[i + 1];
                i ++;
            }
            if (i - pos == 1) {
                token.type = "char";
            } else {
                token.type = "string";
            }
            token.value = temp;
            token.line = lineNumber;
            token.file = fileName;
            tokens.push_back(token);
            i += 2;
        }

        // Scan comments
        else if (codes[i] == '/' && codes[i + 1] == '/') {
            while (codes[i] != '\n') {
                i ++;
            }
            //token.type = "null";
            //tokens.push_back(token);
        }

        // Seperate each line of codes
        else if (codes[i] == '\n' || i == codes.length()) {
            if (codes[i - 1] != ',') {
                lines.push_back(tokens);
                vector<struct tok>().swap(tokens);
                lineNumber ++;
            }
            i ++;
        }

        else {
            i ++;
        }
    }
}