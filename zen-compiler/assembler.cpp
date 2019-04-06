/* Tenton Lien */
/* 9/27/2018 */
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <iostream>
#include "zenc.h"
#include "assembler.h"

std::vector<unsigned int> instructions_b;

unsigned int currentLine = 0;

unsigned char opcodes_bin[] = {
    0x00, 0x01, 0x02, 0x03, 0x04,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x20,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46
};

std::string opcodes_asm[] = {
    "mov", "glob", "xchg", "push", "pop",
    "add", "inc", "sub", "dec", "cmp", "mul", "div",
    "and",
    "call", "ret", "loop", "jmp", "je", "jne", "jg", "jl", "jge", "jle",
    "proc", "sys", "cons", "file", "net", "thread", "gui"
};

int ins_operand_amount[] = {
    2, 2, 1, 1,
    2, 1, 2, 1, 2, 2, 2,
    2,
    1, 1, 1, 1, 1, 1, 1, 1, 1
};

unsigned int bytecodeVersion = 1;
unsigned int globalStackSize = 64;
unsigned int globalDataSegmentSize = 0;
unsigned int globalConstantAmount = 0;
unsigned int procedureAmount = 0;

unsigned int currentProcedureStartPosition = 0;
unsigned int currentInstructionCursor = 0;

bool isGlobal = true;

struct DATA {
    std::string name;
    unsigned int type;
    std::string value;
};

std::vector<struct DATA> globalConstants;

struct JumpPoint {
    std::string tagName;
    unsigned int tagLine;
};

std::vector<struct JumpPoint> jumpPoints;

std::vector<std::string> globalVariables;
std::vector<std::string> localVariables;

std::string trim(std::string str) {
    unsigned int pos;
	while ((pos = str.find(" ")) != std::string::npos) {
		str.replace(pos, 1, "");
	}
    return str;
}

void writeExecutableFile() {
    unsigned int pos;
    if ((pos = sourceFileName.find(".zasm")) == std::string::npos) {
        if ((pos = sourceFileName.find(".zen")) == std::string::npos) {
            showError(0, 0, "Output Error", "Invalid file format");
        }
    }
    
    std::string output = sourceFileName.substr(0, pos) + ".zef";
    std::ofstream of(output, std::ios::binary);
    if (!of) {
        showError(0, 0, "Assemble Error", "Write executable file failed");
    }

    // write magic number
    unsigned int magicNumber = 0x214E455A;
    of.write((char*)&magicNumber, 4);

    // write bytecode version    
    of.write((char*)&bytecodeVersion, 4);

    // write global stack size
    of.write((char*)&globalStackSize, 4);

    // write global data segment size
    of.write((char*)&globalDataSegmentSize, 4);

    // write global constant amount
    of.write((char*)&globalConstantAmount, 4);
    for (auto constant: globalConstants) {
        of.write((char*)&(constant.type), 4);
        unsigned int arraySize, heapSize;

        switch (constant.type) {
            case 0: {
                unsigned long long value = std::stoi(constant.value, 0, 10);
                of.write((char*)&value, 8);
                break;
            }

            case 2: {
                arraySize = constant.value.length() + 1;
                heapSize = constant.value.length() / 4;
                if (heapSize % 4 != 0) {
                    heapSize ++;
                }
                // std::cout << arraySize << " " << heapSize << std::endl;
                
                of.write((char*)&arraySize, 4);
                of.write((char*)&heapSize, 4);
                // int temp = 1;
                for (char singleChar: constant.value) {
                    of.write((char*)&singleChar, 1);
                }
                int empty = heapSize * 4 - arraySize + 1;
                for (int i = 0; i < empty; i ++) {
                    char zero = 0;
                    of.write((char*)&zero, 1);
                }
                break;
            }
        }
        
    }

    // write procedure amount
    of.write((char*)&procedureAmount, 4);

    for (unsigned int instruction_b: instructions_b) {
        of.write((char*)&instruction_b, 4);
    }

    of.close();
    std::cout << "Successfully assembled" << std::endl;
}


int getVariableLocation(std::vector<std::string> variables, std::string variableName) {
    std::vector<std::string>::iterator iter = std::find(variables.begin(),variables.end(), variableName);//返回的是一个迭代器指针
        if (iter == variables.end())
            return -1;
        else {
            return std::distance(variables.begin(), iter);
        }

    return 0;
}


