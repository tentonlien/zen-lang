/* Tenton Lien */
/* 9/27/2018 */
/* ZASM has been rewritten in C++ */
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "zasm.h"

using namespace std;
using namespace zasm;

namespace zasm {
    struct var {
        string name;
        string type;
        unsigned long long value;
    } variable;
    vector<string> lines;
}

vector<struct var> variables;
vector<struct ins> instructions_c;
vector<unsigned long long> instructions_b;
vector<pair<string, int>>labels;

unsigned int dataStart, dataEnd, codeStart;

struct ins {
    string opcode;
    string destinationOperand;
    string sourceOperand;
    int fromWhichLine;
} instruction;

unsigned char opcodes_bin[] = {
    0x00, 0x01, 0x02, 0x03, 
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x20,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39
};

string opcodes_asm[] = {
    "mov", "xchg", "push", "pop",
    "add", "inc", "sub", "dec", "cmp", "mul", "div",
    "and",
    "int", "loop", "jmp", "je", "jne", "jg", "jl", "jge", "jle", "ret"
};

int ins_operand_amount[] = {
    2, 2, 1, 1,
    2, 1, 2, 1, 2, 2, 2,
    2,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

string registers[] = {"ax", "bx", "cx", "dx"};

void ShowError(int lineNumber, string errorInfo) {
    if (lineNumber == 0) {
        cout << "Error: " << errorInfo << endl;
    } else {
        cout << "Error at Line " << lineNumber << ": " << errorInfo << endl;
    }
    exit(0);
}


string trim(string str) {
    int pos;  // pos cannot be unsigned int here!
	while ((pos = str.find(" ")) != string::npos) {
		str.replace(pos, 1, "");
	}
    return str;
}


void DataSectionParser() {
    for (unsigned int i = dataStart; i < dataEnd; i ++) {
        if (lines.at(i) == "" || trim(lines.at(i)) == "" || trim(lines.at(i))[0] == ';') {
            continue;
        }

        bool readName = true;
        bool readType = true;
        bool readValue = true;
        unsigned int k = 0;
        
        // Read variable name
        string name = "";
        while (readName) {
            if (k >= lines.at(i).length()) {
                readName = false;
                break;
            }

            if (isalpha(lines.at(i)[k])) {
                while (lines.at(i)[k] != ' ') {
                    name += lines.at(i)[k];
                    k ++;
                }
                readName = false;
            }

            k ++;
        }

        // Read variable type
        string type = "";
        while (readType) {
            if (k >= lines.at(i).length()) {
                readType = false;
                ShowError(i + 1, "Missing type for varialble '" + variable.name + "'");
                break;
            }

            if (isalpha(lines.at(i)[k])) {
                while (
                    k < lines.at(i).length() && 
                    lines.at(i)[k] != ' ' &&
                    lines.at(i)[k] != ';'
                ) {
                    type += lines.at(i)[k];
                    k ++;
                }
                readType = false;
            }

            k ++;
        }

        // Read variable value
        string value = "";
        size_t st = 0;
        while (readValue) {
            if (k >= lines.at(i).length()) {
                readValue = false;
                ShowError(i + 1, "Missing value for variable '" + variable.name + "'");
                break;
            }
            
            if (isalnum(lines.at(i)[k])) {
                
                while (
                    k < lines.at(i).length() &&
                    lines.at(i)[k] != 0 &&
                    lines.at(i)[k] != ';'
                ) {
                    value += lines.at(i)[k];
                    k ++;
                }
                readValue = false;
                variable.value = stoull(value, &st, 0);

            } else if (lines.at(i)[k] == '?') {
                value = "0";
                readValue = false;
                variable.value = stoull(value, &st, 0);

            } else if (lines.at(i)[k] == '\'') {
                k ++;
                while (
                    k < lines.at(i).length() &&
                    lines.at(i)[k] != 0 &&
                    lines.at(i)[k] != '\''
                ) {
                    value += lines.at(i)[k];
                    k ++;
                }

                if (value.length() == 1) {
                    variable.value = value[0];
                }
                
                readValue = false;
            }

            k ++;
        }

        variable.name = name;
        variable.type = type;


        // Check redefined varaibles
        bool redefined = false;
        for (unsigned int k = 0; k < variables.size(); k ++) {
            if (variables.at(k).name == variable.name) {
                redefined = true;
                break;
            }
        }
        if (redefined) {
            ShowError(i + 1, "Redefined variable '" + variable.name + "'");
        }

        // Check if variable name conflicts with registers
        if (find(registers, registers + sizeof(registers) / sizeof(*registers), variable.name) - registers != sizeof(registers) / sizeof(*registers)) {
            ShowError(i + 1, "Illegal variable name '" + variable.name + "'");
        }

        variables.push_back(variable);
    }
}


void CodeSectionParser() {
    for (unsigned int i = codeStart; i < lines.size(); i ++) {
        unsigned int k = 0;

        if (lines.at(i) == "" || trim(lines.at(i)) == "" || trim(lines.at(i))[0] == ';') {
            continue;
        } else if (lines.at(i)[0] == '@') {
            string labelName = "";
            k ++;
            while(lines.at(i)[k] != ':') {
                labelName += lines.at(i)[k];
                k ++;
            }
            labels.push_back({labelName, i - codeStart});
        }
        
        bool readOpcode = true;
        bool readDestinationOperand = true;
        bool readSourceOperand = true;

        // Read opcode
        string opcode = "";
        while (readOpcode) {
            if ( k >= lines.at(i).length()) {
                readOpcode = false;
                break;
            }

            if (isalpha(lines.at(i)[k])) {
                while (lines.at(i)[k] != ' ' && lines.at(i)[k] != '\n') {
                    opcode += lines.at(i)[k];
                    k ++;
                }
                readOpcode = false;
            }
            k ++;
        }

        for (unsigned int k = 0; k < sizeof(opcodes_asm) / sizeof(opcodes_asm[0]); k ++) {
            if (opcodes_asm[k] == opcode) {
                if (ins_operand_amount[k] == 1) {
                    readSourceOperand = false;
                }
                break;
            } else if (k == sizeof(opcodes_asm) / sizeof (opcodes_asm[0]) - 1) {
                ShowError(i + 1, "Invalid opcode: " + opcode);
            }
        }

        // Read destination operand
        string destinationOperand = "";
        while (readDestinationOperand) {
            if (k >= lines.at(i).length()) {
                readDestinationOperand = false;
                break;
            }

            if (isalnum(lines.at(i)[k])) {
                while (k < lines.at(i).length() && lines.at(i)[k] != ' ' && lines.at(i)[k] != ',' && lines.at(i)[k] != ';') {
                    destinationOperand += lines.at(i)[k];
                    k ++;
                }
                readDestinationOperand = false;
            } 
            k ++;
        }

        // Read source operand
        string sourceOperand = "";
        while (readSourceOperand) {
            if (k >= lines.at(i).length()) {
                readSourceOperand = false;
                break;
            }

            if (isalnum(lines.at(i)[k])) {
                while (k < lines.at(i).length() && lines.at(i)[k] != ' ' && lines.at(i)[k] != ';') {
                    sourceOperand += lines.at(i)[k];
                    k ++;
                }
                readSourceOperand = false;
            }
            k ++;
        }

        instruction.opcode = opcode;
        instruction.destinationOperand = destinationOperand;
        instruction.sourceOperand = sourceOperand;
        instruction.fromWhichLine = i + 1;
        instructions_c.push_back(instruction);
    }
}


bool isMemory(string variableName) {
    for (unsigned int i = 0; i < variables.size(); i ++) {
        if (variables.at(i).name == variableName) {
            return true;
        }
    }
    return false;
}


void InstructionEncoder() {
    for (unsigned int i = 0; i < instructions_c.size(); i ++) {
        unsigned int instruction_b;
        int addressing = 255;
        int destinationOperand;
        int sourceOperand = 0;

        // Encode opcode
        unsigned char opcode_b;
        for (unsigned int k = 0; k < sizeof(opcodes_asm) / sizeof(opcodes_asm[0]); k ++) {
            if (opcodes_asm[k] == instructions_c.at(i).opcode) {
                opcode_b = opcodes_bin[k];
                break;
            } else if (k == sizeof(opcodes_asm) / sizeof(opcodes_asm[0]) - 1) {
                // Todo: Error Handling
            }
        }
        instruction_b = opcode_b << 26;

        // Encode addressing and operands
        if (instructions_c.at(i).sourceOperand == "") {
            // Immediate only
            if (isdigit(instructions_c.at(i).destinationOperand[0])) {
                addressing = 0;
                instruction_b += addressing << 24;

                if (instructions_c.at(i).destinationOperand[instructions_c.at(i).destinationOperand.length() - 1] == 'h') {
                    instructions_c.at(i).destinationOperand[instructions_c.at(i).destinationOperand.length() - 1] = 0;
                    destinationOperand = stoi(instructions_c.at(i).destinationOperand, nullptr, 16);
                    
                } else {
                    size_t st;
                    destinationOperand = stoi(instructions_c.at(i).destinationOperand, &st);
                }
                instruction_b += destinationOperand;
            }

            // Register only
            else if (find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).destinationOperand) - registers != sizeof(registers) / sizeof(*registers)) {
                addressing = 1;
                instruction_b += addressing << 24;
                destinationOperand = find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).destinationOperand) - registers;
                instruction_b += destinationOperand;
            }

            // Memory only
            else if (isMemory(instructions_c.at(i).destinationOperand)) {
                addressing = 1;
                instruction_b += addressing << 24;
                for (unsigned int k = 0; k < variables.size(); k ++) {
                    if (variables.at(k).name == instructions_c.at(i).destinationOperand) {
                        destinationOperand = k + 6;
                        break;
                    }
                }
                instruction_b += destinationOperand;
            }

            else {
                // label
                for (int m = 0; m < labels.size(); m ++) {
                    if (instructions_c.at(i).destinationOperand == labels.at(m).first) {
                        instruction_b += labels.at(m).second;
                        break;
                    } else if (m == labels.size() - 1) {
                        ShowError(instructions_c.at(i).fromWhichLine, "Invalid destination operand in single operand mode.");
                    }
                }
            }
        }

        else {
            if (find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).destinationOperand) - registers != sizeof(registers) / sizeof(*registers)) {
                // Register - immediate
                if (isdigit(instructions_c.at(i).sourceOperand[0])) {
                    addressing = 2;
                    instruction_b += addressing << 24;
                    
                    destinationOperand = find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).destinationOperand) - registers;
                    
                    instruction_b += destinationOperand << 12;

                    if (instructions_c.at(i).sourceOperand[instructions_c.at(i).sourceOperand.length() - 1] == 'h') {
                        instructions_c.at(i).sourceOperand[instructions_c.at(i).sourceOperand.length() - 1] = 0;
                        sourceOperand = stoi(instructions_c.at(i).sourceOperand, nullptr, 16);
                    } else {
                        size_t st;
                        sourceOperand = stoi(instructions_c.at(i).sourceOperand, &st);
                    }

                    instruction_b += sourceOperand;
                }

                // Register - register
                else if (find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).sourceOperand) - registers != sizeof(registers) / sizeof(*registers)) {
                    addressing = 3;
                    instruction_b += addressing << 24;
                    destinationOperand = find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).destinationOperand) - registers;
                    instruction_b += destinationOperand << 12;
                    sourceOperand = find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).sourceOperand) - registers;
                    instruction_b += sourceOperand;
                }

                // Register - memory
                else if (isMemory(instructions_c.at(i).sourceOperand)) {
                    addressing = 3;
                    instruction_b += addressing << 24;
                    destinationOperand = find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).destinationOperand) - registers;
                    instruction_b += destinationOperand << 12;

                    for (unsigned int k = 0; k < variables.size(); k ++) {
                        if (variables.at(k).name == instructions_c.at(i).sourceOperand) {
                            sourceOperand = k + 6;
                            break;
                        }
                    }
                    instruction_b += sourceOperand;
                }
                else {
                    ShowError(instructions_c.at(i).fromWhichLine, "Undefined variable '" + instructions_c.at(i).sourceOperand + "'");
                }
            }
            
            else if (isMemory(instructions_c.at(i).destinationOperand)) {
                // Memory - immediate
                if (isdigit(instructions_c.at(i).sourceOperand[0])) {
                    addressing = 2;
                    instruction_b += addressing << 24;

                    for (unsigned int k = 0; k < variables.size(); k ++) {
                        if (variables.at(k).name == instructions_c.at(i).destinationOperand) {
                            destinationOperand = k + 6;
                            break;
                        }
                    }

                    instruction_b += destinationOperand << 12;

                    if (instructions_c.at(i).sourceOperand[instructions_c.at(i).sourceOperand.length() - 1] == 'h') {
                        instructions_c.at(i).sourceOperand[instructions_c.at(i).sourceOperand.length() - 1] = 0;
                        sourceOperand = stoi(instructions_c.at(i).sourceOperand, nullptr, 16);
                    } else {
                        size_t st;
                        sourceOperand = stoi(instructions_c.at(i).sourceOperand, &st);
                    }

                    instruction_b += sourceOperand;
                }

                // Memory - register
                else if (find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).sourceOperand) - registers != sizeof(registers) / sizeof(*registers)) {
                    addressing = 3;
                    instruction_b += addressing << 24;

                    for (unsigned int k = 0; k < variables.size(); k ++) {
                        if (variables.at(k).name == instructions_c.at(i).destinationOperand) {
                            destinationOperand = k + 6;
                            break;
                        }
                    }

                    instruction_b += destinationOperand << 12;

                    sourceOperand = find(registers, registers + sizeof(registers) / sizeof(*registers), instructions_c.at(i).sourceOperand) - registers;
                    instruction_b += sourceOperand;
                }

                // Memory - memory
                else if (isMemory(instructions_c.at(i).sourceOperand)) {
                    addressing = 3;
                    instruction_b += addressing << 24;

                    for (unsigned int k = 0; k < variables.size(); k ++) {
                        if (variables.at(k).name == instructions_c.at(i).destinationOperand) {
                            destinationOperand = k + 6;
                            break;
                        }
                    }

                    instruction_b += destinationOperand << 12;

                    for (unsigned int k = 0; k < variables.size(); k ++) {
                        if (variables.at(k).name == instructions_c.at(i).sourceOperand) {
                            sourceOperand = k + 6;
                            break;
                        }
                    }

                    instruction_b += sourceOperand;
                }

                else {
                    ShowError(instructions_c.at(i).fromWhichLine, "Invalid source operand  in memory - ? mode.");
                }
            } else {
                ShowError(instructions_c.at(i).fromWhichLine, "Invalid destination operand. The assembler cannot tell the operand mode.");
            }
        }

        instructions_b.push_back(instruction_b);
    }
}

