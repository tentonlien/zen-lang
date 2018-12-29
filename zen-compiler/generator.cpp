#include <iostream>
#include "generator.h"
#include "parser.h"
#include "zenc.h"

std::vector<std::string> asmLines;
std::vector<std::string> globalVariable;
std::vector<std::vector<std::string>> procedures;
bool isMainFunctionFound = false;


void Expr::codegen() {}


void FunctionExpr::codegen() {
    asmLines.push_back("@" + this -> name);
    for (auto item: this -> body) {
        item -> codegen();
    }
    if (this -> name == "main") {
        asmLines.push_back("prog 0 0");
    } else {
        asmLines.push_back("ret");
    }
}


void VariableExpr::codegen() {
    if (this -> value == "") {
        asmLines.push_back("$" + this -> name + " " + this -> type);
    } else {
        asmLines.push_back("$" + this -> name + " " + this -> type + " " + this -> value);
    }
}


void AssignmentExpr::codegen() {
    asmLines.push_back("mov " + this -> leftValue + " " + this -> rightExpr.name);
}


void ClassExpr::codegen() {
    for (auto variable: this -> memberVariables) {
        variable -> codegen();
    }
    for (auto method: this -> methods) {
        method -> codegen();
    }
}


void CallExpr::codegen() {
    asmLines.push_back("call " + this -> callee);
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