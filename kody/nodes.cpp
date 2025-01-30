#include "nodes.hpp"

// 0 - acc, 1 - rtrn address, 2 - a, 3 - b, 4 - c, 5 - sign, 6 - array register x, 7 - array register y, 8 - array register z
long long first_free_register = 18;
long long return_register = 1;
long long a_register = 2;
long long b_register = 3;
long long result_register = 4;
long long sign_register = 5;
long long array_register_x = 6;
long long array_register_y = 7;
long long array_register_z = 8;
long long dzielna = 9;
long long dzielnik = 10;
long long wynik = 11;
long long mocdwa = 12;
long long oryginalny = 13;
long long reszta = 14;
long long znaka = 16;
long long znakb = 17;
long long return_div = 15;
long long code_line = 0;
long long procedure_line = 1;
long long mul_line = 0;
long long div_line = 0;
long long counter_counter = 0;

string first_op = "";

bool is_mul = false;
bool is_div = false;
int div_length = 104;
int mul_length = 46;
long long procedures_length = 0;

vector<ValueNode*> memory;
vector<ProcedureNode*> procedures_vector;
vector<ProcedureArray*> procedure_arrays;
vector<string> assembly;

void add_to_memory(ValueNode* value) {
    memory.push_back(value);
}

void add_to_procedure_arrays(ProcedureArray* array) {
    procedure_arrays.push_back(array);
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

ProcedureArray* find_procedure_array(const string &name) {
    for (auto val : procedure_arrays) {
        if (val->name == name) {
            return val;
        }
    }
    return nullptr;
}

ProcedureNode* find_procedure(const string &name) {
    for (auto procedure : procedures_vector) {
        if (procedure->header->name == name) {
            return procedure;
        }
    }
    return nullptr;
}

void find_index(ArrayElem* arrayElem, long long store_reg, bool inside_procedure) {
        ValueNode* one = find_node("1");

        if (inside_procedure) {
            if (ProcedureArray* arr = dynamic_cast<ProcedureArray*>(arrayElem->array)) {
                if (arrayElem->index->procedure_argument) {
                    add_to_assembly("LOADI " + to_string(arrayElem->index->register_number));
                } else {
                    add_to_assembly("LOAD " + to_string(arrayElem->index->register_number));
                }
                add_to_assembly("SUB " + to_string(arr->start->register_number));
                add_to_assembly("ADD " + to_string(one->register_number));
                add_to_assembly("ADD " + to_string(arr->array->register_number));
                add_to_assembly("STORE " + to_string(store_reg));
            } else {
                if (arrayElem->index->procedure_argument) {
                    add_to_assembly("LOADI " + to_string(arrayElem->index->register_number));
                } else {
                    add_to_assembly("LOAD " + to_string(arrayElem->index->register_number));
                }
                
                add_to_assembly("SUB " + to_string(arrayElem->array->start_register));
                add_to_assembly("ADD " + to_string(one->register_number));
                add_to_assembly("ADD " + to_string(arrayElem->array->register_number));
                add_to_assembly("STORE " + to_string(store_reg));
            }
        } else {
            add_to_assembly("LOAD " + to_string(arrayElem->index->register_number));
            add_to_assembly("SUB " + to_string(arrayElem->array->start_register));
            add_to_assembly("ADD " + to_string(one->register_number));
            add_to_assembly("ADD " + to_string(arrayElem->array->register_number));
            add_to_assembly("STORE " + to_string(store_reg));
        }
       
}

void print_procedures() {
    for (auto procedure : procedures_vector) {
        cout << "Procedure " << procedure->header->name << " with " << procedure->len << " lines of code at line " << procedure->start_line << endl;
    }
}

void print_assembly() {
    long long i = 0;
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

    long long i = 0;
    for (const auto& line : assembly) {
        file << line << endl;
    }

    file.close();
    cout << "Assembly saved to " << filename << " with " << code_line << " lines of code." << endl;
}

void print_memory() {
    for (auto val : memory) {
        cout << "P" << val->register_number << ": " << val->name << " = " << val->value ;
        if (val->procedure_argument) {
            cout << " procedure = " << val->procedure_name << endl;
        } else {
            cout << endl;
        }
    }
} 

void add_div() {
    ValueNode* zero = find_node("0");
    ValueNode* one = find_node("1");
    ValueNode* minus_one = find_node("-1");

    if (zero == nullptr) {
        zero = new ValueNode("0", true, 2, true, true);
        add_to_memory(zero);
    }

    if (one == nullptr) {
        one = new ValueNode("1", true, 2, true, true);
        add_to_memory(one);
    }

    if (minus_one == nullptr) {
        minus_one = new ValueNode("-1", true, 2, true, true);
        add_to_memory(minus_one);
    }

    add_to_assembly("LOAD " + to_string(zero->register_number));
    add_to_assembly("STORE " + to_string(wynik));
    add_to_assembly("LOAD " + to_string(dzielnik));
    add_to_assembly("JZERO 98");
    add_to_assembly("LOAD " + to_string(one->register_number));
    add_to_assembly("STORE 12");
    add_to_assembly("LOAD 9");
    add_to_assembly("JNEG 4");
    add_to_assembly("LOAD " + to_string(one->register_number));
    add_to_assembly("STORE 16");
    add_to_assembly("JUMP 6");
    add_to_assembly("SUB 9");
    add_to_assembly("SUB 9");
    add_to_assembly("STORE 9");
    add_to_assembly("LOAD " + to_string(minus_one->register_number));
    add_to_assembly("STORE 16");
    add_to_assembly("LOAD 10");
    add_to_assembly("JNEG 6");
    add_to_assembly("LOAD " + to_string(one->register_number));
    add_to_assembly("STORE 17");
    add_to_assembly("LOAD 10");
    add_to_assembly("STORE 13");
    add_to_assembly("JUMP 7");
    add_to_assembly("SUB 10");
    add_to_assembly("SUB 10");
    add_to_assembly("STORE 10");
    add_to_assembly("STORE 13");
    add_to_assembly("LOAD " + to_string(minus_one->register_number));
    add_to_assembly("STORE 17");
    add_to_assembly("LOAD 10");
    add_to_assembly("SUB 9");
    add_to_assembly("JPOS 8");
    add_to_assembly("LOAD 10");
    add_to_assembly("ADD 10");
    add_to_assembly("STORE 10");
    add_to_assembly("LOAD 12");
    add_to_assembly("ADD 12");
    add_to_assembly("STORE 12");
    add_to_assembly("JUMP -9");
    add_to_assembly("LOAD 10");
    add_to_assembly("SUB 13");
    add_to_assembly("JNEG 17");
    add_to_assembly("LOAD 9");
    add_to_assembly("SUB 10");
    add_to_assembly("JNEG 7");
    add_to_assembly("LOAD 11");
    add_to_assembly("ADD 12");
    add_to_assembly("STORE " + to_string(wynik));
    add_to_assembly("LOAD 9");
    add_to_assembly("SUB 10");
    add_to_assembly("STORE 9");
    add_to_assembly("LOAD 10");
    add_to_assembly("HALF");
    add_to_assembly("STORE 10");
    add_to_assembly("LOAD 12");
    add_to_assembly("HALF");
    add_to_assembly("STORE 12");
    add_to_assembly("JUMP -18");
    add_to_assembly("LOAD 9");
    add_to_assembly("STORE 14");
    add_to_assembly("LOAD 16");
    add_to_assembly("SUB " + to_string(minus_one->register_number));
    add_to_assembly("JZERO 18");
    add_to_assembly("LOAD 17");
    add_to_assembly("SUB " + to_string(minus_one->register_number));
    add_to_assembly("JZERO 2");
    add_to_assembly("JUMP 13");
    add_to_assembly("LOAD 14");
    add_to_assembly("JZERO 7");
    add_to_assembly("LOAD 11");
    add_to_assembly("ADD " + to_string(one->register_number));
    add_to_assembly("STORE " + to_string(wynik));
    add_to_assembly("LOAD 14");
    add_to_assembly("SUB 13");
    add_to_assembly("STORE 14");
    add_to_assembly("LOAD 11");
    add_to_assembly("SUB 11");
    add_to_assembly("SUB 11");
    add_to_assembly("STORE " + to_string(wynik));
    add_to_assembly("JUMP 21");
    add_to_assembly("LOAD 17");
    add_to_assembly("SUB " + to_string(minus_one->register_number));
    add_to_assembly("JZERO 14");
    add_to_assembly("LOAD 14");
    add_to_assembly("JZERO 7");
    add_to_assembly("LOAD 11");
    add_to_assembly("ADD " + to_string(one->register_number));
    add_to_assembly("STORE " + to_string(wynik));
    add_to_assembly("LOAD 13");
    add_to_assembly("SUB 14");
    add_to_assembly("STORE 14");
    add_to_assembly("LOAD 11");
    add_to_assembly("SUB 11");
    add_to_assembly("SUB 11");
    add_to_assembly("STORE " + to_string(wynik));
    add_to_assembly("JUMP 5");
    add_to_assembly("LOAD 14");
    add_to_assembly("SUB 14");
    add_to_assembly("SUB 14");
    add_to_assembly("STORE 14");
    add_to_assembly("JUMP 3");
    add_to_assembly("LOAD " + to_string(zero->register_number));
    add_to_assembly("STORE 14");
    add_to_assembly("RTRN " + to_string(return_div));
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
        minus_one = new ValueNode("-1", false, 2, true, true);
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
ValueNode::ValueNode(const string &name, bool initialized, long long len, bool increment_reg, bool is_const) : name(name), initialized(initialized), len(len), is_const(is_const) {
    
    if (increment_reg) {
        if (first_free_register < 0) {
            cout << "cannot allocate" << endl;
        }
        register_number = first_free_register++;
    }
}

ValueNode::ValueNode(const string &name, bool initialized, long long register_number, long long len) : name(name), initialized(initialized), register_number(register_number), len(len) {}

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

void ValueNode::analyze(bool inside_procedure) {
}

void ValueNode::translate(bool inside_procedure) {

    if (len == 2) {
        add_to_assembly("SET " + name);
        add_to_assembly("STORE " + to_string(register_number));
    }
}

// ArrayNode**********************************************************************************************************
ArrayNode::ArrayNode(const string &name, long long start, long long end) : ValueNode(name, true, 2, true, false), start(start) {
    size = end - start + 1;
    first_free_register += size;

    ValueNode* start_reg = find_node(to_string(start));
    start_register = start_reg->register_number;
}

ArrayNode::ArrayNode(const string& name) : ValueNode(name, true, 0, true, false) {
    size = 0;
    start = 0;
    start_register = 0;
}

ArrayNode::ArrayNode() : ValueNode("", true, 0, false, false){
    size = 0;
    start = 0;
    start_register = 0;
}


void ArrayNode::analyze(bool inside_procedure) {
}

void ArrayNode::translate(bool inside_procedure) {
    add_to_assembly("SET " + to_string(register_number));
    add_to_assembly("STORE " + to_string(register_number));
}

// ArrayElem**********************************************************************************************************
ArrayElem::ArrayElem(const string &name, ArrayNode* array, ValueNode* index) : ValueNode(name, true, 0, false, false), array(array), index(index) {}

// ProcedureArray******************************************************************************************************
ProcedureArray::ProcedureArray(const string &name, const string& procedure_name) : ArrayNode() {
    this->name = name;
    array = new ArrayNode(name);
    array->procedure_argument = true;
    array->procedure_name = procedure_name;
    start = new ValueNode(name + "start", true, 0, true, false);
    add_to_memory(array);
    add_to_memory(start);
    add_to_procedure_arrays(this);
}

ProcedureArray::~ProcedureArray() {
    delete array;
    delete start;
}

// ConditionNode******************************************************************************************************
ConditionNode::ConditionNode(const string &op, ValueNode* x, ValueNode* y, bool is_negated) : op(op), x(x), y(y), is_negated(is_negated), len(2) {}

ConditionNode::~ConditionNode() {
    delete x;
    delete y;
}

void ConditionNode::analyze(bool inside_procedure) {
    len = 0;
    if (y->is_const && y->name == "0") {
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            len += 6;
        } else {
            len += 1;
        }
    } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            len += 5;
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                len += 7;
            } else {
                len += 2;
            }
    } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
        len += 7;
    } else {
        len += 2;
    }
}