void ExecutableFileWrite(string asmFileName) {
    int pos = asmFileName.find(".asm");
    string output = asmFileName.substr(0, pos) + ".zef";
    ofstream of(output, ios::binary);
    if (!of) {
        ShowError(0, "Write executable file failed");
    }
    unsigned char magicNumber[] = {0x5A, 0x45, 0x4E, 0x21};
    unsigned char bytecodeVersion[] = {0x01, 0x00, 0x00, 0x00}; 
    unsigned int stackSize = 100;
    unsigned int dataSegmentSize = variables.size() + 6;
    unsigned int constantAmount = variables.size();
    unsigned int instructionSegmentSize = instructions_b.size();

    of.write((char*)magicNumber, 4);
    of.write((char*)bytecodeVersion, 4);

    // Write data section
    of.write((char*)&stackSize, 4);
    of.write((char*)&dataSegmentSize, 2);
    of.write((char*)&constantAmount, 2);

    for (unsigned int i = 0; i < variables.size(); i ++) {
        of.write((char*)&variables.at(i).value, 8);
    }

    // Write code section
    of.write((char*)&instructionSegmentSize, 4);

    for (unsigned int i = 0; i < instructionSegmentSize; i ++) {
        of.write((char*)&instructions_b.at(i), 4);
    }

    of.close();
}


