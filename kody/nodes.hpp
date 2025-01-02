#ifndef NODES_HPP
#define NODES_HPP

#include <iostream>
#include <vector>
#include <string>

using namespace std;

class Node {
public:
    virtual ~Node() {}
    virtual void translate() {}
};

class ValueNode : public Node {
public:
    string name;
    bool initialized;
    int register_number;
    int len;

    ValueNode(const string &name, bool initialized, int len);
    void initialize();
    void translate() override;
};

class ConditionNode : public Node {
public:
    ValueNode* x;
    ValueNode* y;
    string op; //jpos, jneg, jzero
    bool is_negated;
    int len = 2;

    ConditionNode(const string &op, ValueNode* x, ValueNode* y, bool is_negated);
    ~ConditionNode() override;
    void translate() override;
};

class CommandNode : public Node {
public:
    int len;
    virtual ~CommandNode() {}
    virtual void set_x(ValueNode* x) {};
    virtual void translate() override {}
};

class CommandsNode : public Node {
public:
    vector<CommandNode*> commands;
    int len;

    CommandsNode();
    ~CommandsNode() override;
    void add_child(CommandNode* command);
    void translate() override;
};

class AssignNode : public CommandNode {
public:
    string op; //add, sub, mul. div, mod, assign
    ValueNode* x;
    ValueNode* y;
    ValueNode* z;

    AssignNode(const string &op, ValueNode* x, ValueNode* y, ValueNode* z);
    ~AssignNode() override;
    void set_x(ValueNode* x) override;
    void translate() override;
};

class IfNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    IfNode(ConditionNode* condition, CommandsNode* commands);
    ~IfNode() override;
    void translate() override;
};

class IfElseNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* if_commands;
    CommandsNode* else_commands;

    IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands);
    ~IfElseNode() override;
    void translate() override;
};

class WhileNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    WhileNode(ConditionNode* condition, CommandsNode* commands);
    ~WhileNode() override;
    void translate() override;
};

class RepeatNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    RepeatNode(ConditionNode* condition, CommandsNode* commands);
    ~RepeatNode() override;
    void translate() override;
};

class ForNode : public CommandNode {
public: 
    ValueNode* i;
    ValueNode* start;
    ValueNode* end;
    ValueNode* one;
    CommandsNode* commands;
    string op; //jpos for increasing, jneg for decreasing

    ForNode(ValueNode* i, ValueNode* start, ValueNode* end, ValueNode* one, CommandsNode* commands, const string &op);
    ~ForNode() override;
    void translate() override;
};

class IONode : public CommandNode {
public:
    string op;
    ValueNode* x;

    IONode(const string &op, ValueNode* x);
    ~IONode() override;
    void translate() override;
};

class MainNode : public Node {
public:
    CommandsNode* commands;
    int len;
    bool declarations;

    MainNode(CommandsNode* commands, bool declarations);
    ~MainNode() override;
    void translate() override;
};

extern int first_free_register;
extern vector<ValueNode*> memory;
extern vector<string> assembly;

void add_to_memory(ValueNode* value);
ValueNode* find_node(const string &name);
void print_assembly();
void print_memory();

#endif