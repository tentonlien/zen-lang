// Author: Tenton Lien
// 10/11/2018
#include <iostream>
#include <string>
#include <map>
#include "zc.h"
#include "tree.h"
#include "zasm/zasm.h"

using namespace std;

extern Tree AST;
extern void console(string, string, string);


// Calculation
void calc(string oprt, string receiver, string leftType, string leftValue, string rightType, string rightValue) {
    // Optimization for constant calculation
    if (leftType == "num" && rightType == "num") {
        if (oprt == "+") {
            string result = to_string(stoll(leftValue) + stoll(rightValue));
            zasm::lines.push_back("mov " + receiver + "," + result);
        } else if (oprt == "-") {
            string result = to_string(stoll(leftValue) - stoll(rightValue));
            zasm::lines.push_back("mov " + receiver + "," + result);
        }
    }
    
    else if (leftType == "word" || rightType == "word") {
        if (oprt == "+") {
            zasm::lines.push_back("mov ax," + leftValue);
            zasm::lines.push_back("add ax," + rightValue);
            zasm::lines.push_back("mov " + receiver + ",ax");
        } else if (oprt == "-") {
            zasm::lines.push_back("mov ax," + leftValue);
            zasm::lines.push_back("sub ax," + rightValue);
            zasm::lines.push_back("mov " + receiver + ",ax");
        }
        
    }
    
}

void generateData() {
    for (int i = 0; i < AST["data"].size(); i ++) {
        zasm::lines.push_back(
            AST["data"][i]["name"].getValue() + " " +
            AST["data"][i]["type"].getValue() + " " +
            AST["data"][i]["value"].getValue()
        );
    }
}

void generateCode(int i) {
    if (AST["prog"][i]["type"].getValue() == "asm") {
        zasm::lines.push_back(AST["prog"][i]["value"].getValue());
    } 
    
    else if (AST["prog"][i]["type"].getValue() == "assign") {
        if (AST["prog"][i]["left"]["type"].getValue() == "var") {
            if (AST["prog"][i]["right"]["type"].getValue() == "num") {
                zasm::lines.push_back("mov " + AST["prog"][i]["left"]["value"].getValue() + "," + AST["prog"][i]["right"]["value"].getValue());
            }
            
            else if (AST["prog"][i]["right"]["type"].getValue() == "arithmetic") {
                calc(AST["prog"][i]["right"]["operator"].getValue(),  // Operator
                    AST["prog"][i]["left"]["value"].getValue(),  // Receiver
                    AST["prog"][i]["right"]["left"]["type"].getValue(),  // Left type
                    AST["prog"][i]["right"]["left"]["value"].getValue(),  // Left value
                    AST["prog"][i]["right"]["right"]["type"].getValue(),  // Right type 
                    AST["prog"][i]["right"]["right"]["value"].getValue());  // Right value
                //
            }
        }
    } else if (AST["prog"][i]["type"].getValue() == "call") {
        //cout << AST["prog"][i]["function"].getValue() << endl;
        /*
        if (AST["prog"][i]["module"].getValue() == "Console") {
            if (AST["prog"][i]["args"].size() != 0) {
                console(AST["prog"][i]["function"].getValue(), AST["prog"][i]["args"]["type"].getValue(), AST["prog"][i]["args"]["value"].getValue());
            } else {
                console(AST["prog"][i]["function"].getValue(), "null", "null");
            }
            
        }
        */
    }
}


void Generator() {
    zasm::lines.push_back("section .data");
    generateData();
    zasm::lines.push_back("section .code");
    for (int i = 0; i < AST["prog"].size(); i++) {
        generateCode(i);
    }
    zasm::lines.push_back("int 0h");
}