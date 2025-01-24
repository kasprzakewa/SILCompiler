#include "nodes.hpp"

// 0 - acc, 1 - rtrn address, 2 - a, 3 - b, 4 - c, 5 - sign, 6 - array register x, 7 - array register y, 8 - array register z
int first_free_register = 9;
int return_register = 1;
int a_register = 2;
int b_register = 3;
int result_register = 4;
int sign_register = 5;
int array_register_x = 6;
int array_register_y = 7;
int array_register_z = 8;
int code_line = 0;
int procedure_line = 1;
int mul_line = 0;

bool is_mul = false;
int procedures_length = 0;

vector<ValueNode*> memory;
vector<ProcedureNode*> procedures;
vector<string> assembly;

void add_to_memory(ValueNode* value) {
    memory.push_back(value);
}

void add_to_assembly(const string& line) {
    assembly.push_back(line);
    code_line++;
}

ValueNode* find_node(const string &name) {
    for (auto val : memory) {
        if (val->name == name) {
            return val;
        }
    }
    return nullptr;
}

ArrayNode* find_array(const string &name) {
    for (auto val : memory) {
        if (dynamic_cast<ArrayNode*>(val) && val->name == name) {
            return dynamic_cast<ArrayNode*>(val);
        }
    }
    return nullptr;
}

ProcedureNode* find_procedure(const string &name) {
    for (auto procedure : procedures) {
        if (procedure->name == name) {
            return procedure;
        }
    }
    return nullptr;
}

void find_index(ArrayElem* arrayElem, long long store_reg) {
        ValueNode* one = find_node("1");
        if (one == nullptr) {
            one = new ValueNode("1", false, 2, true);
            add_to_memory(one);
            add_to_assembly("SET 1");
            add_to_assembly("STORE " + to_string(one->register_number));
        }
        add_to_assembly("LOAD " + to_string(arrayElem->index->register_number));
        add_to_assembly("SUB " + to_string(arrayElem->array->start_register));
        add_to_assembly("ADD " + to_string(one->register_number));
        add_to_assembly("ADD " + to_string(arrayElem->array->register_number));
        add_to_assembly("STORE " + to_string(store_reg));
}

void print_assembly() {
    int i = 0;
    for (auto line : assembly) {
        cout << i++ << " " << line << endl;
    }
}

void save_assembly_to_file(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    int i = 0;
    for (const auto& line : assembly) {
        file << line << endl;
    }

    file.close();
    cout << "Assembly saved to " << filename << " with " << code_line << " lines of code." << endl;
}

void print_memory() {
    for (auto val : memory) {
        cout << "P" << val->register_number << ": " << val->name << " = " << val->value << endl;
    }
} 

void add_mul() {

    add_to_assembly("LOAD " + to_string(a_register));
    add_to_assembly("JNEG 10");
    add_to_assembly("LOAD " + to_string(b_register));
    add_to_assembly("JNEG 2");
    add_to_assembly("JUMP 18");
    add_to_assembly("SUB " + to_string(b_register));
    add_to_assembly("SUB " + to_string(b_register));
    add_to_assembly("STORE " + to_string(b_register));

    ValueNode* minus_one = find_node("-1");
    if (minus_one == nullptr) {
        minus_one = new ValueNode("-1", false, 2, true);
        add_to_memory(minus_one);
    }

    add_to_assembly("LOAD " + to_string(minus_one->register_number));
    add_to_assembly("STORE " + to_string(sign_register));
    add_to_assembly("JUMP 12");
    add_to_assembly("SUB " + to_string(a_register));
    add_to_assembly("SUB " + to_string(a_register));
    add_to_assembly("STORE " + to_string(a_register));
    add_to_assembly("LOAD " + to_string(b_register));
    add_to_assembly("JNEG 4");
    add_to_assembly("LOAD " + to_string(minus_one->register_number));
    add_to_assembly("STORE " + to_string(sign_register));
    add_to_assembly("JUMP 4");
    add_to_assembly("SUB " + to_string(b_register));
    add_to_assembly("SUB " + to_string(b_register));
    add_to_assembly("STORE " + to_string(b_register));
    add_to_assembly("LOAD " + to_string(b_register));
    add_to_assembly("JPOS 2");
    add_to_assembly("JUMP 15");
    add_to_assembly("HALF");
    add_to_assembly("ADD 0");
    add_to_assembly("SUB " + to_string(b_register));
    add_to_assembly("JZERO 4");
    add_to_assembly("LOAD " + to_string(result_register));
    add_to_assembly("ADD " + to_string(a_register));
    add_to_assembly("STORE " + to_string(result_register));
    add_to_assembly("LOAD " + to_string(a_register));
    add_to_assembly("ADD " + to_string(a_register));
    add_to_assembly("STORE " + to_string(a_register));
    add_to_assembly("LOAD " + to_string(b_register));
    add_to_assembly("HALF");
    add_to_assembly("STORE " + to_string(b_register));
    add_to_assembly("JUMP -15");
    add_to_assembly("LOAD " + to_string(sign_register));
    add_to_assembly("JPOS 5");
    add_to_assembly("LOAD " + to_string(result_register));
    add_to_assembly("SUB " + to_string(result_register));
    add_to_assembly("SUB " + to_string(result_register));
    add_to_assembly("STORE " + to_string(result_register));
    add_to_assembly("RTRN " + to_string(return_register));
}