void ConditionNode::translate(bool inside_procedure) {
    if (y->is_const && y->name == "0") {
        if (inside_procedure) {
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                find_index(arrayElem, array_register_x, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_x));
            } else if (x->procedure_argument) {
                add_to_assembly("LOADI " + to_string(x->register_number));
            } else {
                add_to_assembly("LOAD " + to_string(x->register_number));
            }
        } else {
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                find_index(arrayElem, array_register_x, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_x));
            } else {
                add_to_assembly("LOAD " + to_string(x->register_number));
            }
        }
    } else if (inside_procedure) {
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            find_index(arrayElem, array_register_x, inside_procedure);
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_x));
                add_to_assembly("SUBI " + to_string(array_register_y));
            } else {
                add_to_assembly("LOADI " + to_string(array_register_x));
                add_to_assembly("SUB " + to_string(y->register_number));
            }
        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
            find_index(arrayElem, array_register_y, inside_procedure);
            add_to_assembly("LOAD " + to_string(x->register_number));
            add_to_assembly("SUBI " + to_string(array_register_y));
        } else {
            if (x->procedure_argument) {
                add_to_assembly("LOADI " + to_string(x->register_number));
            } else {
                add_to_assembly("LOAD " + to_string(x->register_number));
            }

            if (y->procedure_argument) {
                add_to_assembly("SUBI " + to_string(y->register_number));
            } else {
                add_to_assembly("SUB " + to_string(y->register_number));
            }
        }
        
    } else {
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            find_index(arrayElem, array_register_x, inside_procedure);
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_x));
                add_to_assembly("SUBI " + to_string(array_register_y));
            } else {
                add_to_assembly("LOADI " + to_string(array_register_x));
                add_to_assembly("SUB " + to_string(y->register_number));
            }
        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
            find_index(arrayElem, array_register_y, inside_procedure);
            add_to_assembly("LOAD " + to_string(x->register_number));
            add_to_assembly("SUBI " + to_string(array_register_y));
        } else {
            add_to_assembly("LOAD " + to_string(x->register_number));
            add_to_assembly("SUB " + to_string(y->register_number));
        }
    }

    cout << "condition len is " << len << endl;
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

