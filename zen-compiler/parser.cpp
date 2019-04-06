// Author: Tenton Lien
// 10/7/2018
// All syntax checks are done in the parser

#include <iostream>
#include <memory>
#include "lexer.h"
#include "parser.h"
#include "zenc.h"

std::vector<int> variableList;
std::vector<int> classList;

unsigned int curTokenId = 0;
Token curToken;

static std::shared_ptr<AssignmentExpr> parseAssignment();
static std::shared_ptr<Expr> parseExpression();

unsigned int currentClassId = 0;

std::string jsonFormat(std::string json) {
    unsigned int counter = 0;
    const char* ch = json.c_str();
    std::string result;
    for (unsigned int i = 0; ch[i] != 0; i ++) {
        if (ch[i] == '{') {
            result += ch[i];
            counter ++;
            result += '\n';
            for (unsigned int k = 0; k < counter; k ++) result += "    ";
        } else if (ch[i] == '}') {
            result += '\n';
            counter --;
            for (unsigned int k = 0; k < counter; k ++) result += "    ";
            result += ch[i];
        } else if (ch[i] == ',') {
            result += ch[i];
            result += '\n';
            for (unsigned int k = 0; k < counter; k ++) result += "    ";
        }
         else {
            result += ch[i];
        }
    }
    return result;
}


bool gotoNextToken() {
    if (curTokenId < tokens.size() - 1) {
        curTokenId ++;
        curToken = tokens.at(curTokenId);
        return true;
    }
    return false;
}


std::string Expr::toString() {}

std::string VariableExpr::toString() {
    std::string json = "{";
    json += "type: \"var\",";
    json += "name: \"" + this -> name + "\",";
    json += "dtype: \"" + this -> type + "\",";
    json += "}";
    return json;
}

std::string ConstString::toString() {
    return this -> value;
}

std::string ClassExpr::toString() {
    std::string json = "{";
    json += "type: \"class\",";
    json += "name: \"" + this -> name + "\",";
    json += "variables: [";
    for (auto variable: this -> memberVariables) {
        json += variable -> toString();
    }
    json += "],";

    json += "classes: [";
    for (auto eleClass: this -> classes) {
        json += eleClass -> toString();
    }
    json += "],";

    json += "methods: [";
    for (auto method: this -> methods) {
        json += method -> toString();
    }
    json += "]";
    json += "}";
    return json;
}


std::string FunctionExpr::toString() {
    std::string json = "{";
    json += "type: \"function\",";
    json += "name: \"" + this -> name + "\",";
    json += "body: {";
    for (std::shared_ptr<Expr> singleExpr: this -> body) {
        json += singleExpr -> toString() + ",";
    }
    json += "}";
    json += "},";
    return json;
}


std::string CallExpr::toString() {
    std::string json = "{";
    json += "type: \"call\",";
    json += "callee: \"" + this -> callee + "\",";
    for (unsigned int i = 0; i < this -> args.size(); i ++) {
        std::cout << this -> callee << this -> args.size() << std::endl;
        json += "args: \"" + this -> args[i] -> toString() + "\",";
    }
    json += "}";
    return json;
}


std::string AssignmentExpr::toString() {
    std::string json = "{";
    json += "type: \"Assign\",";
    json += "left: \"" + this -> leftValue + "\",";
    json += "}";
    return json;
}


static std::shared_ptr<VariableExpr> parseVariable() {
    if (!curToken.isKeyword("var")) {
        return nullptr;
    }

    std::shared_ptr<VariableExpr> newVariable(new VariableExpr);
    gotoNextToken(); 

    // handle variable name
    if (!curToken.isIdentifier()) {
        showError(curToken.fileId, curToken.lineNumber, "Variable Statement Error", "Missing Identifier");
    } else {
        newVariable -> name = curToken.value;
    }
    gotoNextToken();

    // handle variable data type
    if (curToken.isPunc(":")) {
        gotoNextToken();
        if (curToken.isType()) {
            newVariable -> type = curToken.value;
        } else {
            showError(curToken.fileId, curToken.lineNumber, "Error", "Invalid data type '" + curToken.value + "'");
        }
    }

    std::cout << "Parse Variable: " << newVariable -> name << " " << newVariable -> type << std::endl;
    return newVariable;
}


static std::shared_ptr<CallExpr> parseCall() {
    std::shared_ptr<CallExpr> newCall = std::make_shared<CallExpr>();
    newCall -> callee = tokens[curTokenId - 1].value;
    while (!curToken.isPunc(")")) {
        gotoNextToken();
        std::shared_ptr<Expr> newExpression = parseExpression();
        if (newExpression != nullptr) {
            newCall -> args.push_back(parseExpression());
        }
    }
    std::cout << "Parse Call: " << newCall -> callee << std::endl;
    return newCall;
}


