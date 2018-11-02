#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>
#include "zc.h"
#include "zasm/zasm.h"

using namespace std;

struct tok token;
vector<struct tok> tokens;
vector<vector<struct tok>> lines;

string keyword[] = {"char", "else", "if", "i32","val", "var", "while", "class"};
string types[] = {
    "function", "class", "method", "byte", "char", "double", "float", 
    "int", "long", "short", "string",
    "i32"
};
string operators[] = {">", "+", "-", "*", "/", "="};
char puncs[] = {',', ';', '(', ')', '.', ':', '{', '}'};
//string libraries[] = {"zen", "zenx"};
//string modules[] = {"Console"};
//string functions[] = {"read", "readln", "writeln"};
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
    while (tmpStr != NULL)
    {
        resultVec.push_back(string(tmpStr));
        tmpStr = strtok(NULL, pattern.c_str());
    }

    delete[] strc;

    return resultVec;
}


void Lexer(string codes) {
    int i = 0;
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
            tokens.push_back(token);
            i ++;
        }

        // Scan words
        else if (codes[i] =='_' || isalpha(codes[i])) {
            temp += codes[i];
            while(codes[i + 1] == '_' || isalnum(codes[i + 1])) {
                temp += codes[i + 1];
                i ++;
            }
            if (find(keyword, keyword + sizeof(keyword) / sizeof(*keyword), temp) - keyword != sizeof(keyword) / sizeof(*keyword)) {
                token.type = "keyword";
            } else if (find(types, 
                types + sizeof(types) / sizeof(*types), temp) - types != sizeof(types) / sizeof(*types)) {
                token.type = "type";
            } else if (find(operators, operators + sizeof(operators) / sizeof(*operators), temp) - operators != sizeof(operators) / sizeof(*operators)) {
                token.type = "operator";
            } 
            else if (temp == "import") {
                //token.type = "import";
                int ii = i;
                while (codes[ii] != '\n' && codes[ii] != 0) {
                    ii ++;
                }
                cout << "TEST " << codes.substr(i + 2, ii - i - 3) << endl;
                vector<string> result = split(codes.substr(i + 2, ii - i - 3), ".");
                string relativePath = "";
                for (int i = 0; i < result.size(); i ++) {
                    relativePath +=  "/" + result.at(i);
                    
                }
                extern string compilerPath;
                string libraryPath = compilerPath + "/lib" + relativePath + ".zen";
                cout << libraryPath << endl;

                // Read source code file
                std::ifstream libFile(libraryPath);

                if (!libFile) {
                    ShowError(0, "Library \"" + libraryPath + "\" not found");
                }

                istreambuf_iterator<char> begin(libFile);
                istreambuf_iterator<char> end;
                string libCodes(begin, end);
    
                libFile.close(); 
                Lexer(libCodes);

                i ++;
                continue;
            } 
            else if (temp == "val") {
                i ++;
                temp = "";
                while (isalpha(codes[i + 1])) {
                    temp += codes[i + 1];
                    i ++;
                }
                token.type = "val";
                i ++;
            } else if (temp == "var") {
                temp= "";
                i ++;
                while (isalnum(codes[i + 1])) {
                    temp += codes[i + 1];
                    i ++;
                }
                token.type = "var";
                i ++;
            } else {
                token.type = "word";
            }
            token.value = temp;
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
            }
            i ++;
        }

        else {
            i ++;
        }
    }
}