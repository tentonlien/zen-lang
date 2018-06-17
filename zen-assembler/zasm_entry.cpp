/* Tenton Lien */
/* 9/27/2018 */
/* ZASM has been rewritten in C++ */
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "zasm.h"

using namespace std;

int main(int argc, char* argv[]) {
    // If retrieving no args
    if (argc == 1) {
        ShowError(0, "No input files");
    }

    string asmFileName = argv[1];
    if (asmFileName.find(".asm") == string:: npos) {
        ShowError(0, "Invalid file format");
    }
    // Load assembly code from file
    ifstream asmFile;
    asmFile.open(asmFileName.data());

    if (!asmFile) {
        ShowError(0, "Failed to load assembly code file");
    }

    string line;
    while (getline(asmFile, line)) {
        zasm::lines.push_back(line);
    }

    asmFile.close();
    
    Initializer(asmFileName);
    return 0;
}