void AssignNode::analyze(bool inside_procedure) {
    if (op == "ASSIGN") {
        len = 0;
        if (inside_procedure) {

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

                len += 5;
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                    len += 7;
                } else if (y->procedure_argument) {
                    len += 2;
                } else {
                    len += 2;
                }

            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                
                len += 7;

            } else {

                if (y->procedure_argument) {
                    len += 1;
                } else {
                    len += 1;
                }

                if (x->procedure_argument) {
                    len += 1;
                } else {
                    len += 1;
                }
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            len += 5;

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                len += 7;
            } else {
                len += 2;
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
            len += 7;

        } else {
            len += 2;
        }
    } else if (op == "MUL") {

        if (!is_mul) {
            is_mul = true;
            procedures_length += mul_length;

            if (! is_div) {
                first_op = "MUL";
            }
        }

        len = 4;

        if (inside_procedure) {

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                len += 6;
            } else if (y->procedure_argument) {
                len += 1;
            } else {
                len += 1;
            }
            len += 1;

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                len += 6;
            } else if (z->procedure_argument) {
                len += 1;
            } else {
                len += 1;
            }
            len += 1;

        } else {

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                len += 6;
            } else {
                len += 1;
            }
            len += 1;
            
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                len += 6;
            } else {
                len += 1;
            }
            len += 1;

        }

        len += 2;
        if (code_line < mul_line) {
            len += 1;
        } else {
            len += 1;
        }

        if (inside_procedure) {
            
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                len += 7;
            } else if (x->procedure_argument) {
                len += 2;
            } else {
                len += 2;
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            len += 7;
        } else {
            len += 2;
        }

    } else if (op == "DIV" || op == "MOD") {
        
        len = 0;
        if (z->is_const && z->name == "0") {
                len += 1;
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                    len += 6;
                } else if (inside_procedure) { 
                    if (x->procedure_argument) {
                        len += 1;
                    } else {
                        len += 1;
                    }
                } else {
                    len += 1;
                }
        } else if (z->is_const && z->name == "1") {
                if (op == "DIV") {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                        len += 6;
                    } else if (inside_procedure) { 
                        if (y->procedure_argument) {
                            len += 1;
                        } else {
                            len += 1;
                        }
                    } else {
                        len += 1;
                    }

                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                        len += 6;
                    } else if (inside_procedure) { 
                        if (x->procedure_argument) {
                            len += 1;
                        } else {
                            len += 1;
                        }
                    } else {
                        len += 1;
                    }


                } else {
                    len += 1;
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                        len += 6;
                    } else if (inside_procedure) { 
                        if (x->procedure_argument) {
                            len += 1;
                        } else {
                            len += 1;
                        }
                    } else {
                        len += 1;
                    }
                }
                
        } else if (y->is_const && y->name == "0") {
            len += 1;
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                len += 6;
            } else if (inside_procedure) { 
                if (x->procedure_argument) {
                    len += 1;
                } else {
                    len += 1;
                }
            } else {
                len += 1;
            }
        } 
        else {
            if (!is_div) {
                is_div = true;
                procedures_length += div_length;

                if (! is_mul) {
                    first_op = "DIV";
                }
            }
            if (inside_procedure) {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                    len += 6;
                } else if (y->procedure_argument) {
                    len += 1;
                } else {
                    len += 1;
                }
            } else {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                    len += 6;
                } else {
                    len += 1;
                }
            }
            len += 1;

            if (inside_procedure) {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                    len += 6;
                } else if (z->procedure_argument) {
                    len += 1;
                } else {
                    len += 1;
                }
            } else {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                    len += 6;
                } else {
                    len += 1;
                }
            }
            len += 3;

            if (code_line < div_line) {
                len += 1;
            } else {
                len += 1;
            }

            if (op == "DIV") {
                len += 1;
                if (inside_procedure) {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        len += 6;
                    } else if (x->procedure_argument) {
                        len += 1;
                    } else {
                        len += 1;
                    }
                } else {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        len += 6;
                    } else {
                        len += 1;
                    }
                }
            } else {
                
                len += 1;
                if (inside_procedure) {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        len += 6;
                    } else if (x->procedure_argument) {
                        len += 1;
                    } else {
                        len += 1;
                    }
                } else {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        len += 6;
                    } else {
                        len += 1;
                    }
                }
            }
        }
    } else {
        if (inside_procedure) {
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            len = 5;

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                len += 8;
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                    len += 5;
                }
            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                len += 8;
            } else {
                len += 3;
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {

            len = 8;
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                len += 5;
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
            len = 8;
        } else {
            len = 3;
        }
        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            len = 5;

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                len += 8;
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                    len += 5;
                }
            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                len += 8;
            } else {
                len += 3;
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {

            len = 8;
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                len += 5;
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
            len = 8;
        } else {
            len = 3;
        }
    }

    cout << "assign len is " << len << endl;
}

