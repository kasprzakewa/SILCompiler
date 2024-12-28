#include "symbol_table.hpp"

std::vector<Symbol> symbolTable;
int last_symbol = 0;

int find_symbol(std::string name) {
    for (int i = last_symbol - 1; i >= 0; i--) {
        if (symbolTable[i].name == name) {
            return i;
        }
    }
    return -1;
}

void add_symbol(std::string name) {
    Symbol s;
    s.name = name;
    symbolTable.push_back(s);
    last_symbol++;
}

void initialize_symbol(int id) {
    if (id >= 0 && id < last_symbol) {
        symbolTable[id].initialized = true;
    }
}

bool is_initialized(int id) {
    if (id >= 0 && id < last_symbol) {
        return symbolTable[id].initialized;
    }
    return false;
}

void print_symbol_table() {
    std::cout << "Symbol Table:" << std::endl;
    for (const auto& symbol : symbolTable) {
        std::cout << symbol.name << "; ";

        if (symbol.initialized) {
            std::cout << "initialized" << std::endl;
        } else {
            std::cout << "uninitialized" << std::endl;
        }
    }
}
