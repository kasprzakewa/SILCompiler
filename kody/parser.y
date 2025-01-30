%{
   #include <iostream>
   #include <vector>
   #include <string>
   #include "nodes.hpp"
   using namespace std;

   extern int yylineno;

   int yywrap() { return 1; }
   int yylex();
   void yyerror(const char *s);

   bool is_in_procedure = false;
   string procedure_name = "";
%}


%locations
%define parse.error verbose

%code requires {
   #include <iostream>
   #include <vector>
   #include <string>
   #include "nodes.hpp"
   using namespace std;
   extern bool is_in_procedure;
}

%union {
   long long ival;
   string *sval;
   MainNode* main;
   CommandsNode* commands;
   CommandNode* command;
   ConditionNode* condition;
   ValueNode* value;
   ProceduresNode* procedures;
   ProgramNode* program;
   vector<ValueNode*>* proc_arguments;
   ProcedureHeader* proc_header;
}

%type <sval> declarations
%type <proc_arguments> args_decl args
%type <proc_header> proc_head
%type <main> main
%type <commands> commands
%type <command> command expression proc_call
%type <condition> condition
%type <value> value identifier number
%type <procedures> procedures
%type <program> program_all
%token <ival> NUM
%token <sval> PID

%token ASSIGN ADD SUB MUL DIV MOD EQ NEQ GT LT GE LE
%token T
%token PROCEDURE IS BEG END PROGRAM IF THEN ELSE ENDIF WHILE ENDWHILE REPEAT UNTIL FOR FROM TO DO ENDFOR DOWNTO READ WRITE
%left ADD SUB
%left MUL DIV MOD
%left EQ NEQ GT LT GE LE
%right UMINUS

%%

program_all:   { 
                  is_in_procedure = true;
               } procedures {
                  is_in_procedure = false;
                  procedure_name = "";
               } main {
                  $$ = new ProgramNode($2, $4);
                  $$->analyze(false);
                  $$->translate(false);
               }

procedures:    procedures PROCEDURE proc_head IS declarations BEG commands END { 
                  $$ = $1;
                  $$->add_procedure(new ProcedureNode($3, $7));
               }
             | procedures PROCEDURE proc_head IS BEG commands END { 
                  $$ = $1;
                  $$->add_procedure(new ProcedureNode($3, $6));   
               }
             | { 
                  $$ = new ProceduresNode();
               }
             ;

main:        PROGRAM IS declarations BEG commands END { 
                  $$ = new MainNode($5, true); 
               }
             | PROGRAM IS BEG commands END { 
                  $$ = new MainNode($4, false); 
               }
             ;

commands:    commands command {
                  $$ = $1;
                  $$->add_child($2);
               }
             | command {
                  $$ = new CommandsNode();
                  $$->add_child($1); 
               }
             ;