void AssignNode::translate(bool inside_procedure) {

    if (op == "ASSIGN") {

        if (inside_procedure) {

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

                find_index(arrayElem, array_register_x, inside_procedure);
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                    find_index(arrayElem, array_register_y, inside_procedure);
                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                } else if (y->procedure_argument) {
                    add_to_assembly("LOADI " + to_string(y->register_number));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                } else {
                    add_to_assembly("LOAD " + to_string(y->register_number));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                }

            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                
                find_index(arrayElem, array_register_y, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly("STORE " + to_string(x->register_number));

            } else {

                if (y->procedure_argument) {
                    add_to_assembly("LOADI " + to_string(y->register_number));
                } else {
                    add_to_assembly("LOAD " + to_string(y->register_number));
                }

                if (x->procedure_argument) {
                    add_to_assembly("STOREI " + to_string(x->register_number));
                } else {
                    add_to_assembly("STORE " + to_string(x->register_number));
                }
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            find_index(arrayElem, array_register_x, inside_procedure);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly("STOREI " + to_string(array_register_x));
            } else {
                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly("STOREI " + to_string(array_register_x));
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
            find_index(arrayElem, array_register_y, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_y));
            add_to_assembly("STORE " + to_string(x->register_number));

        } else {
            add_to_assembly("LOAD " + to_string(y->register_number));
            add_to_assembly("STORE " + to_string(x->register_number));
        }
        
    } else if (op == "MUL") {

        ValueNode* zero = find_node("0");
        add_to_assembly("LOAD " + to_string(zero->register_number));
        add_to_assembly("STORE " + to_string(result_register));

        ValueNode* one = find_node("1");
        add_to_assembly("LOAD " + to_string(one->register_number));
        add_to_assembly("STORE " + to_string(sign_register));

        if (inside_procedure) {

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_y));
            } else if (y->procedure_argument) {
                add_to_assembly("LOADI " + to_string(y->register_number));
            } else {
                add_to_assembly("LOAD " + to_string(y->register_number));
            }
            add_to_assembly("STORE " + to_string(a_register));

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                find_index(arrayElem, array_register_z, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_z));
            } else if (z->procedure_argument) {
                add_to_assembly("LOADI " + to_string(z->register_number));
            } else {
                add_to_assembly("LOAD " + to_string(z->register_number));
            }
            add_to_assembly("STORE " + to_string(b_register));

        } else {

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_y));
            } else {
                add_to_assembly("LOAD " + to_string(y->register_number));
            }
            add_to_assembly("STORE " + to_string(a_register));
            
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                find_index(arrayElem, array_register_z, inside_procedure);
                add_to_assembly("LOADI " + to_string(array_register_z));
            } else {
                add_to_assembly("LOAD " + to_string(z->register_number));
            }
            add_to_assembly("STORE " + to_string(b_register));

        }

        add_to_assembly("SET " + to_string(code_line + 3));
        add_to_assembly("STORE " + to_string(return_register));
        if (code_line < mul_line) {
            add_to_assembly("JUMP " + to_string(mul_line - code_line));
        } else {
            add_to_assembly("JUMP -" + to_string(code_line - mul_line));
        }

        if (inside_procedure) {
            
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                find_index(arrayElem, array_register_x, inside_procedure);
                add_to_assembly("LOAD " + to_string(result_register));
                add_to_assembly("STOREI " + to_string(array_register_x));
            } else if (x->procedure_argument) {
                add_to_assembly("LOAD " + to_string(result_register));
                add_to_assembly("STOREI " + to_string(x->register_number));
            } else {
                add_to_assembly("LOAD " + to_string(result_register));
                add_to_assembly("STORE " + to_string(x->register_number));
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            find_index(arrayElem, array_register_x, inside_procedure);
            add_to_assembly("LOAD " + to_string(result_register));
            add_to_assembly("STOREI " + to_string(array_register_x));
        } else {
            add_to_assembly("LOAD " + to_string(result_register));
            add_to_assembly("STORE " + to_string(x->register_number));
        }

    } else if (op == "DIV" || op == "MOD") {
        if (z->is_const && z->name == "0") {
                ValueNode* zero = find_node("0");
                if (zero == nullptr) {
                    zero = new ValueNode("0", true, 2, true, true);
                    add_to_memory(zero);
                }
                add_to_assembly("LOAD " + to_string(zero->register_number));
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                    find_index(arrayElem, array_register_x, inside_procedure);
                    add_to_assembly("STOREI " + to_string(array_register_x));
                } else if (inside_procedure) { 
                    if (x->procedure_argument) {
                        add_to_assembly("STOREI " + to_string(x->register_number));
                    } else {
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }
                } else {
                    add_to_assembly("STORE " + to_string(x->register_number));
                }
        } else if (z->is_const && z->name == "1") {
                if (op == "DIV") {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                        find_index(arrayElem, array_register_y, inside_procedure);
                        add_to_assembly("LOADI " + to_string(array_register_y));
                    } else if (inside_procedure) { 
                        if (y->procedure_argument) {
                            add_to_assembly("LOADI " + to_string(y->register_number));
                        } else {
                            add_to_assembly("LOAD " + to_string(y->register_number));
                        }
                    } else {
                        add_to_assembly("LOAD " + to_string(y->register_number));
                    }

                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                        find_index(arrayElem, array_register_x, inside_procedure);
                        add_to_assembly("STOREI " + to_string(array_register_x));
                    } else if (inside_procedure) { 
                        if (x->procedure_argument) {
                            add_to_assembly("STOREI " + to_string(x->register_number));
                        } else {
                            add_to_assembly("STORE " + to_string(x->register_number));
                        }
                    } else {
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }


                } else {
                    ValueNode* zero = find_node("0");
                    if (zero == nullptr) {
                        zero = new ValueNode("0", true, 2, true, true);
                        add_to_memory(zero);
                    }
                    add_to_assembly("LOAD " + to_string(zero->register_number));
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                        find_index(arrayElem, array_register_x, inside_procedure);
                        add_to_assembly("STOREI " + to_string(array_register_x));
                    } else if (inside_procedure) { 
                        if (x->procedure_argument) {
                            add_to_assembly("STOREI " + to_string(x->register_number));
                        } else {
                            add_to_assembly("STORE " + to_string(x->register_number));
                        }
                    } else {
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }
                }
        } else if (y->is_const && y->name == "0") {
            ValueNode* zero = find_node("0");
            if (zero == nullptr) {
                zero = new ValueNode("0", true, 2, true, true);
                add_to_memory(zero);
            }
            add_to_assembly("LOAD " + to_string(zero->register_number));
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
                find_index(arrayElem, array_register_x, inside_procedure);
                add_to_assembly("STOREI " + to_string(array_register_x));
            } else if (inside_procedure) { 
                if (x->procedure_argument) {
                    add_to_assembly("STOREI " + to_string(x->register_number));
                } else {
                    add_to_assembly("STORE " + to_string(x->register_number));
                }
            } else {
                add_to_assembly("STORE " + to_string(x->register_number));
            }
        }
        else {
            if (inside_procedure) {
                
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                    find_index(arrayElem, array_register_y, inside_procedure);
                    add_to_assembly("LOADI " + to_string(array_register_y));
                } else if (y->procedure_argument) {
                    add_to_assembly("LOADI " + to_string(y->register_number));
                } else {
                    add_to_assembly("LOAD " + to_string(y->register_number));
                }
            } else {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                    find_index(arrayElem, array_register_y, inside_procedure);
                    add_to_assembly("LOADI " + to_string(array_register_y));
                } else {
                    add_to_assembly("LOAD " + to_string(y->register_number));
                }
            }
            add_to_assembly("STORE " + to_string(dzielna));

            if (inside_procedure) {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                    find_index(arrayElem, array_register_z, inside_procedure);
                    add_to_assembly("LOADI " + to_string(array_register_z));
                } else if (z->procedure_argument) {
                    add_to_assembly("LOADI " + to_string(z->register_number));
                } else {
                    add_to_assembly("LOAD " + to_string(z->register_number));
                }
            } else {
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) {
                    find_index(arrayElem, array_register_z, inside_procedure);
                    add_to_assembly("LOADI " + to_string(array_register_z));
                } else {
                    add_to_assembly("LOAD " + to_string(z->register_number));
                }
            }
            add_to_assembly("STORE " + to_string(dzielnik));
            add_to_assembly("SET " + to_string(code_line + 3));
            add_to_assembly("STORE " + to_string(return_div));

            if (code_line < div_line) {
                add_to_assembly("JUMP " + to_string(div_line - code_line));
            } else {
                add_to_assembly("JUMP -" + to_string(code_line - div_line));
            }

            if (op == "DIV") {
                
                if (inside_procedure) {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        find_index(arrayElem, array_register_x, inside_procedure);
                        add_to_assembly("LOAD " + to_string(wynik));
                        add_to_assembly("STOREI " + to_string(array_register_x));
                    } else if (x->procedure_argument) {
                        add_to_assembly("LOAD " + to_string(wynik));
                        add_to_assembly("STOREI " + to_string(x->register_number));
                    } else {
                        add_to_assembly("LOAD " + to_string(wynik));
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }
                } else {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        find_index(arrayElem, array_register_x, inside_procedure);
                        add_to_assembly("LOAD " + to_string(wynik));
                        add_to_assembly("STOREI " + to_string(array_register_x));
                    } else {
                        add_to_assembly("LOAD " + to_string(wynik));
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }
                }
            } else {
                
                
                if (inside_procedure) {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        find_index(arrayElem, array_register_x, inside_procedure);
                        add_to_assembly("LOAD " + to_string(reszta));
                        add_to_assembly("STOREI " + to_string(array_register_x));
                    } else if (x->procedure_argument) {
                        add_to_assembly("LOAD " + to_string(reszta));
                        add_to_assembly("STOREI " + to_string(x->register_number));
                    } else {
                        add_to_assembly("LOAD " + to_string(reszta));
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }
                } else {
                    if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x))
                    {
                        find_index(arrayElem, array_register_x, inside_procedure);
                        add_to_assembly("LOAD " + to_string(reszta));
                        add_to_assembly("STOREI " + to_string(array_register_x));
                    } else {
                        add_to_assembly("LOAD " + to_string(reszta));
                        add_to_assembly("STORE " + to_string(x->register_number));
                    }
                }
            }
        }
    }
    else {
        if (inside_procedure) {
            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            find_index(arrayElem, array_register_x, inside_procedure);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // WSZYSTKO JEST Z TABLICY
                    find_index(arrayElem, array_register_z, inside_procedure);
                    
                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly(op + "I " + to_string(array_register_z));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                } else { // X I Y Z TABLICY, Z NIE

                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly(op + " " + to_string(z->register_number));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                }
            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // X I Z Z TABLICY, Y NIE
                find_index(arrayElem, array_register_z, inside_procedure);

                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly(op + "I " + to_string(array_register_z));
                add_to_assembly("STOREI " + to_string(array_register_x));
            } else { // TYLKO X Z TABLICY
                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly(op + " " + to_string(z->register_number));
                add_to_assembly("STOREI " + to_string(array_register_x));

            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) { // X NIE Z TABLICY
            find_index(arrayElem, array_register_y, inside_procedure);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // Y I Z Z TABLICY
                find_index(arrayElem, array_register_z, inside_procedure);

                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly(op + "I " + to_string(array_register_z));
                add_to_assembly("STORE " + to_string(x->register_number));
            } else { // TYLKO Y Z TABLICY
                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly(op + " " + to_string(z->register_number));
                add_to_assembly("STORE " + to_string(x->register_number));
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // X I Y NIE Z TABLICY, Z Z TABLICY
            find_index(arrayElem, array_register_z, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_z));
            add_to_assembly(op + " " + to_string(y->register_number));
            add_to_assembly("STORE " + to_string(x->register_number));
        }

            else {
            if (y->procedure_argument) {
                add_to_assembly("LOADI " + to_string(y->register_number));
            } else {
                add_to_assembly("LOAD " + to_string(y->register_number));
            }

            if (z->procedure_argument) {
                add_to_assembly(op + "I " + to_string(z->register_number));
            } else {
                add_to_assembly(op + " " + to_string(z->register_number));
            }

            if (x->procedure_argument) {
                add_to_assembly("STOREI " + to_string(x->register_number));
            } else {
                add_to_assembly("STORE " + to_string(x->register_number));
            }
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {

            find_index(arrayElem, array_register_x, inside_procedure);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) {
                find_index(arrayElem, array_register_y, inside_procedure);
                
                if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // WSZYSTKO JEST Z TABLICY
                    find_index(arrayElem, array_register_z, inside_procedure);
                    
                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly(op + "I " + to_string(array_register_z));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                } else { // X I Y Z TABLICY, Z NIE

                    add_to_assembly("LOADI " + to_string(array_register_y));
                    add_to_assembly(op + " " + to_string(z->register_number));
                    add_to_assembly("STOREI " + to_string(array_register_x));
                }
            } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // X I Z Z TABLICY, Y NIE
                find_index(arrayElem, array_register_z, inside_procedure);

                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly(op + "I " + to_string(array_register_z));
                add_to_assembly("STOREI " + to_string(array_register_x));
            } else { // TYLKO X Z TABLICY
                add_to_assembly("LOAD " + to_string(y->register_number));
                add_to_assembly(op + " " + to_string(z->register_number));
                add_to_assembly("STOREI " + to_string(array_register_x));

            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(y)) { // X NIE Z TABLICY
            find_index(arrayElem, array_register_y, inside_procedure);

            if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // Y I Z Z TABLICY
                find_index(arrayElem, array_register_z, inside_procedure);

                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly(op + "I " + to_string(array_register_z));
                add_to_assembly("STORE " + to_string(x->register_number));
            } else { // TYLKO Y Z TABLICY
                add_to_assembly("LOADI " + to_string(array_register_y));
                add_to_assembly(op + " " + to_string(z->register_number));
                add_to_assembly("STORE " + to_string(x->register_number));
            }

        } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(z)) { // X I Y NIE Z TABLICY, Z Z TABLICY
            find_index(arrayElem, array_register_z, inside_procedure);
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

void IfNode::analyze(bool inside_procedure) {
    condition->analyze(inside_procedure);
    commands->analyze(inside_procedure);
    if (condition->is_negated) {
        len = 1 + commands->len + condition->len;
    } else {
        len = 2 + commands->len + condition->len;
    }

    cout << "if len is " << len << endl;
}

void IfNode::translate(bool inside_procedure) {
    condition->translate(inside_procedure);
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(commands->len + 1)); // wychodzimy z ifa
    } else {
        add_to_assembly(condition->op + " " + to_string(2)); // idziemy do komend
        add_to_assembly("JUMP " + to_string(commands->len + 1)); // wychodzimy z ifa
    }
    commands->translate(inside_procedure);
}

