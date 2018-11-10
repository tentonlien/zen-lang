// Author: Tenton Lien
// 10/7/2018
// All Syntax Checks are done in the parser

#include <string>
#include <map>
#include <cstdio>
#include <utility>
#include "zc.h"
#include "tree.h"
#include "zasm/zasm.h"

using namespace std;

// Abstract syntax tree
Tree AST;
unsigned int pos = 0;
unsigned int lineNumber = 0;
unsigned int mainFunctionStart, mainFunctionEnd;

struct zen_function Function;
vector<struct zen_function> Functions;
struct zen_method Method;
struct zen_class Class;
vector<struct zen_class> Classes;


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


void parseAssignment(string funcName) {
    int newElementId = AST["prog"][funcName].size();
    AST["prog"][funcName].add(newElementId);
    AST["prog"][funcName][newElementId].add("type", "assign");
    AST["prog"][funcName][newElementId].add("operator", tokens.at(pos + 1).value);

    AST["prog"][funcName][newElementId].add("left");
    AST["prog"][funcName][newElementId]["left"].add("type", "var");
    AST["prog"][funcName][newElementId]["left"].add("value", tokens.at(pos).value);

    AST["prog"][funcName][newElementId].add("right");
    if (parseExpression(pos + 2, tokens.size()) != NULL) {
        AST["prog"][funcName][newElementId]["right"].add(*parseExpression(pos + 2, tokens.size()));
    } else if (tokens.at(pos + 2).type == "num") {
        AST["prog"][funcName][newElementId]["right"].add("type", "num");
        AST["prog"][funcName][newElementId]["right"].add("value", tokens.at(pos + 2).value);
    }
}


void parseCall(string funcName, bool isMethod) {
    int newElementId = AST["prog"][funcName].size();
    AST["prog"][funcName].add(newElementId);
    AST["prog"][funcName][newElementId].add("type", "call");
    if (isMethod) {
        AST["prog"][funcName][newElementId].add("class", tokens.at(pos).value);
        AST["prog"][funcName][newElementId].add("value", tokens.at(pos + 2).value);
        if (tokens.at(pos + 3).value != "()") {
            AST["prog"][funcName][newElementId].add("args");
            AST["prog"][funcName][newElementId]["args"].add("type", tokens.at(pos + 4).type);
            AST["prog"][funcName][newElementId]["args"].add("value", tokens.at(pos + 4).value);
        }
    } else {
        AST["prog"][funcName][newElementId].add("class", "");
        AST["prog"][funcName][newElementId].add("value", tokens.at(pos).value);
    }
    
    
}


void parseFunction() {
    for (unsigned int i = 0; i < lines.size(); i ++) {
        tokens = lines.at(i);
        unsigned int c = 0;  // Cursor
        if (!(tokens.size() > c && tokens.at(c).type == "keyword" && tokens.at(c).value == "fun")) continue;
        if (tokens.size() <= ++ c || tokens.at(c).type != "word") {
            ShowError(i + 1, "Invalid value statement");
        }

        if (tokens.size() > ++ c && tokens.at(c).type == "punc" && tokens.at(c).value == "(") {
            while(tokens.at(c).type != "punc" || tokens.at(c).value != ")") {
                c ++;
                // TO-DO Parameter List
            }
            c ++;
        }
        
        if (tokens.size() > c && tokens.at(c).type == "punc" && tokens.at(c).value == ":") {
            Function.returnType = tokens.at(c + 1).value;
            c += 2;
        } else {
            Function.returnType = "";
        }

        if (tokens.size() > c && tokens.at(c).type == "operator" && tokens.at(c).value == "->") {
            Function.startLine = Function.endLine = i;
        }
                            
        else if (tokens.size() > c && tokens.at(c).type == "punc" && tokens.at(c).value == "{") {
            Function.startLine = i;
            unsigned int counter = 1;
            unsigned int cursor = c + 1;
            unsigned int j = i;
            bool loop = true;
            while (j < lines.size() && loop) {
                vector<struct tok> tokens2 = lines.at(j);
                for (unsigned int k = cursor; k < tokens2.size(); k ++) {
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
        }

        else if (tokens.size() > c) {
            //////
        }

        Function.name = tokens.at(1).value;
        Functions.push_back(Function);
        AST["prog"]["fun"].add(Function.name);
        AST["prog"]["fun"][Function.name].add("startLine", to_string(Function.startLine));
        AST["prog"]["fun"][Function.name].add("endLine", to_string(Function.endLine));
        AST["prog"]["fun"][Function.name].add("returnType", Function.returnType);
        continue;
    }    
}


void parseClass() {
        for (unsigned int i = 0; i < lines.size(); i ++) {
        tokens = lines.at(i);
        unsigned int c = 0;  // Cursor
        if (!(tokens.size() > c && tokens.at(c).type == "keyword" && tokens.at(c).value == "class")) continue;
        if (tokens.size() <= ++ c || tokens.at(c).type != "word") {
            ShowError(i + 1, "Invalid value statement");
        }

        if (tokens.size() > ++ c && tokens.at(c).type == "punc" && tokens.at(c).value == "(") {
            while(tokens.at(c).type != "punc" || tokens.at(c).value != ")") {
                c ++;
                // TO-DO Parameter List
            }
            c ++;
        }
        
        if (tokens.size() > c && tokens.at(c).type == "punc" && tokens.at(c).value == ":") {
            Function.returnType = tokens.at(c + 1).value;
            c += 2;
        } else {
            Function.returnType = "";
        }

        if (tokens.size() > c && tokens.at(c).type == "operator" && tokens.at(c).value == "->") {
            Function.startLine = Function.endLine = i;
        }
                            
        else if (tokens.size() > c && tokens.at(c).type == "punc" && tokens.at(c).value == "{") {
            Class.startLine = i;
            unsigned int counter = 1;
            unsigned int cursor = c + 1;
            unsigned int j = i;
            bool loop = true;
            while (j < lines.size() && loop) {
                vector<struct tok> tokens2 = lines.at(j);
                for (unsigned int k = cursor; k < tokens2.size(); k ++) {
                    if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "{") {
                        counter ++;
                    } else if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "}") {
                        counter --;
                    }
                    if (counter == 0) {
                        Class.endLine = j;
                        loop = false;
                        break;
                    }
                }
                cursor = 0;
                j ++;
            }
        }

        else if (tokens.size() > c) {
            //////
        }

        Class.name = tokens.at(1).value;
        Class.file = tokens.at(1).file;
        Classes.push_back(Class);
    }
}


