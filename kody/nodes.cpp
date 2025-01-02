#include "nodes.hpp"

int first_free_register = 0;
vector<ValueNode*> memory;
vector<string> assembly;

void add_to_memory(ValueNode* value) {
    memory.push_back(value);
}

ValueNode* find_node(const string &name) {
    for (auto value : memory) {
        if (value->name == name) {
            return value;
        }
    }
    return nullptr;
}

void print_assembly() {
    int i = 0;
    for (auto line : assembly) {
        cout << ++i << " " << line << endl;
    }
}

void print_memory() {
    for (auto value : memory) {
        cout << "P" << value->register_number << ": " << value->name << endl;
    }
}

// ValueNode**********************************************************************************************************
ValueNode::ValueNode(const string &name, bool initialized, int len) : name(name), initialized(initialized), len(len) {
    register_number = ++first_free_register;
}

void ValueNode::initialize() {
    initialized = true;
}

void ValueNode::translate() {
    if (len == 2) {
        assembly.push_back("SET " + name);
        assembly.push_back("STORE " + to_string(register_number));
    }
}

// ConditionNode******************************************************************************************************
ConditionNode::ConditionNode(const string &op, ValueNode* x, ValueNode* y, bool is_negated) : op(op), x(x), y(y), is_negated(is_negated) {
    len = 2;
}

ConditionNode::~ConditionNode() {
    delete x;
    delete y;
}

void ConditionNode::translate() {
    assembly.push_back("LOAD " + to_string(x->register_number));
    assembly.push_back("SUB " + to_string(y->register_number));
}

// AssignNode********************************************************************************************************
AssignNode::AssignNode(const string &op, ValueNode* x, ValueNode* y, ValueNode* z) : op(op), x(x), y(y), z(z) {
    if (op == "ASSIGN") {
        len = 2;
    } else {
        len = 3;
    }
}

AssignNode::~AssignNode() {
    delete x;
    delete y;
    delete z;
}

void AssignNode::set_x(ValueNode* x) {
    this->x = x;
}

void AssignNode::translate() {
    if (op == "ASSIGN") {
        assembly.push_back("LOAD " + to_string(y->register_number));
        assembly.push_back("STORE " + to_string(x->register_number));
    } else {
        assembly.push_back("LOAD " + to_string(y->register_number));
        assembly.push_back(op + " " + to_string(z->register_number));
        assembly.push_back("STORE " + to_string(x->register_number));
    }
}

// IfNode*************************************************************************************************************
IfNode::IfNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {
    len = 2 + commands->len + 2 - condition->is_negated ? 1 : 0;
}

IfNode::~IfNode() {
    delete condition;
    delete commands;
}

void IfNode::translate() {
    condition->translate();
    if (condition->is_negated) {
        assembly.push_back(condition->op + " " + to_string(commands->len + 1));
    } else {
        assembly.push_back(condition->op + " " + to_string(2));
        assembly.push_back("JUMP " + to_string(commands->len + 1));
    }
    commands->translate();
}

// IfElseNode*********************************************************************************************************
IfElseNode::IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands) : condition(condition), if_commands(if_commands), else_commands(else_commands) {
    len = 4 + if_commands->len + else_commands->len;
}

IfElseNode::~IfElseNode() {
    delete condition;
    delete if_commands;
    delete else_commands;
}

void IfElseNode::translate() {
    condition->translate();
    if (condition->is_negated) {
        assembly.push_back(condition->op + " " + to_string(if_commands->len + 1));
        if_commands->translate();
        assembly.push_back("JUMP " + to_string(else_commands->len + 1));
        else_commands->translate();
    } else {
        assembly.push_back(condition->op + " " + to_string(else_commands->len + 1));
        else_commands->translate();
        assembly.push_back("JUMP " + to_string(if_commands->len + 1));
        if_commands->translate();
    }
}

// WhileNode**********************************************************************************************************
WhileNode::WhileNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {
    len = commands->len + 5 - condition->is_negated ? 1 : 0;
}

WhileNode::~WhileNode() {
    delete condition;
    delete commands;
}

void WhileNode::translate() {
    condition->translate();
    if (condition->is_negated) {
        assembly.push_back(condition->op + " " + to_string(commands->len + 2));
    } else {
        assembly.push_back(condition->op + " " + to_string(2));
        assembly.push_back("JUMP " + to_string(commands->len + 2));
    }
    commands->translate();
    assembly.push_back("JUMP -" + to_string(len));
}

// RepeatNode*********************************************************************************************************
RepeatNode::RepeatNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {
    len = commands->len + 3 + condition->is_negated ? 1 : 0;
}

RepeatNode::~RepeatNode() {
    delete condition;
    delete commands;
}

void RepeatNode::translate() {
    commands->translate();
    condition->translate();
    if (condition->is_negated) {
        assembly.push_back(condition->op + " -" + to_string(len));
    } else {
        assembly.push_back(condition->op + " " + to_string(2));
        assembly.push_back("JUMP -" + to_string(len));
    }
}

// ForNode************************************************************************************************************
ForNode::ForNode(ValueNode* i, ValueNode* start, ValueNode* end, ValueNode* one, CommandsNode* commands, const string &op) : i(i), start(start), end(end), one(one), commands(commands), op(op) {
    len = commands->len + 8;
}

ForNode::~ForNode() {
    delete i;
    delete start;
    delete end;
    delete commands;
}

void ForNode::translate() {
    assembly.push_back("LOAD " + to_string(start->register_number));
    assembly.push_back("STORE " + to_string(i->register_number));
    assembly.push_back("LOAD " + to_string(i->register_number));
    assembly.push_back("SUB " + to_string(end->register_number));
    assembly.push_back(op + " " + to_string(commands->len + 4));
    commands->translate();
    assembly.push_back("LOAD " + to_string(i->register_number));
    
    if (op == "JPOS") {
        assembly.push_back("ADD " + to_string(one->register_number));
    } else {
        assembly.push_back("SUB " + to_string(one->register_number));
    }
    assembly.push_back("JUMP -" + to_string(len - 1));
}

// IONode*************************************************************************************************************
IONode::IONode(const string &op, ValueNode* x) : op(op), x(x) {
    len = 1;
}

IONode::~IONode() {
    delete x;
}

void IONode::translate() {
    assembly.push_back(op + " " + to_string(x->register_number));
}

// CommandsNode*******************************************************************************************************
CommandsNode::CommandsNode() {
    len = 0;
}

CommandsNode::~CommandsNode() {
    for (auto command : commands) {
        delete command;
    }
}

void CommandsNode::add_child(CommandNode* command) {
    commands.push_back(command);
    len += command->len;
}

void CommandsNode::translate() {
    for (auto command : commands) {
        command->translate();
    }
}

// MainNode**********************************************************************************************************
MainNode::MainNode(CommandsNode* commands, bool declarations) : commands(commands), declarations(declarations) {}

MainNode::~MainNode() {
    delete commands;
}

void MainNode::translate() {
    if (declarations) {
        for (auto value : memory) {
            value->translate();
        }
    }
    commands->translate();
    assembly.push_back("HALT");
}