// IfElseNode*********************************************************************************************************
IfElseNode::IfElseNode(ConditionNode* condition, CommandsNode* if_commands, CommandsNode* else_commands) : condition(condition), if_commands(if_commands), else_commands(else_commands) {}

IfElseNode::~IfElseNode() {
    delete condition;
    delete if_commands;
    delete else_commands;
}

void IfElseNode::analyze(bool inside_procedure) {
    condition->analyze(inside_procedure);
    if_commands->analyze(inside_procedure);
    else_commands->analyze(inside_procedure);
    len = 2 + if_commands->len + else_commands->len + condition->len;

    cout << "if else len is " << len << endl;
}

void IfElseNode::translate(bool inside_procedure) {
    condition->translate(inside_procedure);
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(if_commands->len + 2));
        if_commands->translate(inside_procedure);
        add_to_assembly("JUMP " + to_string(else_commands->len + 1));
        else_commands->translate(inside_procedure);
    } else {
        add_to_assembly(condition->op + " " + to_string(else_commands->len + 2));
        else_commands->translate(inside_procedure);
        add_to_assembly("JUMP " + to_string(if_commands->len + 1));
        if_commands->translate(inside_procedure);
    }
}

// WhileNode**********************************************************************************************************
WhileNode::WhileNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

WhileNode::~WhileNode() {
    delete condition;
    delete commands;
}

