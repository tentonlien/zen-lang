#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "zenc.h"
#include "lexer.h"
#include "parser.h"
#include "generator.h"
#include "assembler.h"

std::string sourceFileName;
std::string compilerPath;
std::string sourcePath;
bool infoMode = false;

// zenc YourProgram.zen
// zenc YourProgram.zasm

void showError(unsigned int fileId, unsigned int lineNumber, std::string errorType, std::string errorInfo) {
    // print error type in red color
    #if _WIN32
        system("color 0c");
        std::cout << errorType;
    #elif __linux__
        std::cerr << "\033[22;31m" << errorType << "\033[22;37m";
    #else
        std::cout << errorType;
    #endif

    if (lineNumber == 0) {
        std::cout << ": " << errorInfo << std::endl;
    } else if (fileId == -1) {
        std::cout << " at Line " << lineNumber << ": " << errorInfo << std::endl;
    } else {
        std::cout << " at File \"" << files.at(fileId) << "\", Line " << lineNumber << ": " << errorInfo << std::endl;
    }
    exit(0);
}

std::string readTextFile(std::string path) {
    std::ifstream textFile(path);

    if (!textFile) {
        showError(0, 0, "Loading Error", "Failed to read file");
    }

    std::istreambuf_iterator<char> begin(textFile);
    std::istreambuf_iterator<char> end;
    std::string fileContent(begin, end);
    textFile.close();
    return fileContent;
}


int main(int argc, char* argv[]) {
    compilerPath = argv[0];
    int i = compilerPath.length();
    while (compilerPath[i] != '/') i --;
    compilerPath = compilerPath.substr(0, i);
    
    // Check args and get file name
    if (argc == 1) {
        std::cout << "zenc [-options] *.zen" << std::endl;
        return 0;
    }

    sourceFileName = argv[1];
    if (sourceFileName.find(".zen") == std::string::npos) {
        if (sourceFileName.find(".zasm") == std::string::npos) {
            showError(0, 0, "Format Error", "Invalid file format");
        }
        
        // call assembler
        else {
            std::vector<std::string> asmLines;
            std::string asmCodes = readTextFile(sourceFileName);
            unsigned int curLineStart = 0;
            for (unsigned int i = 0; i < asmCodes.length(); i ++) {
                if (asmCodes[i] == '\n') {
                    //std::cout << "ahahahah " << i - curLineStart <<std::endl;
                    if (i - 1 <= curLineStart) {
                        asmLines.push_back("");
                        curLineStart = i + 1;
                    } else {
                        asmLines.push_back(asmCodes.substr(curLineStart, i - curLineStart - 1));
                        curLineStart = i + 1;
                    }
                    //std::cout << "haha: " << curLineStart << " "<< asmCodes.substr(curLineStart, i  - curLineStart) << std::endl;
                    
                } else if (i + 1 == asmCodes.length()) {
                    if (i - 1 <= curLineStart) {
                        asmLines.push_back("");
                    } else {
                        asmLines.push_back(asmCodes.substr(curLineStart, i - curLineStart + 1));
                    }
                }
            }
            Assembler(asmLines);
            exit(0);
        }
    }

    // Read source code file
    std::string codes = readTextFile(sourceFileName);

    i = sourceFileName.length();
    while (sourceFileName[i] != '/') i --;
    sourcePath = sourceFileName.substr(0, i);

    // lexer: lexical analysis
    Lexer(codes, sourceFileName);
    
    // parser: syntax analysis
    auto AST = Parser(); 

    // generator: codegen
    std::vector<std::string> asmLines = Generator(AST);

    // zen assembler
    Assembler(asmLines);
    
    return 0;
}