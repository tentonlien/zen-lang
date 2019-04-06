#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <memory>
#include <vector>

class Expr {
public:
    std::string name;
    std::string value;
    virtual void codegen();
    virtual std::string toString();
};


class FunctionExpr: public Expr {
public:
    std::string name;
    int returnType;
    std::vector<int> args;
    std::vector<std::shared_ptr<Expr>> body;

    void codegen();
    std::string toString();
};


class VariableExpr: public Expr {
public:
    std::string type;
    std::string name;
    std::string value;

    void codegen();
    std::string toString();
};

class ConstString: public Expr {
public:
    std::string type;
    std::string value;
    
    void codegen();
    std::string toString();
};


class ClassExpr: public Expr {
public:
    std::string name;
    std::vector<int> args;
    std::vector<std::shared_ptr<ClassExpr>> classes;
    std::vector<std::shared_ptr<FunctionExpr>> methods;
    std::vector<std::shared_ptr<VariableExpr>> memberVariables;
    
    void codegen();
    std::string toString();
};


class AssignmentExpr: public Expr {
public:
    std::string leftType;
    std::string leftValue;
    Expr rightExpr;

    void codegen();
    std::string toString();
};


class CallExpr: public Expr {
public:
    std::string callee;
    std::vector<std::shared_ptr<Expr>> args;

    void codegen();
    std::string toString();
};

extern std::shared_ptr<ClassExpr> Parser();

#endif