// Author: Tenton Lien
// 10/7/2018
// All Syntax Checks are done in the parser

#include <iostream>
#include <string>
#include <map>
#include <utility>
#include "zc.h"
#include "tree.h"
#include "../zen_assembler/zasm.h"

using namespace std;

// Abstract syntax tree
Tree AST;
int pos = 0;
int lineNumber = 0;
int mainFunctionStart, mainFunctionEnd;

struct function {
    string name;
    string returnType;
    int startLine;
    int endLine;
    vector<pair<string, string>> parameterList;

} Function;

struct zclass {
    string name;
    int startLine;
    int endLine;
} zClass;

vector<struct function> Functions;
vector<struct zclass> zClasses;

void parseVariable() {
    string name, type, value;
    name = tokens.at(pos + 1).value;
    if (tokens.at(pos + 2).value == ":") {
        type = tokens.at(pos + 3).value;
        value = tokens.at(pos + 5).value;
        pos += 6;
    } else {
        if (tokens.at(pos + 3).type == "num") {
            type = "i64";
            value = tokens.at(pos + 3).value;
            pos += 4;
        }
    }

    if (pos < tokens.size()) {
        ShowError(lineNumber + 1, "Invalid word \"" + tokens.at(pos).value + "\"");
    }

    int newElementId = AST["data"].size();
    AST["data"].add(newElementId);
    AST["data"][newElementId].add("name", name);
    AST["data"][newElementId].add("type", type);
    AST["data"][newElementId].add("value", value);
}


Tree* parseExpression(int start, int end) {
    int loop = start;
    while (loop + 1 < end) {
        string oprt = tokens.at(loop).value;
        if (oprt == "+" || oprt == "-") {
            Tree* tree = new Tree();
            tree -> add("type", "arithmetic");
            tree -> add("operator", oprt);

            tree -> add("left");
            (*tree)["left"].add("type", tokens.at(loop - 1).type);
            (*tree)["left"].add("value", tokens.at(loop - 1).value);

            tree -> add("right");
            (*tree)["right"].add("type", tokens.at(loop + 1).type);
            (*tree)["right"].add("value", tokens.at(loop + 1).value);

            return tree;
        }
        loop ++;
    }
    return NULL;
}


void parseAssignment() {
    int newElementId = AST["prog"].size();
    AST["prog"].add(newElementId);
    AST["prog"][newElementId].add("type", "assign");
    AST["prog"][newElementId].add("operator", tokens.at(pos + 1).value);

    AST["prog"][newElementId].add("left");
    AST["prog"][newElementId]["left"].add("type", "var");
    AST["prog"][newElementId]["left"].add("value", tokens.at(pos).value);

    AST["prog"][newElementId].add("right");
    if (parseExpression(pos + 2, tokens.size()) != NULL) {
        AST["prog"][newElementId]["right"].add(*parseExpression(pos + 2, tokens.size()));
    } else if (tokens.at(pos + 2).type == "num") {
        AST["prog"][newElementId]["right"].add("type", "num");
        AST["prog"][newElementId]["right"].add("value", tokens.at(pos + 2).value);
    }
}


void parseCall(bool isMethod) {
    int newElementId = AST["prog"].size();
    AST["prog"].add(newElementId);
    AST["prog"][newElementId].add("type", "call");
    if (isMethod) {
        AST["prog"][newElementId].add("class", tokens.at(pos).value);
        AST["prog"][newElementId].add("value", tokens.at(pos + 2).value);
    } else {
        AST["prog"][newElementId].add("class", "");
        AST["prog"][newElementId].add("func", tokens.at(pos).value);
    }
    
    
}


