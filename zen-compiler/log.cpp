// Tenton Lien, Nov.7, 2018

#include <cstdio>
#include <iostream>
#include <string>
#include "tree.h"
#include "zc.h"
#include "zasm/zasm.h"
using namespace std;

FILE *fp = NULL;

void outputTokens() {
    fprintf(fp, "Token Stream:\n");
    for (int i = 0; i < lines.size(); i ++) {
        for(int k = 0; k < lines.at(i).size(); k ++) {
            fprintf(fp, "Line %d: %-15s%-10s\n", i, lines.at(i).at(k).type.c_str(), lines.at(i).at(k).value.c_str());
        }
        
    }
    fprintf(fp, "\n");
}

void outputAST() {
    fprintf(fp, "Abstract Syntax Tree:\n");
    extern Tree AST;
    fprintf(fp, (AST.preOrder() + "\n\n").c_str());
}

void outputFunctionList() {
    struct function {
        string name;
        string returnType;
        int startLine;
        int endLine;
        vector<pair<string, string>> parameterList;
    };

    extern vector<struct function> Functions;
    fprintf(fp, "Function List:\n");
    fprintf(fp, "Function Name       Lines      Return Type\n");
    for (int i = 0; i < Functions.size(); i ++) {
        fprintf(fp, "%-20s%d - %-6d%-5s\n", Functions.at(i).name.c_str(), Functions.at(i).startLine, Functions.at(i).endLine, Functions.at(i).returnType.c_str());
    }
    fprintf(fp, "\n");
}

void outputClassList() {
    struct zclass {
        string name;
        int startLine;
        int endLine;
    };

    extern vector<struct zclass> zClasses;
    fprintf(fp, "Class List:\n");
    fprintf(fp, "Class Name          Lines\n");
    for (int i = 0; i < zClasses.size(); i ++) {
        fprintf(fp, "%-20s%d - %-6d\n", zClasses.at(i).name.c_str(), zClasses.at(i).startLine, zClasses.at(i).endLine);
    }
    fprintf(fp, "\n");
}

void outputASM() {
    fprintf(fp, "ASM Code:\n");
    for (int i = 0; i < zasm::lines.size(); i ++) {
        fprintf(fp, (zasm::lines.at(i) + "\n").c_str());
    }
}

void log() {
    extern string sourcePath;
    fp = fopen((sourcePath + "/compilation.log").c_str(), "w+");
    outputTokens();
    outputFunctionList();
    outputClassList();
    outputAST();
    outputASM();
    fclose(fp);
}