// ValueNode**********************************************************************************************************
ValueNode::ValueNode(const string &name, bool initialized, int len, bool increment_reg) : name(name), initialized(initialized), len(len) {
    
    if (increment_reg) {
        register_number = first_free_register++;
    }
}

ValueNode::ValueNode(const string &name, bool initialized, long long register_number, int len) : name(name), initialized(initialized), register_number(register_number), len(len) {}

void ValueNode::set_value(long long value) {
    this->value = value;
}

void ValueNode::create_from_node(ValueNode* node) {
    name = node->name;
    value = node->value;
    initialized = node->initialized;
    len = node->len;
    register_number = node->register_number;
}

void ValueNode::initialize() {
    initialized = true;
}

void ValueNode::analyze() {
    // cout << "Length of node " << name << " = " << len << endl;
}

void ValueNode::translate() {

    if (len == 2) {
        add_to_assembly("SET " + name);
        add_to_assembly("STORE " + to_string(register_number));
    }
}

// ArrayNode**********************************************************************************************************
ArrayNode::ArrayNode(const string &name, long long start, long long end) : ValueNode(name, true, 4, true), start(start) {
    size = end - start + 1;
    first_free_register += size;

    ValueNode* start_reg = find_node(to_string(start));
    start_register = start_reg->register_number;
}

void ArrayNode::analyze() {
    // cout << "Length of array " << name << " = " << len << endl;
}

void ArrayNode::translate() {
    add_to_assembly("SET " + to_string(register_number));
    add_to_assembly("STORE " + to_string(register_number));
}

// ArrayElem**********************************************************************************************************
ArrayElem::ArrayElem(const string &name, ArrayNode* array, ValueNode* index) : ValueNode(name, true, 0, false), array(array), index(index) {}

// ConditionNode******************************************************************************************************
ConditionNode::ConditionNode(const string &op, ValueNode* x, ValueNode* y, bool is_negated) : op(op), x(x), y(y), is_negated(is_negated), len(2) {}

ConditionNode::~ConditionNode() {
    delete x;
    delete y;
}

void ConditionNode::analyze() {

}

void ConditionNode::translate() {
    add_to_assembly("LOAD " + to_string(x->register_number));
    add_to_assembly("SUB " + to_string(y->register_number));
}

// AssignNode********************************************************************************************************
AssignNode::AssignNode(const string &op, ValueNode* x, ValueNode* y, ValueNode* z) : op(op), x(x), y(y), z(z) {}

AssignNode::~AssignNode() {
    delete x;
    delete y;
    delete z;
}

void AssignNode::set_x(ValueNode* x) {
    this->x = x;
}

void AssignNode::analyze() {
    if (op == "ASSIGN") {
        len = 2;
    } else if (op == "MUL") {
        if (!is_mul) {
            is_mul = true;
            mul_line = procedure_line;
            procedures_length += 46;
        }
        
        len = 13;
    } else {
        len = 3;
    }

    // cout << "Length of assign node with op " << op << " = " << len << endl;
           
}

