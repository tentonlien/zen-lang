#include <iostream>
#include "generator.h"
#include "parser.h"
#include "zenc.h"

std::vector<std::string> asmLines;
std::vector<std::string> globalVariable;
std::vector<std::vector<std::string>> procedures;
bool isMainFunctionFound = false;

unsigned int tempVariableId = 0;


void Expr::codegen() {}


void AssignmentExpr::codegen() {
    asmLines.push_back("mov " + this -> leftValue + " " + this -> rightExpr.name);
}


void ConstString::codegen() {
    asmLines.push_back("$tmp" + std::to_string(tempVariableId) + " i8_ptr '" + this -> value + "'");
    asmLines.push_back("push tmp" + std::to_string(tempVariableId ++));
}


void CallExpr::codegen() {
    if (this -> callee == "asm") {
        if (!this -> args.empty()) {
            asmLines.push_back(this -> args[0] -> toString());
        } else {
            showError(0, 0, "Error", "Missing args for asm function");
        }
    } else {
        for (unsigned int i = 0; i < this -> args.size(); i ++) {
            //asmLines.push_back("push " + this -> args[i] -> toString());
            this -> args[i] -> codegen();
        }
        asmLines.push_back("call " + this -> callee);
    }
}


void VariableExpr::codegen() {
    // convert data type
    if (this -> type == "int") {
        this -> type = "i64";
    }
    if (this -> value == "") {
        asmLines.push_back("$" + this -> name + " " + this -> type);
    } else {
        asmLines.push_back("$" + this -> name + " " + this -> type + " " + this -> value);
    }
}


void FunctionExpr::codegen() {
    asmLines.push_back("@" + this -> name);
    for (auto item: this -> body) {
        item -> codegen();
    }
    if (this -> name == "main") {
        asmLines.push_back("proc 0 0 0");
    } else {
        asmLines.push_back("ret");
    }
}


void ClassExpr::codegen() {
    for (auto variable: this -> memberVariables) {
        variable -> codegen();
    }
    for (auto method: this -> methods) {
        method -> codegen();
    }
}


std::vector<std::string> Generator(std::shared_ptr<ClassExpr> AST) {   
    asmLines.push_back(";source_filename: " + sourceFileName);
    asmLines.push_back(".version 1");
    asmLines.push_back(".stack 256");

    AST -> codegen();
    
    for (auto item:asmLines) {
        std::cout << item <<std::endl;
    }
    return asmLines;
}