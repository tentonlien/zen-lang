#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "../zen_assembler/zasm.h"
#include "zc.h"

using namespace std;

extern struct tok token;
extern vector<struct tok> tokens;
string compilerPath;

int main(int argc, char* argv[]) {
    compilerPath = argv[0];
    int i = compilerPath.length();
    while (compilerPath[i] != '/') i --;
    compilerPath = compilerPath.substr(0, i);
    
    // Check args and get file name
    if (argc == 1) {
        cout << "zc -[options] *.zen" << endl;
        return 0;
    }

    string sourceFileName = argv[1];
    if (sourceFileName.find(".zen") == string:: npos) {
        ShowError(0, "Invalid file format");
    }

    // Read source code file
    std::ifstream sourceFile(sourceFileName);

    if (!sourceFile) {
        ShowError(0, "Failed to load assembly code file");
    }

    istreambuf_iterator<char> begin(sourceFile);
    istreambuf_iterator<char> end;
    string codes(begin, end);
    
    sourceFile.close();  
    Lexer(codes);  // Lexical analysis

    //*
    cout << "Token Stream" << endl;
    for (int i = 0; i < lines.size(); i ++) {
        for(int k = 0; k < lines.at(i).size(); k ++) {
            printf("Line %d: %-15s%-10s\n", i, lines.at(i).at(k).type.c_str(), lines.at(i).at(k).value.c_str());
        }
        
    }
    //*/

    cout << endl << "Abstract Syntax Tree" << endl;
    Parser();  // Syntax analysis
    cout << endl << endl;
    Generator();
    
    for (int i = 0; i < zasm::lines.size(); i ++) {
        cout << zasm::lines.at(i) << endl;
    }
    

    int pos = sourceFileName.find(".zen");
    string output = sourceFileName.substr(0, pos) + ".asm";
    Initializer(output);  // Call assembler

    return 0;
}