void AssignNode::translate() {

    if (op == "ASSIGN") {

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            find_index(arrayElem, array_register_x);
            add_to_assembly("LOAD " + to_string(y->register_number));
            add_to_assembly("STOREI " + to_string(array_register_x));

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
            find_index(arrayElem, array_register_y);
            add_to_assembly("LOADI " + to_string(array_register_y));
            add_to_assembly("STORE " + to_string(x->register_number));

        } else {
            add_to_assembly("LOAD " + to_string(y->register_number));
            add_to_assembly("STORE " + to_string(x->register_number));
            x->set_value(y->value);
        }
        
    } else if (op == "MUL") {

        ValueNode* zero = find_node("0");
        add_to_assembly("LOAD " + to_string(zero->register_number));
        add_to_assembly("STORE " + to_string(result_register));

        ValueNode* one = find_node("1");
        add_to_assembly("LOAD " + to_string(one->register_number));
        add_to_assembly("STORE " + to_string(sign_register));

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
            find_index(arrayElem, array_register_y);
            add_to_assembly("LOADI " + to_string(array_register_y));
            add_to_assembly("STORE " + to_string(a_register));
        } else {
            add_to_assembly("LOAD " + to_string(y->register_number));
            add_to_assembly("STORE " + to_string(a_register));
        }
        
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
            find_index(arrayElem, array_register_z);
            add_to_assembly("LOADI " + to_string(array_register_z));
            add_to_assembly("STORE " + to_string(b_register));
        } else {
            add_to_assembly("LOAD " + to_string(z->register_number));
            add_to_assembly("STORE " + to_string(b_register));
        }

        add_to_assembly("SET " + to_string(code_line + 3));
        add_to_assembly("STORE " + to_string(return_register));
        add_to_assembly("JUMP -" + to_string(code_line - mul_line));

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            find_index(arrayElem, array_register_x);
            add_to_assembly("LOAD " + to_string(result_register));
            add_to_assembly("STOREI " + to_string(array_register_x));
        } else {
            add_to_assembly("LOAD " + to_string(result_register));
            add_to_assembly("STORE " + to_string(x->register_number));
        }

        x->set_value(y->value * z->value);
    }
    else {
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            find_index(arrayElem, array_register_x);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y);
                
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // WSZYSTKO JEST Z TABLICY
                    find_index(arrayElem, array_register_z);
                    
                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly(op + "I " + to_string(array_register_z));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                } else { // X I Y Z TABLICY, Z NIE

                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly(op + " " + to_string(z->register_number));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                }
            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // X I Z Z TABLICY, Y NIE
                find_index(arrayElem, array_register_z);

                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly(op + "I " + to_string(array_register_z));
                add_to_assembly("STOREI " + to_string(array_register_x));
            } else { // TYLKO X Z TABLICY
                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly(op + " " + to_string(z->register_number));
                add_to_assembly("STOREI " + to_string(array_register_x));

            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) { // X NIE Z TABLICY
            find_index(arrayElem, array_register_y);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // Y I Z Z TABLICY
                find_index(arrayElem, array_register_z);

                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly(op + "I " + to_string(array_register_z));
                add_to_assembly("STORE " + to_string(x->register_number));
            } else { // TYLKO Y Z TABLICY
                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly(op + " " + to_string(z->register_number));
                add_to_assembly("STORE " + to_string(x->register_number));
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // X I Y NIE Z TABLICY, Z Z TABLICY
            find_index(arrayElem, array_register_z);
            add_to_assembly("LOADI " + to_string(array_register_z));
            add_to_assembly(op + " " + to_string(y->register_number));
            add_to_assembly("STORE " + to_string(x->register_number));
        } else { // WSZYSTKO NIE Z TABLICY
            add_to_assembly("LOAD " + to_string(y->register_number));
            add_to_assembly(op + " " + to_string(z->register_number));
            add_to_assembly("STORE " + to_string(x->register_number));
        }

        if (op == "ADD") {
            x->set_value(y->value + z->value);
        } else if (op == "SUB") {
            x->set_value(y->value - z->value);
        } else if (op == "DIV") {
            x->set_value(y->value / z->value);
        } else if (op == "MOD") {
            x->set_value(y->value % z->value);
        }
    }
}

// IfNode*************************************************************************************************************
IfNode::IfNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

IfNode::~IfNode() {
    delete condition;
    delete commands;
}

