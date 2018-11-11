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

struct zen_method Method;
vector<struct zen_class> Classes;

void scanFunction() {
    struct zen_class Class;
    Class.name = "main";
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
        Class.methods.push_back(Method);
        continue;
    }
    Classes.push_back(Class);
}


void scanClass() {
    for (unsigned int i = 0; i < lines.size(); i ++) {
        struct zen_class Class;
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
            Class.returnType = tokens.at(c + 1).value;
            c += 2;
        } else {
            Class.returnType = "";
        }

        if (tokens.size() > c && tokens.at(c).type == "operator" && tokens.at(c).value == "->") {
            Class.startLine = Class.endLine = i;
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


void scanMethod() {
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


void parseAssignment(Tree branch) {
    branch.add("type", "assign");
    branch.add("operator", tokens.at(pos + 1).value);

    branch.add("left");
    branch["left"].add("type", "var");
    branch["left"].add("value", tokens.at(pos).value);

    branch.add("right");
    if (parseExpression(pos + 2, tokens.size()) != NULL) {
        branch["right"].add(*parseExpression(pos + 2, tokens.size()));
    } else if (tokens.at(pos + 2).type == "num") {
        branch["right"].add("type", "num");
        branch["right"].add("value", tokens.at(pos + 2).value);
    }
}


void parseASM(Tree branch) {
    // Check syntax
    if (pos + 1 >= tokens.size() || !(tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "(")) {
        ShowError(lineNumber + 1, "Missing symbol \"(\" while parsing asm");
    }
    if (pos + 3 >= tokens.size() || !(tokens.at(pos + 3).type == "punc" && tokens.at(pos + 3).value == ")")) {
        ShowError(lineNumber + 1, "Missing symbol \")\" while parsing asm");
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

    branch.add("type", "asm");
    branch.add("value", tokens.at(pos + 2).value);
}


void parseLine(string className, struct zen_method def, int lineNumber) {
    for (lineNumber = def.startLine + 1; lineNumber <= def.endLine; lineNumber ++) {
        tokens = lines.at(lineNumber);
        pos = 0;

        while(pos < tokens.size()) {
            // Parse variables
            if (tokens.at(pos).type == "keyword" && tokens.at(pos).value == "var") {
                parseVariable();
                break;
            }

            // Parse Assembly Instructions
            else if (tokens.at(pos).type == "keyword" && tokens.at(pos).value == "asm") {
                int newElementId = AST["prog"][className]["def"][def.name]["body"].size();
                AST["prog"][className]["def"][def.name]["body"].add(newElementId);
                Tree branch = AST["prog"][className]["def"][def.name]["body"][newElementId];
                parseASM(branch);
                break;
            }

            else if (tokens.at(pos).type == "word" && pos + 2 < tokens.size()) {
                // Parse Method Call
                if (pos + 3 < tokens.size() &&
                    tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "." &&
                    tokens.at(pos + 2).type == "word" &&
                    tokens.at(pos + 3).type == "punc" && tokens.at(pos + 3).value == "("
                ) {
                    if (!(tokens.at(tokens.size() - 1).type == "punc" && tokens.at(tokens.size() - 1).value == ")")) {
                        ShowError(lineNumber + 1, "Missing \")\"");
                    }
                    int newElementId = AST["prog"][className]["def"][def.name]["body"].size();
                    AST["prog"][className]["def"][def.name]["body"].add(newElementId);
                    Tree branch = AST["prog"][className]["def"][def.name]["body"][newElementId];
                    branch.add("type", "call");
                    branch.add("class", tokens.at(pos).value);
                    branch.add("def", tokens.at(pos + 2).value);
                    if (tokens.size() > pos + 4 && tokens.at(pos + 3).value != "()" && tokens.at(pos + 4).value != ")") {
                        branch.add("args");
                        branch["args"].add("type", tokens.at(pos + 4).type);
                        branch["args"].add("value", tokens.at(pos + 4).value);
                    } else {
                        branch.add("args");
                        branch["args"].add("type", "null");
                    }
                    break;
                }

                // Parse Function Call
                if (tokens.at(pos + 1).type == "punc" && tokens.at(pos + 1).value == "(") {
                    if (!(tokens.at(tokens.size() - 1).type == "punc" && tokens.at(tokens.size() - 1).value == ")")) {
                        ShowError(lineNumber + 1, "Missing \")\"");
                    }
                    int newElementId = AST["prog"][className]["def"][def.name]["body"].size();
                    AST["prog"][className]["def"][def.name]["body"].add(newElementId);
                    Tree branch = AST["prog"][className]["def"][def.name]["body"][newElementId];
                    branch.add("type", "call");
                    branch.add("class", "main");
                    branch.add("def", tokens.at(pos).value);
                    if (tokens.size() > 3 && tokens.at(pos + 3).value != "()") {
                        branch.add("args");
                        branch["args"].add("type", tokens.at(pos + 4).type);
                        branch["args"].add("value", tokens.at(pos + 4).value);
                    } else {
                        branch.add("args");
                        branch["args"].add("type", "null");
                    }
                    break;
                }

                // Parse assginment
                else if (tokens.at(pos).type == "word" && pos + 2 < tokens.size()  && tokens.at(pos + 1).value == "=" && tokens.at(pos + 2).value != "(") {
                    int newElementId = AST["prog"][className]["def"][def.name]["body"].size();
                    AST["prog"][className]["def"][def.name]["body"].add(newElementId);
                    Tree branch = AST["prog"][className]["def"][def.name]["body"][newElementId];
                    parseAssignment(branch);
                    break;
                    //pos ++;
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

    scanFunction();
    scanClass();
    scanMethod();
    bool isMainFunctionExist = false;
    
    // Parse Class
    for (unsigned int i = 0; i < Classes.size(); i ++) {
        
        AST["prog"].add(Classes.at(i).name);
        AST["prog"][Classes.at(i).name].add("def");
        
        for (unsigned int k = 0; k < Classes.at(i).methods.size(); k ++) {
            AST["prog"][Classes.at(i).name]["def"].add(Classes.at(i).methods.at(k).name);
            AST["prog"][Classes.at(i).name]["def"][Classes.at(i).methods.at(k).name].add("body");
            parseLine(Classes.at(i).name, Classes.at(i).methods.at(k), 0);

            if (Classes.at(i).name == "main" && Classes.at(i).methods.at(k).name == "main") {
                isMainFunctionExist = true;
            }
        }
        
        // Check if main function exist
        if (i == Classes.size() - 1 && !isMainFunctionExist) {
            ShowError(0, "Function \"main\" required as entry of the program");
        }
    }
}