void WhileNode::analyze(bool inside_procedure) {
    condition->analyze(inside_procedure);
    commands->analyze(inside_procedure);
    if (condition->is_negated) {
        len = commands->len + condition->len + 2;
    } else {
        len = commands->len + condition->len + 3;
    }

    cout << "while len is " << len << endl;
}

void WhileNode::translate(bool inside_procedure) {
    condition->translate(inside_procedure);
    if (condition->is_negated) {
        add_to_assembly(condition->op + " " + to_string(commands->len + 2));
    } else {
        add_to_assembly(condition->op + " " + to_string(2));
        add_to_assembly("JUMP " + to_string(commands->len + 2));
    }
    commands->translate(inside_procedure);
    add_to_assembly("JUMP -" + to_string(len - 1));
}

// RepeatNode*********************************************************************************************************
RepeatNode::RepeatNode(ConditionNode* condition, CommandsNode* commands) : condition(condition), commands(commands) {}

RepeatNode::~RepeatNode() {
    delete condition;
    delete commands;
}

void RepeatNode::analyze(bool inside_procedure) {
    condition->analyze(inside_procedure);
    commands->analyze(inside_procedure);
    if (condition->is_negated) {
        len = commands->len + condition->len + 1;
    } else {
        len = commands->len + condition->len + 2;
    }

    cout << "repeat len is " << len << endl;
}

void RepeatNode::translate(bool inside_procedure) {
    commands->translate(inside_procedure);
    condition->translate(inside_procedure);
    if (condition->is_negated) {
        add_to_assembly(condition->op + " -" + to_string(len - 1));
    } else {
        add_to_assembly(condition->op + " " + to_string(2));
        add_to_assembly("JUMP -" + to_string(len - 1));
        
    }
}

// ForNode************************************************************************************************************
ForNode::ForNode(ValueNode* i, ValueNode* start, ValueNode* end, ValueNode* one, CommandsNode* commands, const string &op) : i(i), start(start), end(end), one(one), commands(commands), op(op) {
    counter = new ValueNode("counter" + to_string(counter_counter++), true, 0, true, false);
    add_to_memory(counter);
}

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

