#ifndef AST_NODES_HPP
#define AST_NODES_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

extern vector<string> tac;
void add_to_tac(string line);
void print_tac();

class ASTNode {   
public:
    string name;
    bool initialized;
    virtual ~ASTNode() {}
    virtual void print() = 0;
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

    void print() override {
        cout << name;
    }

    void translate() override {
        add_to_tac("identifier: " + name);
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

    void print() override {
        cout << name;
    }

    bool is_number() {
        return number;
    }

    pair<bool, string> is_initialized() { 
        return {initialized, name}; 
    }

    void translate() override {
        add_to_tac("value: " + name);
    }
};

class ExpressionNode : public ASTNode {
public:
    ValueNode* left;
    ValueNode* right;
    string op;

    ExpressionNode(ValueNode* left, ValueNode* right, const string &op) : left(left), right(right), op(op) {
        this->initialized = left->is_initialized().first && right->is_initialized().first;
    }

    ExpressionNode(ValueNode* left) {
        this->left = left;
        this->right = nullptr;
        this->op = "";
        this->name = left->get_name();
        this->initialized = left->is_initialized().first;
    }

    ~ExpressionNode() {
        delete left;
        delete right;
    }

    void print() override {
        if (right == nullptr && left != nullptr) {
            left->print();
        } else {
            cout << "(";
            left->print();
            cout << " " << op << " ";
            right->print();
            cout << ")";
        }
    }

    bool is_number() {
        return left->is_number() && right->is_number();
    }

    void translate() override {
        add_to_tac("expression: " + left->get_name() + " " + op + " " + right->get_name());
    }
};

class ConditionNode : public ASTNode {
public:
    ValueNode* left;
    ValueNode* right;
    string op;

    ConditionNode(ValueNode* left, ValueNode* right, const string &op) : left(left), right(right), op(op) {}

    ~ConditionNode() {
        delete left;
        delete right;
    }

    void print() override {
        cout << "(";
        left->print();
        cout << " " << op << " ";
        right->print();
        cout << ")";
    }