command:     identifier ASSIGN expression ';' {
                  if ($1->is_iterator) {
                     std::string error_msg = "Cannot assign value to iterator " + $1->name;
                     yyerror(error_msg.c_str());
                  }
                  $1->initialize();
                  $$ = $3;
                  $$->set_x($1);
               }
             | IF condition THEN commands ELSE commands ENDIF {
                  $$ = new IfElseNode($2, $4, $6); 
               }
             | IF condition THEN commands ENDIF {
                  $$ = new IfNode($2, $4);
               }
             | WHILE condition DO commands ENDWHILE { 
                  $$ = new WhileNode($2, $4);
               }
             | REPEAT commands UNTIL condition ';' { 
                  $$ = new RepeatNode($4, $2);
               }
             | FOR PID FROM value TO value DO {
                  if (is_in_procedure) {
                     ValueNode* i = find_node(procedure_name + *$2);

                     if (i == nullptr) {
                        i = new ValueNode(procedure_name + *$2, true, 0, true, false);
                        add_to_memory(i);
                     } else if (i->is_iterator) {
                        std::string error_msg = "Reusing iterator " + i->name;
                        yyerror(error_msg.c_str());
                     }

                     i->is_iterator = true;
                     i->initialize();
                  } else {
                     ValueNode* i = find_node(*$2);

                     if (i == nullptr) {
                        i = new ValueNode(*$2, true, 0, true, false);
                        add_to_memory(i);
                     } else if (i->is_iterator) {
                        std::string error_msg = "Reusing iterator " + i->name;
                        yyerror(error_msg.c_str());
                     }

                     i->is_iterator = true;
                     i->initialize();
                  }
               } commands ENDFOR {
                  
                  ValueNode* one = find_node("1");

                  if (one == nullptr) {
                     one = new ValueNode("1", true, 2, true, true);
                     add_to_memory(one);
                  }

                  ValueNode* i;

                  if (is_in_procedure) {
                     i = find_node(procedure_name + *$2);
                  } else {
                     i = find_node(*$2);
                  }
                  
                  $$ = new ForNode(i, $4, $6, one, $9, "JPOS"); 
                  i->is_iterator = false;
               }
             | FOR PID FROM value DOWNTO value DO {
                  if (is_in_procedure) {
                     ValueNode* i = find_node(procedure_name + *$2);

                     if (i == nullptr) {
                        i = new ValueNode(procedure_name + *$2, true, 0, true, false);
                        add_to_memory(i);
                     } else if (i->is_iterator) {
                        std::string error_msg = "Reusing iterator " + i->name;
                        yyerror(error_msg.c_str());
                     }

                     i->is_iterator = true;
                     i->initialize();
                  } else {
                     ValueNode* i = find_node(*$2);

                     if (i == nullptr) {
                        i = new ValueNode(*$2, true, 0, true, false);
                        add_to_memory(i);
                     } else if (i->is_iterator) {
                        std::string error_msg = "Reusing iterator " + i->name;
                        yyerror(error_msg.c_str());
                     }

                     i->is_iterator = true;
                     i->initialize();
                  }
               } commands ENDFOR {
                  ValueNode* one = find_node("1");

                  if (one == nullptr) {
                     one = new ValueNode("1", true, 2, true, true);
                     add_to_memory(one);
                  }

                  ValueNode* i;

                  if (is_in_procedure) {
                     i = find_node(procedure_name + *$2);
                  } else {
                     i = find_node(*$2);
                  }

                  $$ = new ForNode(i, $4, $6, one, $9, "JNEG");
                  i->is_iterator = false;
               }
             | proc_call ';' { 
                  $$ = $1;
               }
             | READ identifier ';' { 
                  $2->initialize();
                  $$ = new IONode("GET", $2);
               }
             | WRITE value ';' {
                  $$ = new IONode("PUT", $2);
               }
             ;

proc_head:
            PID '(' {
               ProcedureNode* proc = find_procedure(*$1);
               // if (proc != nullptr) {
               //    std::string error_msg = "Procedure " + *$1 + " already defined";
               //    yyerror(error_msg.c_str());
               // }
               procedure_name = *$1;
            } 
            args_decl ')'
            {
               $$ = new ProcedureHeader(procedure_name, $4);
            }
            ;

proc_call:   PID '(' args ')' {
                  ProcedureNode* proc = find_procedure(*$1);
                  if (proc == nullptr) {
                     std::string error_msg = "Procedure " + *$1 + " not defined";
                     yyerror(error_msg.c_str());
                  }
                  $$ = new ProcedureCallNode(proc, $3);
               }
            ;

declarations:declarations ',' PID {

                  if (is_in_procedure) {
                     ValueNode* val = find_node(procedure_name + *$3);

                     if (val == nullptr) {
                        val = new ValueNode(procedure_name + *$3, false, 0, true, false);
                        add_to_memory(val);
                     } else {
                        std::string error_msg = "Variable " + *$3 + " already declared in procedure " + procedure_name;
                        yyerror(error_msg.c_str());
                     }

                  } else {
                     ValueNode* val = find_node(*$3);

                     if (val == nullptr) {
                        val = new ValueNode(*$3, false, 0, true, false);
                        add_to_memory(val);
                     } else {
                        std::string error_msg = "Variable " + val->name + " already declared";
                        yyerror(error_msg.c_str());
                     }
                  }
               }
             | PID {

                  if (is_in_procedure) {
                     ValueNode* val = find_node(procedure_name + *$1);

                     if (val == nullptr) {
                        val = new ValueNode(procedure_name + *$1, false, 0, true, false);
                        add_to_memory(val);
                     } else {
                        std::string error_msg = "Variable " + *$1 + " already declared in procedure " + procedure_name;
                        yyerror(error_msg.c_str());
                     }
                  } else {
                     ValueNode* val = find_node(*$1);

                     if (val == nullptr) {
                        val = new ValueNode(*$1, false, 0, true, false);
                        add_to_memory(val);
                     } else {
                        std::string error_msg = "Variable " + val->name + " already declared";
                        yyerror(error_msg.c_str());
                     }
                  }
               }
            | declarations ',' PID'['number':'number']' {
               cout << "array from " << $5->name << " to " << $7->name << endl;
               if (is_in_procedure) {
                  ArrayNode* arr = new ArrayNode(procedure_name + *$3, stoll($5->name), stoll($7->name));
                  add_to_memory(arr);

                  ArrayNode* arr2 = find_array(procedure_name + *$3);
               } else {
                  ArrayNode* arr = new ArrayNode(*$3, stoll($5->name), stoll($7->name));
                  add_to_memory(arr);

                  ArrayNode* arr2 = find_array(*$3);
               }
            }
            | PID'['number':'number']' {
               cout << "array from " << $3->name << " to " << $5->name << endl;
               if (is_in_procedure) {
                  ArrayNode* arr = new ArrayNode(procedure_name + *$1, stoll($3->name), stoll($5->name));
                  add_to_memory(arr);

                  ArrayNode* arr2 = find_array(procedure_name + *$1);
               } else {
               ArrayNode* arr = new ArrayNode(*$1, stoll($3->name), stoll($5->name));
               add_to_memory(arr);

               ArrayNode* arr2 = find_array(*$1);
               }
            }
             ;