void ForNode::analyze(bool inside_procedure) {
    if (inside_procedure) {

        len = 0;
        
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(end)) {
            len += 6;
        } else if (end->procedure_argument) {
            len += 1;
        } else {
            len += 1;
        }
        
        len += 1;

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(start)) {
            len += 6;
        } else if (start->procedure_argument) {
            len += 1;
        } else {
            len += 1;
        }

        if (i->procedure_argument) {
            len += 1;
        } else {
            len += 1;
        }

        len += 2;
        commands->analyze(inside_procedure);
        len += commands->len;

        if (i->procedure_argument) {
            len += 1;
        } else {
            len += 1;
        }
        
        if (op == "JPOS") {
            len += 1;
        } else {
            len += 1;
        }
        len += 1;
        
    } else {
        len = 0;
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(end)) {
            len += 6;
        } else {
            len += 1;
        }

        len += 1;

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(start)) {
            len += 6;
        } else {
            len += 1;
        } 
        
        len += 3;
        commands->analyze(inside_procedure);
        len += commands->len;
        len += 3;

    }

    cout << "for len is " << len << endl;
}

void ForNode::translate(bool inside_procedure) {

    if (inside_procedure) {

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(end)) {
            find_index(arrayElem, array_register_y, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_y));
        } else if (end->procedure_argument) {
            add_to_assembly("LOADI " + to_string(end->register_number));
        } else {
            add_to_assembly("LOAD " + to_string(end->register_number));
        }
        
        add_to_assembly("STORE " + to_string(counter->register_number));

        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(start)) {
            find_index(arrayElem, array_register_x, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_x));
        } else if (start->procedure_argument) {
            add_to_assembly("LOADI " + to_string(start->register_number));
        } else {
            add_to_assembly("LOAD " + to_string(start->register_number));
        }

        if (i->procedure_argument) {
            add_to_assembly("STOREI " + to_string(i->register_number));
        } else {
            add_to_assembly("STORE " + to_string(i->register_number));
        }

        add_to_assembly("SUB " + to_string(counter->register_number));
        add_to_assembly(op + " " + to_string(commands->len + 4));
        commands->translate(inside_procedure);

        if (i->procedure_argument) {
            add_to_assembly("LOADI " + to_string(i->register_number));
        } else {
            add_to_assembly("LOAD " + to_string(i->register_number));
        }
        
        if (op == "JPOS") {
            add_to_assembly("ADD " + to_string(one->register_number));
        } else {
            add_to_assembly("SUB " + to_string(one->register_number));
        }
        add_to_assembly("JUMP -" + to_string(commands->len + 5));
        
    } else {

        if (ArrayElem* arrayEnd = dynamic_cast<ArrayElem*>(end)) {
            find_index(arrayEnd, array_register_y, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_y));
        } else {
            add_to_assembly("LOAD " + to_string(end->register_number));
        }

        add_to_assembly("STORE " + to_string(counter->register_number));

        if (ArrayElem* arrayStart = dynamic_cast<ArrayElem*>(start)) {
            find_index(arrayStart, array_register_x, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_x));
        } else {
            add_to_assembly("LOAD " + to_string(start->register_number));
        } 

        add_to_assembly("STORE " + to_string(i->register_number));
        add_to_assembly("SUB " + to_string(counter->register_number));
        add_to_assembly(op + " " + to_string(commands->len + 4));
        commands->translate(inside_procedure);
        add_to_assembly("LOAD " + to_string(i->register_number));
        
        if (op == "JPOS") {
            add_to_assembly("ADD " + to_string(one->register_number));
        } else {
            add_to_assembly("SUB " + to_string(one->register_number));
        }
        add_to_assembly("JUMP -" + to_string(commands->len + 5));

    }     
}

// IONode*************************************************************************************************************
IONode::IONode(const string &op, ValueNode* x) : op(op), x(x) {
    len = 1;
}

IONode::~IONode() {
    delete x;
}

void IONode::analyze(bool inside_procedure) {
    ValueNode* one = find_node("1");
    if (one == nullptr) {
        one = new ValueNode("1", true, 2, true, true);
        cout << "at line 1323 one reg is " << one->register_number << endl;
        add_to_memory(one);
    }
    if (inside_procedure) {
        if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            len = 7;
            if (op == "GET") {
                len += 1;
            }
        } else if (x->procedure_argument) {
            if (op == "PUT") {
                len = 2;
            } else {
                len = 3;
            }
        } else {
            len = 1;
        }
    } else if (ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
        ValueNode* one = find_node("1");
        len = 7;
        if (one == nullptr) {
            one = new ValueNode("1", true, 2, true, true);
            cout << "at line 1346 one reg is " << one->register_number << endl;
            add_to_memory(one);
        }
        if (op == "GET") {
            len += 1;
        }
    }

    cout << "io len is " << len << endl;
}