static std::shared_ptr<Expr> parseExpression() {
    std::shared_ptr<Expr> newExpression;
    // parse class

    if (curToken.isKeyword("class")) {
        // TO DO
        gotoNextToken();
    }

    // parse function
    else if (curToken.isKeyword("fun")) {
        //newExpr = parseFunction();
    }

    // parse return
    else if (curToken.isKeyword("return")) {
        // TO DO
    }

    // parse variable
    else if (curToken.isKeyword("var")) {
        return parseVariable();
    }

    // parse assignment
    else if (curToken.isOperator("=")) {
        return parseAssignment();
    }
    
    // parse call
    else if (curToken.isIdentifier() || curToken.isKeyword("asm")) {
        gotoNextToken();
        if (curToken.isPunc("(")) {
            return parseCall();
        }
        return newExpression;
    }

    else if (curToken.isString()) {
        std::shared_ptr<ConstString> newString = std::make_shared<ConstString>();
        newString -> value = curToken.value;
        return newString;
    }
    return nullptr;
}


static std::shared_ptr<AssignmentExpr> parseAssignment() {
    std::shared_ptr<AssignmentExpr> newAssign = std::make_shared<AssignmentExpr>();
    newAssign -> leftValue = tokens[curTokenId - 1].value;
    //newAssign.leftExpr = parseAll();
    gotoNextToken();
    std::cout << "Parse Assignment: " << newAssign -> leftValue << std::endl;
    return newAssign;
}


static std::shared_ptr<FunctionExpr> parseFunction() {
    if (!curToken.isKeyword("fun")) {
        return nullptr;
    }

    std::shared_ptr<FunctionExpr> newFunction(new FunctionExpr);
    gotoNextToken();

    // handle function name
    if (curToken.isIdentifier()) {
        newFunction -> name = curToken.value;
    }
    
    while (!curToken.isPunc("{") && !curToken.isOperator("->")) {
        gotoNextToken();
    }

    // parse function body
    unsigned int braceCount = 0;
    while (gotoNextToken()) {
        if (curToken.isPunc("{")) {
            braceCount ++;
        } else if (curToken.isPunc("}")) {
            if (braceCount == 0) {
                break;
            } else {
                braceCount --;
            }
        } else {
            std::shared_ptr<Expr> newExpression(parseExpression());
            if (newExpression != nullptr) {
                newFunction -> body.push_back(newExpression);
            }
        }
    }
    
    std::cout << "Parse Function: " << newFunction -> name << std::endl;
    return newFunction;
}


static std::shared_ptr<ClassExpr> parseClass() {
    if (!curToken.isKeyword("class")) {
        std::cout << "passed: " << tokens.at(curTokenId + 1).value << std::endl;
        return nullptr;
    }

    std::shared_ptr<ClassExpr> newClass(new ClassExpr);
    gotoNextToken();

    if (curToken.isIdentifier()) {
        newClass -> name = curToken.value;
    } else {
        showError(0, 0, "Error", "Missing class name: " + curToken.value);
    }
    
    gotoNextToken();

    unsigned int braceCount = 0;
    while (gotoNextToken()) {
        if (curToken.isPunc("{")) {
            braceCount ++;
        } else if (curToken.isPunc("}")) {
            if (braceCount == 0) {
                break;
            } else {
                braceCount --;
            }
        } else {
            // parse functions
            std::shared_ptr<FunctionExpr> functionPointer(parseFunction());
            if (functionPointer  != nullptr) {
                newClass -> methods.push_back(functionPointer);
            } else {
                // parse classes
                std::shared_ptr<ClassExpr> classPointer(parseClass());
                if (classPointer != nullptr) {
                    newClass -> classes.push_back(classPointer);
                } else {
                    // parse global variables
                    std::shared_ptr<VariableExpr> variablePointer(parseVariable());
                    if (variablePointer != nullptr) {
                        newClass -> memberVariables.push_back(variablePointer);
                    }
                }
            }
        }
    }
    
    std::cout << "Parse Class: " << newClass -> name << std::endl;
    return newClass;
}


std::shared_ptr<ClassExpr> Parser() {
    curToken = tokens.at(curTokenId);
    std::shared_ptr<ClassExpr> mainClass(parseClass());
    if (mainClass != nullptr) {
        std::cout << jsonFormat(mainClass -> toString()) << std::endl;
    } else {
        std::cout << "nullptr" <<std::endl;
    }
    return mainClass;
}