args_decl:   args_decl ',' PID {
                  ValueNode* val = find_node(procedure_name + *$3);
                  
                  if (val == nullptr) {
                     val = new ValueNode(procedure_name + *$3, true, 0, true, false); 
                     val->procedure_argument = true;
                     val->procedure_name = procedure_name;
                     add_to_memory(val);
                     $$ = $1;
                     $$->push_back(val);
                  } else {
                     std::string error_msg = "Variable " + *$3 + " already declared in procedure " + procedure_name;
                     yyerror(error_msg.c_str());
                  }
                  
               }
             | PID {
                  ValueNode* val = find_node(procedure_name + *$1);

                  if (val == nullptr) {
                     val = new ValueNode(procedure_name + *$1, true, 0, true, false); 
                     val->procedure_argument = true;
                     val->procedure_name = procedure_name;
                     add_to_memory(val);
                     $$ = new vector<ValueNode*>();
                     $$->push_back(val);
                  } else {
                     std::string error_msg = "Variable " + *$1 + " already declared in procedure " + procedure_name;
                     yyerror(error_msg.c_str());
                  }
               }
            | args_decl ',' T PID {

               ProcedureArray* arr = find_procedure_array(procedure_name + *$4);
               ValueNode* val = find_node(procedure_name + *$4);
               ArrayNode* arr2 = find_array(procedure_name + *$4);

               if (val != nullptr || arr != nullptr || arr2 != nullptr) {
                  std::string error_msg = "Variable " + *$4 + " already declared in procedure " + procedure_name;
                  yyerror(error_msg.c_str());
               } else {
               
                  arr = new ProcedureArray(procedure_name + *$4, procedure_name);
                  $$ = $1;
                  $$->push_back(arr);

               }
            }
            | T PID {
               ProcedureArray* arr = find_procedure_array(procedure_name + *$2);
               ValueNode* val = find_node(procedure_name + *$2);
               ArrayNode* arr2 = find_array(procedure_name + *$2);

               if (val != nullptr || arr != nullptr || arr2 != nullptr) {
                  std::string error_msg = "Variable " + *$2 + " already declared in procedure " + procedure_name;
                  yyerror(error_msg.c_str());
               } else {
                  arr = new ProcedureArray(procedure_name + *$2, procedure_name);
                  $$ = new vector<ValueNode*>();
                  $$->push_back(arr);
               }
            }
             ;

args:        args ',' PID { 

                  if (is_in_procedure) {
                     ValueNode* val = find_node(procedure_name + *$3);

                     if (val->is_iterator) {
                        std::string error_msg = "Cannot pass iterator " + val->name + " as argument";
                        yyerror(error_msg.c_str());
                     }
                     if(val == nullptr) {
                        std::string error_msg = "Variable " + *$3 + " not declared";
                        yyerror(error_msg.c_str());
                     }
                     val->initialize();
                     $$ = $1;
                     $$->push_back(val);
                  } else {
                     ValueNode* val = find_node(*$3);

                     if (val->is_iterator) {
                        std::string error_msg = "Cannot pass iterator " + val->name + " as argument";
                        yyerror(error_msg.c_str());
                     }
                     if(val == nullptr) {
                        std::string error_msg = "Variable " + *$3 + " not declared";
                        yyerror(error_msg.c_str());
                     }
                     val->initialize();
                     $$ = $1;
                     $$->push_back(val); 
                  }
               }
             | PID {

                  if (is_in_procedure) {
                     ValueNode* val = find_node(procedure_name + *$1);

                     if (val->is_iterator) {
                        std::string error_msg = "Cannot pass iterator " + val->name + " as argument";
                        yyerror(error_msg.c_str());
                     }
                     if(val == nullptr) {
                        std::string error_msg = "Variable " + *$1 + " not declared";
                        yyerror(error_msg.c_str());
                     } 
                     val->initialize();
                     $$ = new vector<ValueNode*>();
                     $$->push_back(val);
                  } else {
                     ValueNode* val = find_node(*$1);

                     if (val->is_iterator) {
                        std::string error_msg = "Cannot pass iterator " + val->name + " as argument";
                        yyerror(error_msg.c_str());
                     }
                     if(val == nullptr) {
                        std::string error_msg = "Variable " + *$1 + " not declared";
                        yyerror(error_msg.c_str());
                     }
                     val->initialize();
                     $$ = new vector<ValueNode*>();
                     $$->push_back(val); 
                  }
               }
             ;