    void translate() override {
        add_to_tac("condition: " + left->get_name() + " " + op + " " + right->get_name());
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

    void print() override {
        cout << "VAR ";
        for (int i = 0; i < identifiers.size(); i++) {
            identifiers[i]->print();
            if (i != identifiers.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }

    void add_child(IdentifierNode* node){
        identifiers.push_back(node);
    }

    void translate() override {
        add_to_tac("declarations: ");
        for (IdentifierNode* identifier : identifiers) {
            add_to_tac(identifier->get_name());
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

    void print() override {
        for (int i = 0; i < commands.size(); i++) {
            commands[i]->print();
        }
    }

    void add_child(CommandNode* node){
        commands.push_back(node);
    }

    void translate() override {
        add_to_tac("commands: ");
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

    void print() override {
        identifier->print();
        cout << " = ";
        expression->print();
        cout << endl;
    }

    void translate() override {
        add_to_tac("assign: " + identifier->get_name() + " = " + expression->left->get_name());
    }
};

class IfNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    IfNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

    ~IfNode() {
        delete condition;
        delete commands;
    }

    void print() override {
        cout << "IF ";
        condition->print();
        cout << " THEN " << endl;
        commands->print();
        cout << "ENDIF" << endl;
    }

    void translate() override {
        add_to_tac("if: ");
        condition->translate();
        commands->translate();
    }
};

class IfElseNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* if_commands;
    CommandsNode* else_commands;

    IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands) : condition(condition), if_commands(if_commands), else_commands(else_commands) {}

    ~IfElseNode() {
        delete condition;
        delete if_commands;
        delete else_commands;
    }

    void print() override {
        cout << "IF ";
        condition->print();
        cout << " THEN " << endl;
        if_commands->print();
        cout << "ELSE" << endl;
        else_commands->print();
        cout << "ENDIF" << endl;
    }

    void translate() override {
        add_to_tac("ifelse: ");
        condition->translate();
        if_commands->translate();
        else_commands->translate();
    }
};

class WhileNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    WhileNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

    ~WhileNode() {
        delete condition;
        delete commands;
    }

    void print() override {
        cout << "WHILE ";
        condition->print();
        cout << " DO " << endl;
        commands->print();
        cout << "ENDWHILE" << endl;
    }

    void translate() override {
        add_to_tac("while: ");
        condition->translate();
        commands->translate();
    }
};

class RepeatUntilNode : public CommandNode {
public:
    CommandsNode* commands;
    ConditionNode* condition;

    RepeatUntilNode(CommandsNode* commands, ConditionNode* condition) : commands(commands), condition(condition) {}

    ~RepeatUntilNode() {
        delete commands;
        delete condition;
    }

    void print() override {
        cout << "REPEAT" << endl;
        commands->print();
        cout << "UNTIL ";
        condition->print();
        cout << endl;
    }

    void translate() override {
        add_to_tac("repeatuntil: ");
        commands->translate();
        condition->translate();
    }
};

class ForToNode : public CommandNode {
public:
    IdentifierNode* identifier;
    ValueNode* start;
    ValueNode* end;
    CommandsNode* commands;

    ForToNode(IdentifierNode* identifier, ValueNode* start, ValueNode* end, CommandsNode* commands) : identifier(identifier), start(start), end(end), commands(commands) {}

    ~ForToNode() {
        delete start;
        delete end;
        delete commands;
    }

    void print() override {
        cout << "FOR " << identifier << " FROM ";
        start->print();
        cout << " TO ";
        end->print();
        cout << " DO " << endl;
        commands->print();
        cout << "ENDFOR" << endl;
    }

    void translate() override {
        add_to_tac("forto: ");
        start->translate();
        end->translate();
        commands->translate();
    }
};

class ForDownToNode : public CommandNode {
public:
    IdentifierNode* identifier;
    ValueNode* start;
    ValueNode* end;
    CommandsNode* commands;

    ForDownToNode(IdentifierNode* identifier, ValueNode* start, ValueNode* end, CommandsNode* commands) : identifier(identifier), start(start), end(end), commands(commands) {}

    ~ForDownToNode() {
        delete start;
        delete end;
        delete commands;
    }

    void print() override {
        cout << "FOR " << identifier << " FROM ";
        start->print();
        cout << " DOWNTO ";
        end->print();
        cout << " DO " << endl;
        commands->print();
        cout << "ENDFOR" << endl;
    }

    void translate() override {
        add_to_tac("fordownto: ");
        start->translate();
        end->translate();
        commands->translate();
    }
};

class ReadNode : public CommandNode {
public:
    IdentifierNode* identifier;

    ReadNode(IdentifierNode* identifier) : identifier(identifier) {}

    void print() override {
        cout << "READ ";
        identifier->print();
        cout << endl;
    }

    void translate() override {
        add_to_tac("read: " + identifier->get_name());
    }
};

class WriteNode : public CommandNode {
public:
    ValueNode* value;

    WriteNode(ValueNode* value) : value(value) {}

    ~WriteNode() {
        delete value;
    }

    void print() override {
        cout << "WRITE ";
        value->print();
        cout << endl;
    }

    void translate() override {
        add_to_tac("write: " + value->get_name());
    }
};

class MainNode : public ASTNode {
public:
    CommandsNode* commands;

    MainNode(CommandsNode* commands) : commands(commands) {}

    ~MainNode() {
        delete commands;
    }

    void print() override {
        cout << "PROGRAM IS" << endl;
        cout << "BEGIN" << endl;
        commands->print();
        cout << "END" << endl;
    }

    void translate() override {
        add_to_tac("main");
        commands->translate();
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

    void print() override {
        cout << "PROGRAM IS" << endl;
        declarations->print();
        cout << "BEGIN" << endl;
        commands->print();
        cout << "END" << endl;
    }

    void translate() override {
        tac.push_back("main with dec");
        declarations->translate();
        commands->translate();
    }
};

#endif