void IONode::translate(bool inside_procedure) {
    if (inside_procedure) {

        if(ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
            find_index(arrayElem, array_register_x, inside_procedure);
            add_to_assembly("LOADI " + to_string(array_register_x));
            add_to_assembly(op + " 0");
            if (op == "GET") {
                add_to_assembly("STOREI " + to_string(array_register_x));
            }
        } else if (x->procedure_argument) {
            add_to_assembly("LOADI " + to_string(x->register_number));
            add_to_assembly(op + " 0");
            if (op == "GET") {
                add_to_assembly("STOREI " + to_string(x->register_number));
            }
        } else {
            add_to_assembly(op + " " + to_string(x->register_number));
        }

        
    } else if(ArrayElem* arrayElem = dynamic_cast<ArrayElem*>(x)) {
        find_index(arrayElem, array_register_x, inside_procedure);
        add_to_assembly("LOADI " + to_string(array_register_x));
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

void CommandsNode::analyze(bool inside_procedure) {
    for (auto command : commands) {
        command->analyze(inside_procedure);
        len += command->len;
    }
}

void CommandsNode::translate(bool inside_procedure) {
    for (auto command : commands) {
        command->translate(inside_procedure);
    }
}
// MainNode**********************************************************************************************************
MainNode::MainNode(CommandsNode* commands, bool declarations) : commands(commands), declarations(declarations) {}

MainNode::~MainNode() {
    delete commands;
}

void MainNode::analyze(bool inside_procedure) {
    commands->analyze(inside_procedure);
}

void MainNode::translate(bool inside_procedure) {
    if(is_mul) {
        if (is_div) {
            if (mul_line < div_line) {
                add_mul();
                add_div();
            } else {
                add_div();
                add_mul();
            }
        } else {
            add_mul();
        }
    } else {
        if (is_div) {
            add_div();
        }
    }
    for (auto val : memory) {
        val->translate(inside_procedure);
    }
    commands->translate(inside_procedure);
    add_to_assembly("HALT");
}

// ProcedureHeader*****************************************************************************************************
ProcedureHeader::ProcedureHeader(const string &name, vector<ValueNode*>* args) : name(name), arguments(args) {}

// ProcedureNode*******************************************************************************************************
ProcedureNode::ProcedureNode(ProcedureHeader* header, CommandsNode* commands) : header(header), commands(commands) {
    len = 0;
    return_address = new ValueNode("RTRN" + header->name, true, 0, true, true);
    add_to_memory(return_address);
}

ProcedureNode::~ProcedureNode() {
    delete commands;
}

void ProcedureNode::translate(bool inside_procedure) {
    commands->translate(inside_procedure);
    add_to_assembly("RTRN " + to_string(return_address->register_number));
}

void ProcedureNode::analyze(bool inside_procedure) {
    start_line = procedure_line;
    commands->analyze(inside_procedure);
    len = commands->len + 1;
    procedure_line += len;
}

// ProcedureCallNode***************************************************************************************************
ProcedureCallNode::ProcedureCallNode(ProcedureNode* procedure, vector<ValueNode*>* arguments) : procedure(procedure), arguments(arguments) {

}

ProcedureCallNode::~ProcedureCallNode() {
    delete procedure;
    for (auto arg : *arguments) {
        delete arg;
    }
}

void ProcedureCallNode::translate(bool inside_procedure) {
    for (long long i = 0; i < arguments->size(); i++) {
        if (arguments->at(i)->procedure_argument) {
            add_to_assembly("LOAD " + to_string(arguments->at(i)->register_number));
            add_to_assembly("STORE " + to_string(procedure->header->arguments->at(i)->register_number));
        } else {
            if (ProcedureArray* array = dynamic_cast<ProcedureArray*>(procedure->header->arguments->at(i))) {
                ArrayNode* arrayNode = dynamic_cast<ArrayNode*>(arguments->at(i));
                
                add_to_assembly("LOAD " + to_string(arrayNode->register_number));
                add_to_assembly("STORE " + to_string(array->array->register_number));
                add_to_assembly("LOAD " + to_string(arrayNode->start_register));
                add_to_assembly("STORE " + to_string(array->start->register_number));
                
            } else {
                add_to_assembly("SET " + to_string(arguments->at(i)->register_number));
                add_to_assembly("STORE " + to_string(procedure->header->arguments->at(i)->register_number));
            }
        }
        
    }
    add_to_assembly("SET " + to_string(code_line + 3));
    add_to_assembly("STORE " + to_string(procedure->return_address->register_number));
    add_to_assembly("JUMP -" + to_string(code_line - procedure->start_line));
}

void ProcedureCallNode::analyze(bool inside_procedure) {
        len = 0;
        for (long long i = 0; i < arguments->size(); i++) {
        if (arguments->at(i)->procedure_argument) {
            len += 2;
        } else {
            if (ProcedureArray* array = dynamic_cast<ProcedureArray*>(procedure->header->arguments->at(i))) {

                len += 4;
                
            } else {
               len += 2;
            }
        }
        
    }
    len += 3;
}

// ProceduresNode*******************************************************************************************************
ProceduresNode::ProceduresNode() {
    len = 0;
}

ProceduresNode::~ProceduresNode() {
    for (auto procedure : procedures_vector) {
        delete procedure;
    }
}

void ProceduresNode::add_procedure(ProcedureNode* procedure) {
    procedures_vector.push_back(procedure);
}

void ProceduresNode::print() {
    for (auto arg : procedures_vector) {
        cout << arg->header->name << ": " << endl;
        for (auto val : *arg->header->arguments) {
            cout << val->name << endl;
        }
        cout << endl;
    }
}

void ProceduresNode::translate(bool inside_procedure) {
    for (auto procedure : procedures_vector) {
        procedure->translate(inside_procedure);
    }
}

void ProceduresNode::analyze(bool inside_procedure) {
    for (auto procedure : procedures_vector) {
        procedure->analyze(inside_procedure);
        len += procedure->len;
        cout << "procedure length " << len << endl;
    }
    procedures_length += len;
    
}

// ProgramNode********************************************************************************************************
ProgramNode::ProgramNode(ProceduresNode* procedures, MainNode* main) : main(main), procedures(procedures) {}

ProgramNode::~ProgramNode() {
    delete main;
    delete procedures;
}

void ProgramNode::translate(bool inside_procedure) {
    if (procedures_length > 0) {
        add_to_assembly("JUMP " + to_string(procedures_length + 1));
    }
    procedures->translate(true);
    main->translate(false);
}

void ProgramNode::analyze(bool inside_procedure) {
    procedures->analyze(true);
    main->analyze(false);
    if (procedures_vector.size() > 0) {
        cout << "procedures size is " << procedures_vector.size() << endl;
        if (is_mul) {
            if (is_div) {
                if (first_op == "MUL") {
                    div_line = procedures_length - div_length + 1;
                    mul_line = procedures_length - div_length - mul_length + 1;
                } else {
                    mul_line = procedures_length - mul_length + 1;
                    div_line = procedures_length - mul_length - div_length + 1;
                }
            } else {
                mul_line = procedures_length - mul_length + 1;
            }
        } else {
            if (is_div) {
                div_line = procedures_length - div_length + 1;
            }
        }
    } else {
        if (is_mul) {
            if (is_div) {
                if (first_op == "MUL") {
                    div_line = mul_length + 1;
                    mul_line = 1;
                } else {
                    mul_line = div_length + 1;
                    div_line = 1;
                }
            } else {
                mul_line = 1;
            }
        } else {
            if (is_div) {
                div_line = 1;
            }
        }
    }

    cout << "procedures length " << procedures_length << endl;
    cout << "mul line is " << mul_line << endl;
    cout << "div line is " << div_line << endl;
}