expression:  value {

                  $$ = new AssignNode("ASSIGN", nullptr, $1, nullptr);
               }
             | value ADD value {
                  $$ = new AssignNode("ADD", nullptr, $1, $3);
               }
             | value SUB value {
                  $$ = new AssignNode("SUB", nullptr, $1, $3);
               }
             | value MUL value {
                  $$ = new AssignNode("MUL", nullptr, $1, $3);

                  ValueNode* zero = find_node("0");
                  if (zero == nullptr) {
                        zero = new ValueNode("0", true, 2, true, true);
                        add_to_memory(zero);
                  }

                  ValueNode* one = find_node("1");
                  if (one == nullptr) {
                        one = new ValueNode("1", true, 2, true, true);
                        add_to_memory(one);
                  }
               }
             | value DIV value {
                  $$ = new AssignNode("DIV", nullptr, $1, $3);
               }
             | value MOD value {
                  $$ = new AssignNode("MOD", nullptr, $1, $3);
               }
               ;

condition:   value EQ value { 
                  $$ = new ConditionNode("JZERO", $1, $3, false);
               }
             | value NEQ value { 
                  $$ = new ConditionNode("JZERO", $1, $3, true); 
               }
             | value GT value { 
                  $$ = new ConditionNode("JPOS", $1, $3, false);
               }
             | value LT value {
                  $$ = new ConditionNode("JNEG", $1, $3, false);
               }
             | value GE value { 
                  $$ = new ConditionNode("JNEG", $1, $3, true);
               }
             | value LE value { 
                  $$ = new ConditionNode("JPOS", $1, $3, true);
               }
               ;

value:       number {
                  $$ = $1;
               }
             | identifier {
                  if (!$1->initialized) {
                     std::string error_msg = "Variable " + $1->name + " not initialized";
                     yyerror(error_msg.c_str());
                  }
                  $$ = $1;
               }
               ;

number:      NUM {
                  $$ = find_node(to_string($1));

                  if ($$ == nullptr) {
                     $$ = new ValueNode(to_string($1), true, 2, true, true);
                     $$->initialize();
                     add_to_memory($$);
                     $$->set_value($1);
                  }
               }
            | SUB NUM %prec UMINUS {
                  $$ = find_node(to_string(-$2));

                  if ($$ == nullptr) {
                     $$ = new ValueNode(to_string(-$2), true, 2, true, true);
                     $$->initialize();
                     add_to_memory($$);
                     $$->set_value(-$2);
                  }
               }