/*
void parseModuleFunction() {
    int newElementId = AST["prog"].size();
    if (tokens.at(pos).value == "Console") {
        AST["prog"].add(newElementId);
        AST["prog"][newElementId].add("type", "i_call");
        AST["prog"][newElementId].add("module", tokens.at(pos).value);
        AST["prog"][newElementId].add("function", tokens.at(pos+ 2).value);
        AST["prog"][newElementId].add("args");
        if (tokens.at(pos + 3).value != "()") {
            AST["prog"][newElementId]["args"].add("type", tokens.at(pos + 4).type);
            AST["prog"][newElementId]["args"].add("value", tokens.at(pos + 4).value);
        }
        
    }
}
*/

void parseClasses() {
    for (int i = 0; i < lines.size(); i ++) {
        tokens = lines.at(i);
        int c = 0;  // Cursor
        if (!(tokens.size() > c && tokens.at(c).type == "keyword" && tokens.at(c).value == "val")) continue;
        if (tokens.size() <= ++ c || tokens.at(c).type != "word") {
            ShowError(i + 1, "Invalid value statement");
        }
        if (!(tokens.size() > ++ c && tokens.at(c).type == "operator" && tokens.at(c).value == "=")) continue;
        if (!(tokens.size() > ++ c && tokens.at(c).type == "keyword" && tokens.at(c).value == "class")) continue;
        
        if (!(tokens.size() > ++ c && tokens.at(c).type == "operator" && tokens.at(c).value == "->")) continue;
                            
        if (tokens.size() > ++c && tokens.at(c).type == "punc" && tokens.at(c).value == "{") {
            zClass.name = tokens.at(1).value;
            zClass.startLine = i;
            int counter = 1;
            int cursor = 7;
            int j = i;
            bool loop = true;
            while (j < lines.size() && loop) {
                vector<struct tok> tokens2 = lines.at(j);
                for (int k = cursor; k < tokens2.size(); k ++) {
                    if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "{") {
                        counter ++;
                    } else if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "}") {
                        counter --;
                    }
                    if (counter == 0) {
                        zClass.endLine = j;
                        loop = false;
                        break;
                    }
                }
                cursor = 0;
                j ++;
            }
                                    
            zClasses.push_back(zClass);
        }    
    }        
}

void parseFunctions() {
    for (int i = 0; i < lines.size(); i ++) {
        tokens = lines.at(i);
        int c = 0;  // Cursor
        if (!(tokens.size() > c && tokens.at(c).type == "keyword" && tokens.at(c).value == "val")) continue;
        if (tokens.size() <= ++ c || tokens.at(c).type != "word") {
            ShowError(i + 1, "Invalid value statement");
        }
        if (!(tokens.size() > ++ c && tokens.at(c).type == "operator" && tokens.at(c).value == "=")) continue;
        if (!(tokens.size() > ++ c && tokens.at(c).type == "punc" && tokens.at(c).value == "(")) continue;
        // TO-DO Parameter List
        if (!(tokens.size() > ++ c && tokens.at(c).type == "punc" && tokens.at(c).value == ")")) continue;
        if (tokens.size() > ++ c && tokens.at(c).type == "punc" && tokens.at(c).value == ":") {
            Function.returnType = tokens.at(c + 1).value;
            c += 2;
        } else {
            Function.returnType = "";
        }
        if (!(tokens.size() > c && tokens.at(c).type == "operator" && tokens.at(c).value == "->")) continue;
                            
        if (tokens.size() > ++c && tokens.at(c).type == "punc" && tokens.at(c).value == "{") {
            Function.name = tokens.at(1).value;
            Function.startLine = i;
            int counter = 1;
            int cursor = 7;
            int j = i;
            bool loop = true;
            while (j < lines.size() && loop) {
                vector<struct tok> tokens2 = lines.at(j);
                for (int k = cursor; k < tokens2.size(); k ++) {
                    if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "{") {
                        counter ++;
                    } else if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "}") {
                        counter --;
                    }
                    if (counter == 0) {
                        Function.endLine = j;
                        loop = false;
                        break;
                    }
                }
                cursor = 0;
                j ++;
            }
                                    
            Functions.push_back(Function);
        }    
    }        
}


