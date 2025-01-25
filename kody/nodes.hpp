#ifndef NODES_HPP
#define NODES_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream> 

using namespace std;

class Node {
public:
    virtual ~Node() {}
    virtual void translate(bool inside_procedure) {}
    virtual void analyze(bool inside_procedure) {}
};

class ValueNode : public Node {
public:
    string name;
    long long value;
    bool initialized;
    long long register_number;
    int len;
    bool is_const;

    ValueNode(const string &name, bool initialized, int len, bool increment_reg, bool is_const);
    ValueNode(const string &name, bool initialized, long long register_number, int len);
    void set_value(long long value);
    void create_from_node(ValueNode* node);
    void initialize();
    virtual void translate(bool inside_procedure) override;
    virtual void analyze(bool inside_procedure) override;
};

class ArrayNode : public ValueNode {
public:
    long long size;
    long long start_register;
    long long start;

    ArrayNode(const string &name, long long start, long long end);
    ArrayNode(const string& name);
    void analyze(bool inside_procedure) override;
    void translate(bool inside_procedure) override;
};

class ArrayElem : public ValueNode {
public:
    string name;
    ArrayNode* array;
    ValueNode* index;

    ArrayElem(const string &name, ArrayNode* array, ValueNode* index);

};

class ConditionNode : public Node {
public:
    ValueNode* x;
    ValueNode* y;
    string op; //jpos, jneg, jzero
    bool is_negated;
    int len;

    ConditionNode(const string &op, ValueNode* x, ValueNode* y, bool is_negated);
    ~ConditionNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class CommandNode : public Node {
public:
    int len;
    virtual ~CommandNode() {}
    virtual void set_x(ValueNode* x) {};
    virtual void translate(bool inside_procedure) override {}
    virtual void print() {}
    virtual void analyze(bool inside_procedure) override {}
};

class CommandsNode : public Node {
public:
    vector<CommandNode*> commands;
    int len;

    CommandsNode();
    ~CommandsNode() override;
    void add_child(CommandNode* command);
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
    void print();
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
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class IfNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    IfNode(ConditionNode* condition, CommandsNode* commands);
    ~IfNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class IfElseNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* if_commands;
    CommandsNode* else_commands;

    IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands);
    ~IfElseNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class WhileNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    WhileNode(ConditionNode* condition, CommandsNode* commands);
    ~WhileNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class RepeatNode : public CommandNode {
public:
    ConditionNode* condition;
    CommandsNode* commands;

    RepeatNode(ConditionNode* condition, CommandsNode* commands);
    ~RepeatNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class ForNode : public CommandNode {
public: 
    ValueNode* i;
    ValueNode* start;
    ValueNode* end;
    ValueNode* one;
    CommandsNode* commands;
    string op; //jpos for increasing, jneg for decreasing
    ValueNode* counter;

    ForNode(ValueNode* i, ValueNode* start, ValueNode* end, ValueNode* one, CommandsNode* commands, const string &op);
    ForNode(ValueNode* i);
    ~ForNode() override;
    void set_start(ValueNode* start);
    void set_end(ValueNode* end);
    void set_commands(CommandsNode* commands);
    void set_op(const string& op);
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class IONode : public CommandNode {
public:
    string op;
    ValueNode* x;

    IONode(const string &op, ValueNode* x);
    ~IONode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class MainNode : public Node {
public:
    CommandsNode* commands;
    int len;
    bool declarations;

    MainNode(CommandsNode* commands, bool declarations);
    ~MainNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class ProcedureHeader : public Node {
public:
    string name;
    vector<ValueNode*>* arguments;

    ProcedureHeader(const string& name, vector<ValueNode*>* args);
};


class ProcedureNode : public Node {
public:
    ProcedureHeader* header;
    CommandsNode* commands;
    int len;
    long long start_line;
    ValueNode* return_address;

    ProcedureNode(ProcedureHeader* header, CommandsNode* commands);
    ~ProcedureNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class ProcedureCallNode : public CommandNode {
public:
    ProcedureNode* procedure;
    vector<ValueNode*>* arguments;

    ProcedureCallNode(ProcedureNode* procedure, vector<ValueNode*>* arguments);
    ~ProcedureCallNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class ProceduresNode : public Node {
public:
    int len;

    ProceduresNode();
    ~ProceduresNode() override;
    void add_procedure(ProcedureNode* procedure);
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

class ProgramNode : public Node {
public:
    MainNode* main;
    ProceduresNode* procedures;
    int len;

    ProgramNode(ProceduresNode* procedures, MainNode* main);
    ~ProgramNode() override;
    void translate(bool inside_procedure) override;
    void analyze(bool inside_procedure) override;
};

extern int first_free_register;
extern int code_line;
extern vector<ValueNode*> memory;
extern vector<ProcedureNode*> procedures;
extern vector<string> assembly;

void add_to_memory(ValueNode* value);
ValueNode* find_node(const string &name);
ArrayNode* find_array(const string &name);
ProcedureNode* find_procedure(const string &name);
void find_index(ArrayElem* arrayElem, long long store_reg);
void print_assembly();
void print_memory();
void save_assembly_to_file(const string& filename);

#endif