void IfNode::analyze() {
    commands->analyze();
    if (condition->is_negated) {
        len = 3 + commands->len;
    } else {
        len = 4 + commands->len;
    }

    // cout << "Length of if node = " << len << endl;
}

void IfNode::translate() {
    condition->translate();
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(commands->len + 1));
    } else {
        add_to_assembly(condition->op + " " + to_string(2));
        add_to_assembly("JUMP " + to_string(commands->len + 1));
    }
    commands->translate();
}

// IfElseNode*********************************************************************************************************
IfElseNode::IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands) : condition(condition), if_commands(if_commands), else_commands(else_commands) {}

IfElseNode::~IfElseNode() {
    delete condition;
    delete if_commands;
    delete else_commands;
}

void IfElseNode::analyze() {
    if_commands->analyze();
    else_commands->analyze();
    len = 4 + if_commands->len + else_commands->len;

    // cout << "Length of if else node = " << len << endl;
}

void IfElseNode::translate() {
    condition->translate();
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(if_commands->len + 2));
        if_commands->translate();
        add_to_assembly("JUMP " + to_string(else_commands->len + 1));
        else_commands->translate();
    } else {
        add_to_assembly(condition->op + " " + to_string(else_commands->len + 2));
        else_commands->translate();
        add_to_assembly("JUMP " + to_string(if_commands->len + 1));
        if_commands->translate();
    }
}

// WhileNode**********************************************************************************************************
WhileNode::WhileNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

WhileNode::~WhileNode() {
    delete condition;
    delete commands;
}

void WhileNode::analyze() {
    commands->analyze();
    if (condition->is_negated) {
        len = commands->len + 4;
    } else {
        len = commands->len + 5;
    }

    // cout << "Length of while node = " << len << endl;
}

void WhileNode::translate() {
    condition->translate();
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(commands->len + 2));
    } else {
        add_to_assembly(condition->op + " " + to_string(2));
        add_to_assembly("JUMP " + to_string(commands->len + 2));
    }
    commands->translate();
    add_to_assembly("JUMP -" + to_string(len));
}

// RepeatNode*********************************************************************************************************
RepeatNode::RepeatNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

RepeatNode::~RepeatNode() {
    delete condition;
    delete commands;
}

void RepeatNode::analyze() {
    commands->analyze();
    if (condition->is_negated) {
        len = commands->len + 4;
    } else {
        len = commands->len + 3;
    }

    // cout << "Length of repeat node = " << len << endl;
}

void RepeatNode::translate() {
    commands->translate();
    condition->translate();
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(2));
        add_to_assembly("JUMP -" + to_string(len - 1));
    } else {
        add_to_assembly(condition->op + " -" + to_string(len - 1));
    }
}

// ForNode************************************************************************************************************
ForNode::ForNode(ValueNode* i, ValueNode* start, ValueNode* end, ValueNode* one, CommandsNode* commands, const string &op) : i(i), start(start), end(end), one(one), commands(commands), op(op) {}

ForNode::ForNode(ValueNode* i) : i(i) {}

ForNode::~ForNode() {
    delete i;
    delete start;
    delete end;
    delete commands;
}

void ForNode::set_start(ValueNode* start) {
    this->start = start;
}

void ForNode::set_end(ValueNode* end) {
    this->end = end;
}

void ForNode::set_commands(CommandsNode* commands) {
    this->commands = commands;
}

void ForNode::set_op(const string& op) {
    this->op = op;
}

void ForNode::analyze() {
    commands->analyze();
    len = commands->len + 8;

    // cout << "Length of for node = " << len << endl;
}

void ForNode::translate() {
    add_to_assembly("LOAD " + to_string(start->register_number));
    add_to_assembly("STORE " + to_string(i->register_number));
    add_to_assembly("LOAD " + to_string(i->register_number));
    add_to_assembly("SUB " + to_string(end->register_number));
    add_to_assembly(op + " " + to_string(commands->len + 4));
    commands->translate();
    add_to_assembly("LOAD " + to_string(i->register_number));
    
    if (op == "JPOS") {
        add_to_assembly("ADD " + to_string(one->register_number));
    } else {
        add_to_assembly("SUB " + to_string(one->register_number));
    }
    add_to_assembly("JUMP -" + to_string(len - 2));
}

// IONode*************************************************************************************************************
IONode::IONode(const string &op, ValueNode* x) : op(op), x(x) {
    len = 1;
}

