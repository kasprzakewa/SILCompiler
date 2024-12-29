#include "ast_nodes.hpp"

vector<IdentifierNode*> nodes;
vector<string> tac;

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