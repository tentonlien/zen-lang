// Author: Tenton Lien
// 10/11/2018
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include "zc.h"
#include "tree.h"
#include "zasm/zasm.h"

using namespace std;

extern Tree AST;
extern void console(string, string, string);

string label = "main.main";
vector<pair<string, Tree>> procedures;

// Calculation
void calc(string oprt, string receiver, string leftType, string leftValue, string rightType, string rightValue) {
    // Optimization for constant calculation
    if (leftType == "num" && rightType == "num") {
        if (oprt == "+") {
            string result = to_string(stoll(leftValue) + stoll(rightValue));
            zasm::lines.push_back(label + "mov " + receiver + "," + result);
        } else if (oprt == "-") {
            string result = to_string(stoll(leftValue) - stoll(rightValue));
            zasm::lines.push_back(label + "mov " + receiver + "," + result);
        }
    }
    
    else if (leftType == "word" || rightType == "word") {
        if (oprt == "+") {
            zasm::lines.push_back(label + "mov ax," + leftValue);
            label = "";
            zasm::lines.push_back(label + "add ax," + rightValue);
            zasm::lines.push_back(label + "mov " + receiver + ",ax");
        } else if (oprt == "-") {
            zasm::lines.push_back(label + "mov ax," + leftValue);
            label = "";
            zasm::lines.push_back(label + "sub ax," + rightValue);
            zasm::lines.push_back(label + "mov " + receiver + ",ax");
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

void generateCode(Tree branch) {
    label = "@" + label + ":";
    for (int i = 0; i < branch.size(); i ++) {
        // ASM
        if (branch[i]["type"].getValue() == "asm") {
            zasm::lines.push_back(label + branch[i]["value"].getValue());
        }
    
        else if (branch[i]["type"].getValue() == "assign") {
        
            if (branch[i]["left"]["type"].getValue() == "var") {
                if (branch[i]["right"]["type"].getValue() == "num") {
                    zasm::lines.push_back(label + "mov " + branch[i]["left"]["value"].getValue() + "," + branch[i]["right"]["value"].getValue());
                }
            
                else if (branch[i]["right"]["type"].getValue() == "arithmetic") {
                    calc(branch[i]["right"]["operator"].getValue(),  // Operator
                        branch[i]["left"]["value"].getValue(),  // Receiver
                        branch[i]["right"]["left"]["type"].getValue(),  // Left type
                        branch[i]["right"]["left"]["value"].getValue(),  // Left value
                        branch[i]["right"]["right"]["type"].getValue(),  // Right type 
                        branch[i]["right"]["right"]["value"].getValue());  // Right value
                }
            }
        } else if (branch[i]["type"].getValue() == "call") {
            if (branch[i]["args"]["type"].getValue() != "null") {
                zasm::lines.push_back("push " + branch[i]["args"]["value"].getValue());
            }
            zasm::lines.push_back(label + "jmp " + branch[i]["class"].getValue() + "." + branch[i]["def"].getValue());
            
            label = branch[i]["class"].getValue() + "." + branch[i]["def"].getValue();
            procedures.push_back({label, AST["prog"][branch[i]["class"].getValue()]["def"][branch[i]["def"].getValue()]["body"]});
        }
        
        label = "";
    }
}


void Generator() {
    zasm::lines.push_back("section .data");
    generateData();

    zasm::lines.push_back("section .code");

    label = "main.main";
    generateCode(AST["prog"]["main"]["def"]["main"]["body"]);
    zasm::lines.push_back("int 0h");

    while (!procedures.empty()) {
        label = procedures.begin() -> first;
        generateCode(procedures.begin() -> second);
        zasm::lines.push_back("ret 0");
        procedures.erase(procedures.begin());
    } 
}