IONode::~IONode() {
    delete x;
}

void IONode::analyze() {
    // cout << "Length of io node = " << len << endl;

}

void IONode::print() {
    cout << "WRITE " << x->name << endl;
}

void IONode::translate() {
    if(ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
        ValueNode* one = find_node("1");
        if (one == nullptr) {
            one = new ValueNode("1", false, 2, true);
            add_to_memory(one);
            add_to_assembly("SET 1");
            add_to_assembly("STORE " + to_string(one->register_number));
        }
        add_to_assembly("LOAD " + to_string(arrayElem->index->register_number));
        add_to_assembly("SUB " + to_string(arrayElem->array->start_register));
        add_to_assembly("ADD " + to_string(one->register_number));
        add_to_assembly("ADD " + to_string(arrayElem->array->register_number));
        add_to_assembly("STORE " + to_string(array_register_x));
        add_to_assembly("LOADI 0");
        add_to_assembly(op + " 0");
        if (op == "GET") {
            add_to_assembly("STOREI " + to_string(array_register_x));
        }
    } else {
        add_to_assembly(op + " " + to_string(x->register_number));
    }
    
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

void CommandsNode::print() {
    for (auto comm : commands) {
        comm->print();
    }
}

void CommandsNode::add_child(CommandNode* command) {
    commands.push_back(command);
}

void CommandsNode::analyze() {
    for (auto command : commands) {
        command->analyze();
        len += command->len;
    }
    // cout << "Length of commands node = " << len << endl;
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

void MainNode::analyze() {
    commands->analyze();
}

void MainNode::translate() {
    if(is_mul) {
        cout << "Mul starts at line " << mul_line << endl;
        add_mul();
    }
    for (auto val : memory) {
        val->translate();
    }
    commands->translate();
    add_to_assembly("HALT");
}

// ProcedureNode*******************************************************************************************************
ProcedureNode::ProcedureNode(const string& name, CommandsNode* commands) : name(name), commands(commands) {
    len = 0;
}

ProcedureNode::~ProcedureNode() {
    delete commands;
}

void ProcedureNode::translate() {
    cout << "Procedure " << name << " starts at line " << start_line << endl;
    commands->translate();
    add_to_assembly("RTRN " + to_string(return_register));
}

void ProcedureNode::analyze() {
    start_line = procedure_line;
    commands->analyze();
    len = commands->len + 1;
    procedure_line += len;
}

// ProcedureCallNode***************************************************************************************************
ProcedureCallNode::ProcedureCallNode(ProcedureNode* procedure) : procedure(procedure) {}

ProcedureCallNode::~ProcedureCallNode() {
    delete procedure;
}

void ProcedureCallNode::translate() {
    add_to_assembly("SET " + to_string(code_line + 3));
    add_to_assembly("STORE " + to_string(return_register));
    add_to_assembly("JUMP -" + to_string(code_line - procedure->start_line));
}

void ProcedureCallNode::analyze() {
    len = procedure->len;
    cout << "Length of procedure call node = " << len << endl;
}

// ProceduresNode*******************************************************************************************************
ProceduresNode::ProceduresNode() {
    len = 0;
}

ProceduresNode::~ProceduresNode() {
    for (auto procedure : procedures) {
        delete procedure;
    }
}

void ProceduresNode::add_procedure(ProcedureNode* procedure) {
    procedures.push_back(procedure);
}

void ProceduresNode::translate() {
    for (auto procedure : procedures) {
        procedure->translate();
    }
}

void ProceduresNode::analyze() {
    for (auto procedure : procedures) {
        procedure->analyze();
        len += procedure->len;
    }
    procedures_length += len;
    cout << "Length of procedures node = " << len << endl;
}

// ProgramNode********************************************************************************************************
ProgramNode::ProgramNode(ProceduresNode* procedures, MainNode* main) : main(main), procedures(procedures) {}

ProgramNode::~ProgramNode() {
    delete main;
    delete procedures;
}

void ProgramNode::translate() {
    if (procedures_length > 0) {
        add_to_assembly("JUMP " + to_string(procedures_length + 1));
    }
    procedures->translate();
    main->translate();
}

void ProgramNode::analyze() {
    procedures->analyze();
    main->analyze();
    cout << "Length of procedures = " << procedures_length << endl;
}