void parseLine(int start, int end) {
    for (lineNumber = start; lineNumber <= end; lineNumber ++) {
        tokens = lines.at(lineNumber);
        pos = 0;

        while(pos < tokens.size()) {
            // Parse variables
            if (tokens.at(pos).type == "keyword" && tokens.at(pos).value == "var") {
                parseVariable();
            }

            // Parse Assembly Instructions
            else if (tokens.at(pos).type == "word" && tokens.at(pos).value == "__ASM__") {
                // Check syntax
                if (pos + 1 >= tokens.size() || !(tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "(")) {
                    ShowError(lineNumber + 1, "Missing symbol \"(\"");
                }
                if (pos + 3 >= tokens.size() || !(tokens.at(pos + 3).type == "punc" && tokens.at(pos + 3).value == ")")) {
                    ShowError(lineNumber + 1, "Missing symbol \")\"");
                }
                if (tokens.at(pos + 2).type != "string") {
                    ShowError(lineNumber + 1, "Parameter's data type error. String required.");
                }
                if (pos + 4 < tokens.size()) {
                    string unidentified = "";
                    for (int i = pos + 4; i < tokens.size(); i ++) {
                        unidentified += "\"" + tokens.at(i).value + "\" ";
                    }
                    ShowError(lineNumber + 1, "Unidentified symbol " + unidentified + "at the end of this line.");
                }

                int newElementId = AST["prog"].size();
                AST["prog"].add(newElementId);
                AST["prog"][newElementId].add("type", "asm");
                AST["prog"][newElementId].add("value", tokens.at(pos + 2).value);
                break;
            }

            else if (tokens.at(pos).type == "word" && pos + 2 < tokens.size()) {

                // Parse Method
                if (pos + 3 < tokens.size() &&
                    tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "." &&
                    tokens.at(pos + 2).type == "word" &&
                    tokens.at(pos + 3).type == "punc" && tokens.at(pos + 3).value == "("
                ) {
                    if (!(tokens.at(tokens.size() - 1).type == "punc" && tokens.at(tokens.size() - 1).value == ")")) {
                        ShowError(lineNumber + 1, "Missing \")\"");
                    }
                    parseCall(true);
                    break;
                }

                // Parse Function
                if (tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "(") {
                    if (!(tokens.at(tokens.size() - 1).type == "punc" && tokens.at(tokens.size() - 1).value == ")")) {
                        ShowError(lineNumber + 1, "Missing \")\"");
                    }
                    parseCall(false);
                    break;
                }


                // Parse assginment
                else if (tokens.at(pos).type == "word" && pos + 2 < tokens.size()  && tokens.at(pos + 1).value == "=" && tokens.at(pos + 2).value != "(") {
                    //cout << tokens.at(pos).value << endl;
                    parseAssignment();
                    pos ++;
                }
                
                else {
                    pos ++;
                }
            }

            else {
                pos ++;
            }    
        }
    }
}


void Parser() {
    // Initial abstract syntax tree
    AST.add("data");
    AST.add("prog");
    parseClasses();
    parseFunctions();
    cout << "Function Manager" << endl;
    for (int i = 0; i < Functions.size(); i ++) {
        cout << Functions.at(i).name << " " << Functions.at(i).startLine << " " << Functions.at(i).endLine << " " << Functions.at(i).returnType << endl;
        if (Functions.at(i).name == "main") {
            parseLine(Functions.at(i).startLine, Functions.at(i).endLine);
            break;
        }
        if (i == Functions.size() - 1) {
            ShowError(0, "Function \"main\" required as entry of the program");
        }
    }

    cout << "Class Manager" << endl;
    for (int i = 0; i < zClasses.size(); i ++) {
        cout << zClasses.at(i).name << " " << zClasses.at(i).startLine << " " << zClasses.at(i).endLine << endl;
    }

    
    
    AST.preOrder();
}