void analyse(std::vector<std::string> words) {
    std::cout << "analyse: " << words[0] << std::endl;
    // analyse jump tag
    if (words[0][0] == '#') {
        struct JumpPoint jumpPoint;
        jumpPoint.tagName = words[0].substr(1);
        jumpPoint.tagLine = currentInstructionCursor;
        jumpPoints.push_back(jumpPoint);
        std::cout << jumpPoint.tagName << " " << jumpPoint.tagLine << std::endl;
        std::vector<std::string> swap;
        for (unsigned int i = 1; i < words.size(); i ++) {
            swap.push_back(words[i]);
        }
        words = swap;
    }

    // analyse config items
    if (words[0][0] == '.') {
        // bytecode version
        if (words[0] == ".version") {
            //exit(0);
            bytecodeVersion = std::stoi(words[1], 0, 10);
            
        }
        // global stack size
        else if (words[0] == ".stack") {
            globalStackSize = std::stoi(words[1], 0, 10);
        }

        else {
            showError(-1, currentLine + 1, "Assemble Error", "Invalid configuration \"" + words[0] + "\"");
        }
    }

    // analyse variable and constant
    else if (words[0][0] == '$') {
        
        struct DATA newConstant;

        if (words[1] == "i64") {
            newConstant.type = 0;
        } else if (words[1] == "f64") {
            newConstant.type = 1;
        } else if (words[1] == "i8_ptr") {
            newConstant.type = 2;
        } else if (words[1] == "i16_ptr") {
            newConstant.type = 3;
        } else if (words[1] == "i32_ptr") {
            newConstant.type = 4;
        } else if (words[1] == "i64_ptr") {
            newConstant.type = 5;
        } else if (words[1] == "f32_ptr") {
            newConstant.type = 6;
        } else if (words[1] == "f64_ptr") {
            newConstant.type = 7;
        } else {
            showError(-1, currentLine + 1, "Assemble Error", "Invalid data type \"" + words[1] + "\"");
        }

        // constant
        if (words.size() == 3) {
            if (words[2][0] == '\'') {
                newConstant.value = words[2].substr(1,words[2].length() - 2);
            } else {
                newConstant.value = words[2];
            }
            
            if (isGlobal) {
                globalConstants.push_back(newConstant);
                globalVariables.push_back(words[0].substr(1));
                globalDataSegmentSize ++;
                globalConstantAmount ++;
            } else {
                localVariables.push_back(words[0].substr(1));
                // increase local data segment size
                instructions_b[currentProcedureStartPosition] ++;
            }
        }
        
        // variable
        else if (words.size() == 2) {
            if (isGlobal) {
                globalVariables.push_back(words[0].substr(1));
                globalDataSegmentSize ++;
            } else {
                localVariables.push_back(words[0].substr(1));
                // increase local data segment size
                instructions_b[currentProcedureStartPosition] ++;
            }
        }
    }

    // analyse procedure
    else if (words[0][0] == '@') {
        isGlobal = false;
        currentProcedureStartPosition = instructions_b.size();
        instructions_b.push_back(0);  // local data segment size
        instructions_b.push_back(0);  // local constant amount
        instructions_b.push_back(0);  // local instruction amount
        currentInstructionCursor = 0;
        procedureAmount ++;
    }

    // analyse comments
    else if (words[0][0] == ';') {
        return;
    }

    else {
        // analyse instructions
        unsigned int instructionAmount = sizeof(opcodes_asm) / sizeof(*opcodes_asm);
        currentInstructionCursor ++;
        for (unsigned int i = 0; i < instructionAmount; i ++) {
            if (opcodes_asm[i] == words[0]) {
                unsigned int curInstruction =  opcodes_bin[i] << 24;

                // single-operand instruction
                if (words.size() == 2) {
                    int operand;
                    operand = isdigit(words[1][0]) ? stoi(words[1], 0, 10) : getVariableLocation(localVariables, words[1]);
                    curInstruction += operand << 16;
                }

                // two-operand instruction
                else if (words.size() == 3) {
                    int operand_1, operand_2;
                    if (words[0] == "glob") {
                        operand_1 = getVariableLocation(localVariables, words[1]);
                        operand_2 = getVariableLocation(globalVariables, words[2]);
                    }
                    
                    else {
                        operand_1 = stoi(words[1], 0, 10);
                        operand_2 = stoi(words[2], 0, 10);
                    }
                    curInstruction += operand_1 << 16;
                    curInstruction += operand_2;
                }
                
                // three-operand instruction
                else if (words.size() == 4) {
                    int operand_1, operand_2, operand_3;
                    operand_1 = isdigit(words[1][0]) ? stoi(words[1], 0, 10) : getVariableLocation(localVariables, words[1]);
                    operand_2 = isdigit(words[2][0]) ? stoi(words[2], 0, 10) : getVariableLocation(localVariables, words[2]);
                    operand_3 = isdigit(words[3][0]) ? stoi(words[3], 0, 10) : getVariableLocation(localVariables, words[3]);
                    curInstruction += operand_1 << 16;
                    curInstruction += operand_2 << 8;
                    curInstruction += operand_3;
                }
                printf("Ins: %llx\n", curInstruction);
                instructions_b.push_back(curInstruction);

                // increase amount of instruction amount
                instructions_b[currentProcedureStartPosition + 2] ++;
                break;
            }
        }
    }
}


void Assembler(std::vector<std::string> asmLines) {
    for (auto line: asmLines) {
        if (line == "") {
            currentLine ++;
            continue;
        }
        //std::cout << "*" << line << "*" << std::endl;
        //std::cout << (int)line[line.length() - 1] << std::endl;
        std::vector<std::string> words;
        unsigned int curWordStart = 0;
        bool readingString = false;
        for (unsigned int i = 0; i < line.length(); i ++) {
            if (line[i] == '\'') {
                readingString = !readingString;
            }
            if (line[i + 1] == ' ' && readingString == false) {
                words.push_back(line.substr(curWordStart, i - curWordStart + 1));
                curWordStart = i + 2;
            } else if (i + 1 == line.length()) {
                words.push_back(line.substr(curWordStart, i - curWordStart + 1));
            }
        }
        for (auto word: words) {
            //std::cout << word << " + ";
        }
        //std::cout << std::endl;
        analyse(words);
        currentLine ++;
    }
    writeExecutableFile();
    for (auto variable: localVariables) {
        //std::cout << variable << std::endl;
    }
}