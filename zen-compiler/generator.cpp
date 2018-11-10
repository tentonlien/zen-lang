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

void generateCode(string funcName, int i) {
    if (AST["prog"][funcName][i]["type"].getValue() == "asm") {
        zasm::lines.push_back(AST["prog"][funcName][i]["value"].getValue());
    } 
    
    else if (AST["prog"][funcName][i]["type"].getValue() == "assign") {
        if (AST["prog"][funcName][i]["left"]["type"].getValue() == "var") {
            if (AST["prog"][funcName][i]["right"]["type"].getValue() == "num") {
                zasm::lines.push_back("mov " + AST["prog"][funcName][i]["left"]["value"].getValue() + "," + AST["prog"][funcName][i]["right"]["value"].getValue());
            }
            
            else if (AST["prog"][funcName][i]["right"]["type"].getValue() == "arithmetic") {
                calc(AST["prog"][funcName][i]["right"]["operator"].getValue(),  // Operator
                    AST["prog"][funcName][i]["left"]["value"].getValue(),  // Receiver
                    AST["prog"][funcName][i]["right"]["left"]["type"].getValue(),  // Left type
                    AST["prog"][funcName][i]["right"]["left"]["value"].getValue(),  // Left value
                    AST["prog"][funcName][i]["right"]["right"]["type"].getValue(),  // Right type 
                    AST["prog"][funcName][i]["right"]["right"]["value"].getValue());  // Right value
            }
        }
    } else if (AST["prog"][funcName][i]["type"].getValue() == "call") {
        if (AST["prog"][funcName][i]["class"].getValue() != "") {
            // TEMP: Console.write()
            zasm::lines.push_back("mov ax,0");
            zasm::lines.push_back("mov bx,1");
            zasm::lines.push_back("mov dx," + AST["prog"][funcName][i]["args"]["value"].getValue());
            zasm::lines.push_back("int 11h");

            // Function
            if (AST["prog"][funcName][i]["class"].getValue() == "") {
                for (int i = 0; i < Functions.size(); i ++) {
                    break;
                }
            }

        }
    }
}


void Generator() {
    zasm::lines.push_back("section .data");
    generateData();
    zasm::lines.push_back("section .code");
    for (int i = 0; i < AST["prog"]["main"].size(); i ++) {
        generateCode("main", i);
    }
    zasm::lines.push_back("int 0h");
}