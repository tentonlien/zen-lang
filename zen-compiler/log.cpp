// Tenton Lien, Nov.7, 2018

#include <cstdio>
#include <iostream>
#include <string>
#include "tree.h"
#include "zc.h"
#include "zasm/zasm.h"
using namespace std;

FILE *fp = NULL;

string jsonFormat(string json) {
    unsigned int counter = 0;
    const char* ch = json.c_str();
    string result;
    for (unsigned int i = 0; ch[i] != 0; i ++) {
        if (ch[i] == '{') {
            result += ch[i];
            counter ++;
            result += '\n';
            for (int k = 0; k < counter; k ++) result += "    ";
        } else if (ch[i] == '}') {
            result += '\n';
            counter --;
            for (int k = 0; k < counter; k ++) result += "    ";
            result += ch[i];
        } else if (ch[i] == ',') {
            result += ch[i];
            result += '\n';
            for (int k = 0; k < counter; k ++) result += "    ";
        }
         else {
            result += ch[i];
        }
    }
    return result;
}

void outputTokens() {
    fprintf(fp, "Token Stream:\n");
    for (unsigned int i = 0; i < lines.size(); i ++) {
        for(unsigned int k = 0; k < lines.at(i).size(); k ++) {
            fprintf(fp, "%-50s Line %d: %-15s%-10s\n", lines.at(i).at(k).file.c_str(), lines.at(i).at(k).line, lines.at(i).at(k).type.c_str(), lines.at(i).at(k).value.c_str());
        }
        
    }
    fprintf(fp, "\n");
}


void outputAST() {
    fprintf(fp, "Abstract Syntax Tree:\n");
    extern Tree AST;
    fprintf(fp, (jsonFormat(AST.preOrder()) + "\n\n").c_str());
}


void outputFunctionList() {
    fprintf(fp, "Function List:\n");
    fprintf(fp, "%-20s%-15s%-10s\n", "Name", "Lines", "Return Type");
    for (unsigned int i = 0; i < Functions.size(); i ++) {
        fprintf(fp, "%-20s%d - %-10d%-5s\n", Functions.at(i).name.c_str(), Functions.at(i).startLine, Functions.at(i).endLine, Functions.at(i).returnType.c_str());
    }
    fprintf(fp, "\n");
}


void outputClassList() {
    fprintf(fp, "Class List:\n");
    fprintf(fp, "%-20s%-15sMethod\n", "Class", "Lines");
    for (unsigned int i = 0; i < Classes.size(); i ++) {
        fprintf(fp, "%-20s%d - %-11d", Classes.at(i).name.c_str(), Classes.at(i).startLine, Classes.at(i).endLine);
        for (unsigned int k = 0; k < Classes.at(i).methods.size(); k ++) {
            if (k == 0) {
                fprintf(fp, "%-20s%d - %-10d%-5s\n", Classes.at(i).methods.at(k).name.c_str(), Classes.at(i).methods.at(k).startLine, Classes.at(i).methods.at(k).endLine, Classes.at(i).methods.at(k).returnType.c_str());
            } else {
                fprintf(fp, "%35s%-20s%d - %-10d%-5s\n", " ", Classes.at(i).methods.at(k).name.c_str(), Classes.at(i).methods.at(k).startLine, Classes.at(i).methods.at(k).endLine, Classes.at(i).methods.at(k).returnType.c_str());
            }
            
        }
    }
    fprintf(fp, "\n");
}


void outputASM() {
    fprintf(fp, "ASM Code:\n");
    for (unsigned int i = 0; i < zasm::lines.size(); i ++) {
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