void parseMethod() {
    for (unsigned int i = 0; i < lines.size(); i ++) {
        tokens = lines.at(i);
        unsigned int c = 0;  // Cursor
        string whichFile;

        if (!(tokens.size() > c && tokens.at(c).type == "keyword" && tokens.at(c).value == "def")) continue;
        if (tokens.size() <= ++ c || tokens.at(c).type != "word") {
            ShowError(i + 1, "Invalid value statement");
        }

        if (tokens.size() > ++ c && tokens.at(c).type == "punc" && tokens.at(c).value == "(") {
            while(tokens.at(c).type != "punc" || tokens.at(c).value != ")") {
                c ++;
                // TO-DO Parameter List
            }
            c ++;
        }
        
        if (tokens.size() > c && tokens.at(c).type == "punc" && tokens.at(c).value == ":") {
            Method.returnType = tokens.at(c + 1).value;
            c += 2;
        } else {
            Method.returnType = "";
        }

        if (tokens.size() > c && tokens.at(c).type == "operator" && tokens.at(c).value == "->") {
            Method.startLine = Method.endLine = i;
        }
                            
        else if (tokens.size() > c && tokens.at(c).type == "punc" && tokens.at(c).value == "{") {
            Method.startLine = i;
            unsigned int counter = 1;
            unsigned int cursor = c + 1;
            unsigned int j = i;
            bool loop = true;
            while (j < lines.size() && loop) {
                vector<struct tok> tokens2 = lines.at(j);
                for (unsigned int k = cursor; k < tokens2.size(); k ++) {
                    if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "{") {
                        counter ++;
                    } else if (tokens2.at(k).type == "punc" && tokens2.at(k).value == "}") {
                        counter --;
                    }
                    if (counter == 0) {
                        Method.endLine = j;
                        loop = false;
                        break;
                    }
                }
                cursor = 0;
                j ++;
            }
        }

        else if (tokens.size() > c) {
            //////
        }

        Method.name = tokens.at(1).value;
        for (unsigned int k = 0; k < Classes.size(); k ++) {
            if (Classes.at(k).file == tokens.at(1).file) {
                if (Method.startLine > Classes.at(k).startLine && Method.endLine <= Classes.at(k).endLine) {
                    Classes.at(k).methods.push_back(Method);
                }
            }
        }     
    }        
}


void parseLine(string funcName, unsigned int start, unsigned int end) {
    for (lineNumber = start; lineNumber <= end; lineNumber ++) {
        tokens = lines.at(lineNumber);
        pos = 0;

        while(pos < tokens.size()) {
            // Parse variables
            if (tokens.at(pos).type == "keyword" && tokens.at(pos).value == "var") {
                parseVariable();
            }

            // Parse Assembly Instructions
            else if (tokens.at(pos).type == "keyword" && tokens.at(pos).value == "asm") {
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
                    for (unsigned int i = pos + 4; i < tokens.size(); i ++) {
                        unidentified += "\"" + tokens.at(i).value + "\" ";
                    }
                    ShowError(lineNumber + 1, "Unidentified symbol " + unidentified + "at the end of this line.");
                }

                int newElementId = AST["prog"][funcName].size();
                AST["prog"][funcName].add(newElementId);
                AST["prog"][funcName][newElementId].add("type", "asm");
                AST["prog"][funcName][newElementId].add("value", tokens.at(pos + 2).value);
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
                    parseCall(funcName, true);
                    break;
                }

                // Parse Function
                if (tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "(") {
                    if (!(tokens.at(tokens.size() - 1).type == "punc" && tokens.at(tokens.size() - 1).value == ")")) {
                        ShowError(lineNumber + 1, "Missing \")\"");
                    }
                    parseCall(funcName, false);
                    break;
                }


                // Parse assginment
                else if (tokens.at(pos).type == "word" && pos + 2 < tokens.size()  && tokens.at(pos + 1).value == "=" && tokens.at(pos + 2).value != "(") {
                    parseAssignment(funcName);
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
    AST["prog"].add("fun");
    AST["prog"].add("class");

    parseFunction();
    parseClass();
    parseMethod();
    
    for (unsigned int i = 0; i < Functions.size(); i ++) {
        cout << "func: " << Functions.at(i).name << endl;
        if (Functions.at(i).name == "main") {
            parseLine("main", Functions.at(i).startLine + 1, Functions.at(i).endLine);
            break;
        }
        if (i == Functions.size() - 1) {
            ShowError(0, "Function \"main\" required as entry of the program");
        }
    }
}