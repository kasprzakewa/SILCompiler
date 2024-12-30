#include "ast_nodes.hpp"

vector<IdentifierNode*> nodes;
vector<string> tac;
vector<TAC> tac2;
int temp_counter = 0;
int label_counter = 0;

string increase_temp_counter() {
    return "t" + to_string(temp_counter++);
}

string increase_label_counter() {
    return "L" + to_string(label_counter++);
}

string get_label(int offset) {
    return "L" + to_string(label_counter - offset);
}

void add_node(IdentifierNode* node) {
    nodes.push_back(node);
}

IdentifierNode* find_node(string name) {
    for (IdentifierNode* node : nodes) {
        if (node->name == name) {
            return node;
        }
    }
    return nullptr;
}

void initialize_node(IdentifierNode* node) {
    node->initialized = true;
}

void print_nodes() {
    for (IdentifierNode* node : nodes) {
        cout << node->name << endl;
    }
}

void add_to_tac(string line) {
    tac.push_back(line);
}

void print_tac() {
    for (string line : tac) {
        cout << line << endl;
    }
}

void add_to_tac2(string op, string result, string arg1, string arg2) {
    tac2.emplace_back(op, result, arg1, arg2);
}

void print_tac2() {
    for (TAC line : tac2) {
        cout << line.op << " " << line.result << " " << line.arg1 << " " << line.arg2 << endl;
    }
}