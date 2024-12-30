#ifndef AST_NODES_HPP
#define AST_NODES_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

struct TAC {
    string op;
    string result;
    string arg1;
    string arg2;
    

    TAC(string op, string result = "", string arg1 = "", string arg2 = "")
        : op(op), result(result), arg1(arg1), arg2(arg2) {}
};

extern vector<TAC> tac;
void add_to_tac(string op, string result = "", string arg1 = "", string arg2 = "");
void print_tac();

extern int temp_counter;
string increase_temp_counter();

extern int label_counter;
string increase_label_counter();
string get_label(int offset);  

class ASTNode {   
public:
    string name;
    bool initialized;
    virtual ~ASTNode() {}
    virtual void translate() {}
    virtual pair<bool, string> is_initialized() { return {initialized, name}; }
    virtual string get_name() { return name; }
};

class IdentifierNode : public ASTNode {
public:

    IdentifierNode(const string &name, bool initialized) {
        this->name = name;
        this->initialized = initialized;
    }

    IdentifierNode(ASTNode* node) {
        this->name = node->get_name();
        this->initialized = node->is_initialized().first;
    }

    void translate() override {
        cout << name;
    }
};

extern vector<IdentifierNode*> nodes;

void add_node(IdentifierNode* node);
IdentifierNode* find_node(string name);
void print_nodes();
void initialize_node(IdentifierNode* node);

class ValueNode : public ASTNode {
public:
    bool number = false;
    bool initialized = false;

    ValueNode(const string &name) {
        this->name = name;
        this->number = true;
        this->initialized = true;
    }

    ValueNode(IdentifierNode* identifier) {
        this->name = identifier->get_name();
        this->number = false;
        this->initialized = identifier->is_initialized().first;
    }

    bool is_number() {
        return number;
    }

    pair<bool, string> is_initialized() { 
        return {initialized, name}; 
    }

    void translate() override {
        cout << name;
    }
};

class ExpressionNode : public ASTNode {
public:
    ValueNode* left;
    ValueNode* right;
    string op;
    ValueNode* temp;

    ExpressionNode(ValueNode* left, ValueNode* right, const string &op) : left(left), right(right), op(op) {
        this->initialized = left->is_initialized().first && right->is_initialized().first;
        temp = new ValueNode(increase_temp_counter());
    }

    ExpressionNode(ValueNode* left) {
        this->left = left;
        this->right = nullptr;
        this->op = "";
        this->name = left->get_name();
        this->initialized = left->is_initialized().first;
        this->temp = nullptr;
    }

    ~ExpressionNode() {
        delete left;
        delete right;
    }

    bool is_number() {
        return left->is_number() && right->is_number();
    }

    void translate() override {
        if (!(right == nullptr && left != nullptr)) {
            add_to_tac(op, temp->get_name(), left->get_name(), right->get_name());
        }
    }
};

class ConditionNode : public ASTNode {
public:
    ValueNode* left;
    ValueNode* right;
    string op;
    ValueNode* temp;

    ConditionNode(ValueNode* left, ValueNode* right, const string &op) : left(left), right(right), op(op) {
        temp = new ValueNode(increase_temp_counter());
    }

    ~ConditionNode() {
        delete left;
        delete right;
    }

    void translate() override {
        add_to_tac(op, temp->get_name(), left->get_name(), right->get_name());
    }
};

class DeclarationsNode : public ASTNode {
public:
    vector<IdentifierNode*> identifiers;

    ~DeclarationsNode() {
        for (IdentifierNode* identifier : identifiers) {
            delete identifier;
        }
    }

    void add_child(IdentifierNode* node){
        identifiers.push_back(node);
    }

    void translate() override {
        for (IdentifierNode* identifier : identifiers) {
            add_to_tac("DECLARE", identifier->get_name());
        }
    }
};

class CommandNode : public ASTNode {
public:
    virtual ~CommandNode() {}
};

class CommandsNode : public ASTNode {
public:
    vector<CommandNode*> commands;

    ~CommandsNode() {
        for (CommandNode* command : commands) {
            delete command;
        }
    }

    void add_child(CommandNode* node){
        commands.push_back(node);
    }

    void translate() override {
        for (CommandNode* command : commands) {
            command->translate();
        }
    }
};

class AssignNode : public CommandNode{
public:
    IdentifierNode* identifier;
    ExpressionNode* expression;

    AssignNode(IdentifierNode* identifier, ExpressionNode* expression) : identifier(identifier), expression(expression) {}

    ~AssignNode() {
        delete identifier;
        delete expression;
    }

    void translate() override {
        if (expression->temp != nullptr) {
            expression->translate();
            add_to_tac("ASSIGN", identifier->get_name(), expression->temp->get_name());
        } else {
            add_to_tac("ASSIGN", identifier->get_name(), expression->get_name());
        } 
    }
};

class IfNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;
    string label;

    IfNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {
        label = increase_label_counter();
    }

    ~IfNode() {
        delete condition;
        delete commands;
    }

    void translate() override {
        condition->translate();
        add_to_tac("IF", condition->temp->get_name(), label);
        commands->translate();
        add_to_tac("LABEL", label);
    }
};

class IfElseNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* if_commands;
    CommandsNode* else_commands;
    string label1;
    string label2;

    IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands) : condition(condition), if_commands(if_commands), else_commands(else_commands) {
        label1 = increase_label_counter();
        label2 = increase_label_counter();
    }

    ~IfElseNode() {
        delete condition;
        delete if_commands;
        delete else_commands;
    }

    void translate() override {
        condition->translate();
        add_to_tac("IF", condition->temp->get_name(), label1);
        if_commands->translate();
        add_to_tac("GOTO", label2);
        add_to_tac("LABEL", label1);
        else_commands->translate();
        add_to_tac("LABEL", label2);
    }
};

class WhileNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;
    string label1;
    string label2;

    WhileNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {
        label1 = increase_label_counter();
        label2 = increase_label_counter();
    }

    ~WhileNode() {
        delete condition;
        delete commands;
    }

    void translate() override {
        add_to_tac("LABEL", label1);
        condition->translate();
        add_to_tac("IF", condition->temp->get_name(), label2);
        commands->translate();
        add_to_tac("GOTO", label1);
        add_to_tac("LABEL", label2);
    }
};

class RepeatUntilNode : public CommandNode {
public:
    CommandsNode* commands;
    ConditionNode* condition;
    string label1;
    string label2;

    RepeatUntilNode(CommandsNode* commands, ConditionNode* condition) : commands(commands), condition(condition) {
        label1 = increase_label_counter();
        label2 = increase_label_counter();
    }

    ~RepeatUntilNode() {
        delete commands;
        delete condition;
    }

    void translate() override {
        add_to_tac("LABEL", label1);
        commands->translate();
        condition->translate();
        add_to_tac("IF", condition->temp->get_name(), label2);
        add_to_tac("GOTO", label1);
        add_to_tac("LABEL", label2);
    }
};

class ForToNode : public CommandNode {
public:
    IdentifierNode* identifier;
    ValueNode* start;
    ValueNode* end;
    CommandsNode* commands;
    ConditionNode* condition;
    string label1;
    string label2;

    ForToNode(IdentifierNode* identifier, ValueNode* start, ValueNode* end, CommandsNode* commands) : identifier(identifier), start(start), end(end), commands(commands) {
        CommandNode* increase_pid = new AssignNode(identifier, new ExpressionNode(new ValueNode(identifier), new ValueNode("1"), "ADD"));
        commands->add_child(increase_pid);
        condition = new ConditionNode(new ValueNode(identifier), end, "LE");
        label1 = increase_label_counter();
        label2 = increase_label_counter();
    }

    ~ForToNode() {
        delete start;
        delete end;
        delete commands;
    }

    void translate() override {
        add_to_tac("ASSIGN", identifier->get_name(), start->get_name());
        add_to_tac("LABEL", label1);
        condition->translate();
        add_to_tac("IF", condition->temp->get_name(), label2);
        commands->translate();
        add_to_tac("GOTO", label1);
        add_to_tac("LABEL", label2);
    }
};

class ForDownToNode : public CommandNode {
public:
    IdentifierNode* identifier;
    ValueNode* start;
    ValueNode* end;
    CommandsNode* commands;
    ConditionNode* condition;
    string label1;
    string label2;

    ForDownToNode(IdentifierNode* identifier, ValueNode* start, ValueNode* end, CommandsNode* commands) : identifier(identifier), start(start), end(end), commands(commands) {
        CommandNode* decrease_pid = new AssignNode(identifier, new ExpressionNode(new ValueNode(identifier), new ValueNode("1"), "SUB"));
        commands->add_child(decrease_pid);
        condition = new ConditionNode(new ValueNode(identifier), end, "GE");
        label1 = increase_label_counter();
        label2 = increase_label_counter();
    }

    ~ForDownToNode() {
        delete start;
        delete end;
        delete commands;
    }

    void translate() override {
        add_to_tac("ASSIGN", identifier->get_name(), start->get_name());
        add_to_tac("LABEL", label1);
        condition->translate();
        add_to_tac("IF", condition->temp->get_name(), label2);
        commands->translate();
        add_to_tac("GOTO", label1);
        add_to_tac("LABEL", label2);
    }
};

class ReadNode : public CommandNode {
public:
    IdentifierNode* identifier;

    ReadNode(IdentifierNode* identifier) : identifier(identifier) {}

    void translate() override {
        add_to_tac("READ", identifier->get_name());
    }
};

class WriteNode : public CommandNode {
public:
    ValueNode* value;

    WriteNode(ValueNode* value) : value(value) {}

    ~WriteNode() {
        delete value;
    }

    void translate() override {
        add_to_tac("WRITE", value->get_name());
    }
};

class MainNode : public ASTNode {
public:
    CommandsNode* commands;

    MainNode(CommandsNode* commands) : commands(commands) {}

    ~MainNode() {
        delete commands;
    }

    void translate() override {
    }
};

class MainDecNode : public ASTNode {
public:
    DeclarationsNode* declarations;
    CommandsNode* commands;

    MainDecNode(DeclarationsNode* declarations, CommandsNode* commands) : declarations(declarations), commands(commands) {}

    ~MainDecNode() {
        delete declarations;
        delete commands;
    }

    void translate() override {
        declarations->translate();
        for(CommandNode* command : commands->commands) {
            command->translate();
        }

    }
};

#endif
