#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <iostream>
#include <string>
#include <vector>

struct Symbol {
    bool initialized = false;
    std::string name;
};

extern int last_symbol;

int find_symbol(std::string name);
void add_symbol(std::string name);
void initialize_symbol(int id);
bool is_initialized(int id);
void print_symbol_table();

#endif