identifier:  PID {
                  if (is_in_procedure) {
                     $$ = find_node(procedure_name + *$1);

                     if ($$ == nullptr) {
                        ProcedureArray* arrp = find_procedure_array(procedure_name + *$1);
                        if (arrp != nullptr) {
                           std::string error_msg = "Cannot use array " + *$1 + " as variable";
                           yyerror(error_msg.c_str());
                        }
                        std::string error_msg = "Variable " + *$1 + " not declared";
                        yyerror(error_msg.c_str());
                     } 
                     if (ArrayNode* arr = dynamic_cast<ArrayNode*>($$)) {
                        std::string error_msg = "Cannot use array " + *$1 + " as variable";
                        yyerror(error_msg.c_str());
                     }
                     
                  } else {
                     $$ = find_node(*$1);

                     if ($$ == nullptr) {
                        std::string error_msg = "Variable " + *$1 + " not declared";
                        yyerror(error_msg.c_str());
                     }
                     if (ArrayNode* arr = dynamic_cast<ArrayNode*>($$)) {
                        std::string error_msg = "Cannot use array " + *$1 + " as variable";
                        yyerror(error_msg.c_str());
                     }
                  } 

               }
               | PID'['PID']' {

                  if (is_in_procedure) {

                     ProcedureArray* arrp = find_procedure_array(procedure_name + *$1);

                     if (arrp == nullptr) {

                        ArrayNode* arr = find_array(procedure_name + *$1);

                        if (arr == nullptr) {
                           ValueNode* val = find_node(procedure_name + *$1);
                           if (val != nullptr) {
                              std::string error_msg = "Variable " + *$1 + " is not an array";
                              yyerror(error_msg.c_str());
                           }
                           std::string error_msg = "Array " + *$1 + " not declared";
                           yyerror(error_msg.c_str());
                        }

                        ValueNode* index = find_node(procedure_name + *$3);

                        if (index == nullptr) {
                           std::string error_msg = "Variable " + *$3 + " not declared";
                           yyerror(error_msg.c_str());
                        }
                        if (!index->initialized) {
                           std::string error_msg = "Variable " + *$3 + " not initialized";
                           yyerror(error_msg.c_str());
                        }
                        
                        string arr_name = arr->name + "[" + index->name + "]";
                        $$ = new ArrayElem(arr_name, arr, index);
                        

                     } else {
                        
                        ValueNode* index = find_node(procedure_name + *$3);

                        if (index == nullptr) {
                           std::string error_msg = "Variable " + *$3 + " not declared";
                           yyerror(error_msg.c_str());
                        }
                        if (!index->initialized) {
                           std::string error_msg = "Variable " + *$3 + " not initialized";
                           yyerror(error_msg.c_str());
                        }

                        string arr_name = arrp->array->name + "[" + procedure_name + *$3 + "]";
                        $$ = new ArrayElem(arr_name, arrp, index);

                     }

                  } else {

                     ArrayNode* arr = find_array(*$1);

                     if (arr == nullptr) {
                        ValueNode* val = find_node(*$1);
                        if (val != nullptr) {
                           std::string error_msg = "Variable " + *$1 + " is not an array";
                           yyerror(error_msg.c_str());
                        }
                        std::string error_msg = "Array " + *$1 + " not declared";
                        yyerror(error_msg.c_str());
                     }

                     ValueNode* index = find_node(*$3);

                     if (index == nullptr) {
                        std::string error_msg = "Variable " + *$3 + " not declared";
                        yyerror(error_msg.c_str());
                     }
                     if (!index->initialized) {
                        std::string error_msg = "Variable " + *$3 + " not initialized";
                        yyerror(error_msg.c_str());
                     }

                     string arr_name = arr->name + "[" + index->name + "]";
                     $$ = new ArrayElem(arr_name, arr, index);

                  }
                  
               }
               | PID'['number']' {
                  if (is_in_procedure) {

                     ProcedureArray* arr = find_procedure_array(procedure_name + *$1);

                     if (arr == nullptr) {
                        ArrayNode* arr = find_array(procedure_name + *$1);

                        if (arr == nullptr) {
                           ValueNode* val = find_node(procedure_name + *$1);
                           if (val != nullptr) {
                              std::string error_msg = "Variable " + *$1 + " is not an array";
                              yyerror(error_msg.c_str());
                           }
                           std::string error_msg = "Array " + *$1 + " not declared";
                           yyerror(error_msg.c_str());
                        }

                        string arr_name = arr->name + "[" + $3->name + "]";
                        $$ = new ArrayElem(arr_name, arr, $3);
                     } else {
                        string arr_name = arr->name + "[" + $3->name + "]";
                        $$ = new ArrayElem(arr_name, arr, $3);
                        cout << arr->array->register_number;
                     }

                  } else {

                     ArrayNode* arr = find_array(*$1);

                     if (arr == nullptr) {
                        ValueNode* val = find_node(*$1);
                        if (val != nullptr) {
                           std::string error_msg = "Variable " + *$1 + " is not an array";
                           yyerror(error_msg.c_str());
                        }
                        std::string error_msg = "Array " + *$1 + " not declared";
                        yyerror(error_msg.c_str());
                     }

                     long long reg = stoll($3->name) - arr->start + arr->register_number;
                     string arr_name = arr->name + "[" + $3->name + "]";
                     $$ = new ArrayElem(arr_name, arr, $3);

                  }
               }
             ;
%%

int main() {
   yyparse();
   print_memory();
   cout << endl;
   print_assembly();
   save_assembly_to_file("output.txt");
   return 0;
}

void yyerror(const char *str)
{
   fprintf(stderr, "\033[0;31mERROR AT LINE %d: %s\033[0m\n", yylineno - 1, str);
   exit(EXIT_FAILURE);
}