void Initializer(string asmFileName) {
    // Scan the boundary of data section and code section
    for (unsigned int i = 0; i < lines.size(); i ++) {
        vector<string> words;
        string line = lines.at(i);
        int pos = 0;

        // Split the words of each line
        for (unsigned int k = 0; k < line.length(); k ++) {
            if (line[k] == ' ') {
                if (k == pos) {
                    pos ++;
                    cout << pos << " ";
                } else {
                    words.push_back(line.substr(pos, k - pos));
                    pos = k + 1;
                }
            } else if (k == line.length() - 1) {
                words.push_back(line.substr(pos, k - pos + 1));
            }
        }
        
        int wordsAmount = words.size();

        // Find data section
        if ((wordsAmount == 2 || (wordsAmount >= 3 && words.at(2)[0] == ';')) &&
            words.at(0) == "section" && words.at(1) == ".data"
        ) {
            dataStart = i + 1;
            
        }

        // Find code section
        else if ((wordsAmount == 2 || (wordsAmount >= 3 && words.at(2)[0] == ';')) &&
            words.at(0) == "section" && words.at(1) == ".code"
        ) {
            dataEnd = i;
            codeStart = i + 1;
        }
    }
    
    DataSectionParser();
    CodeSectionParser();
    InstructionEncoder();
    ExecutableFileWrite(asmFileName);
}