#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "zasm/zasm.h"
#include "zc.h"

using namespace std;

extern struct tok token;
extern vector<struct tok> tokens;
string compilerPath;
string sourcePath;
bool infoMode = false;

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

    i = sourceFileName.length();
    while (sourceFileName[i] != '/') i --;
    sourcePath = sourceFileName.substr(0, i);

    // Lexical analysis
    Lexer(codes, sourceFileName);  
    
    // Syntax analysis
    Parser();  
    
    Generator();
    
    // Call assembler
    int pos = sourceFileName.find(".zen");
    string output = sourceFileName.substr(0, pos) + ".asm";
    Initializer(output);  

    // Output compilation log
    extern void log();
    log();
    return 0;
}